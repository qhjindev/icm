#include "icc/Log.h"
#include "PublishClient.h"
#include "MsgConnectorImpl.h"
#include <algorithm>
#include <memory>

using namespace std;

ThreadMutex PublishClient::mutex;
bool PublishClient::connectionErrorPending(false);

int PublishClient::keepAliveTryTimes = 3;
int PublishClient::keepAliveTimeSpan = 10;
bool PublishClient::enableBuffering(false);

PublishClientHandler::~PublishClientHandler() {
  if (publishClient != 0) {
    publishClient->handler = 0;
  }
}
int PublishClientHandler::open(void *) {
  if (SvcHandler::open(0) == -1)
    return -1;

  return 0;
}

int PublishClientHandler::close(void *) {
  return 0;
}

int PublishClientHandler::handleInput(ACE_HANDLE fd) {
  assert(publishClient != 0);
  publishClient->unackedKeepAliveProbes = 0;

  Msg tmpMsg;
  int result = MsgProtocol::parse(&stream, &tmpMsg);
  if (result == -1) {
    ICC_ERROR("PublishClientHandler::handleInput error: %s", OS::strerror(OS::map_errno(OS::last_error())));
    return -1;
  } else if (result == 1) {
    ICC_ERROR("PublishClientHandler::handleInput error, half read!!");
    return -1;
  }

  assert(result == 0);
  // parse complete done

  if (tmpMsg.messageType != Msg_KeepAlive_ACK) {
    ICC_ERROR("PublishClientHandler::handleInput error!!!, unknown message type: %d", tmpMsg.messageType);
    return -1;
  }

  return 0;
}

int PublishClientHandler::handleClose(ACE_HANDLE h, ReactorMask mask) {
  ICC_ERROR("PublishClientHandler::handleClose\n");
  publishClient->setConnectionError(true);
  return SvcHandler::handleClose(h, mask);
}

bool PublishClient::getConnectionError() {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);
  return connectionErrorPending;
}

void PublishClient::setConnectionError(bool arg) {
  ACE_GUARD(ThreadMutex, guard, mutex);
  connectionErrorPending = arg;
}

bool PublishClient::enableKeepAlive() {
//ICC_INFO("enableKeepAlive begin\n");
  keepAliveTryTimes = SubscribeClient::keepAliveTryTimes;
  keepAliveTimeSpan = SubscribeClient::keepAliveTimeSpan;

  if (reactor == 0)
    return false;
  if (keepAliveEnabled == true)
    return true;

  TimeValue tv1(keepAliveTimeSpan);
  TimeValue tv2(keepAliveTimeSpan);
  if (reactor->scheduleTimer(&keepAliveTimeoutHandler, this, tv1, tv2) == -1) {
    ICC_ERROR("enableKeepAlive() failed!! %D\n");
    return false;
  }

  keepAliveEnabled = true;
  return true;
}

bool PublishClient::disableKeepAlive() {
  assert(reactor != NULL);

  reactor->cancelTimer(&keepAliveTimeoutHandler);
  keepAliveEnabled = false;
  return true;
}

int PublishClient::svc(void) {
  TimeValue* tv = new TimeValue(1);
  while (true) {
    onConnOk();
    if(enableBuffering) {
      publishFromBuffer();
    }
    while (!getConnectionError()) {
      reactor->handleEvents(tv);
    }

    assert(getConnectionError() == true);
    onConnError();
    while (reestablishConnectionAndPublishFromBuffer() == false)
      ;

    this->setConnectionError(false);
  }

  return 0;
}

bool PublishClient::init(const std::string& brokerIp, short port) {
  this->brokerIp = brokerIp;
  this->brokerPort = port;
//  connectionErrorPending = false;
  keepAliveEnabled = false;
  unackedKeepAliveProbes = 0;

  connector = new MsgConnectorImpl<PublishClientHandler, PublishClient>(this->reactor, this);

  InetAddr connAddr(brokerPort, brokerIp.c_str());
  if (connector->connect(handler, connAddr) == -1) {
    ICC_ERROR("PublishClient::init, connect error: %s", OS::strerror(OS::map_errno(OS::last_error())));

    setConnectionError(true);
    return false;
  }

  setConnectionError(false);
  registerToBroker();
  activate();
  return true;
}

bool PublishClient::registerToBroker() {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);

  OutputStream os;
  MsgProtocol::start(Msg_Publish, &os);
  std::string t("registerPublisher");
  os.write_string(t);

  //ICC_INFO("sending Msg_Publish");
  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1) {
    ICC_ERROR("PublishClient::registerToBroker error, send Msg_Publish failed");
    return false;
  }

  Msg tmpMsg;
  TimeValue tv(0, 1000);
  OS::sleep(tv);
  result = MsgProtocol::parse(&handler->getStream(), &tmpMsg);
  if (result == -1) {
    OS::sleep(1);
    result = MsgProtocol::parse(&handler->getStream(), &tmpMsg);
    if (result == -1) {
      ICC_ERROR("PublishClient::registerToBroker error, recv Msg_Publish_OK failed");
      return -1;
    }
  }

  if (result == 1) {
    ICC_ERROR("PublishClientHandler::handleInput error, half read!!");
    assert(false);
    return -1;
  }

  assert(result == 0);
  assert(tmpMsg.messageType == Msg_Publish_OK);

  ICC_DEBUG("recved Msg_Publish_OK, keepAliveTryTimes: %d, keepAliveTimeSpan: %d, enableBuffering: %d",
      tmpMsg.keepAliveTryTimes, tmpMsg.keepAliveTimeSpan, tmpMsg.enableBuffering);
  PublishClient::keepAliveTryTimes = tmpMsg.keepAliveTryTimes;
  PublishClient::keepAliveTimeSpan = tmpMsg.keepAliveTimeSpan;
  PublishClient::enableBuffering = tmpMsg.enableBuffering;

  return true;
}

int PublishClient::publish(OutputStream& os) {
  assert(brokerIp != "");
  MsgProtocol::check(os);

  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);
  if (connectionErrorPending == false) {
    int result = MsgProtocol::send(&handler->getStream(), &os);
    if (result == -1) {
      // connection broke
      connectionErrorPending = true;
    }
  }

  if (enableBuffering  && connectionErrorPending) {
    ICC_ERROR("PublishClient::publish, connection error, now publish to buffer");

    publishToBuffer_i(os);
    return -1;
  }

  //ICC_INFO("PublishClient::publish success");
  return 0;
}

int PublishClient::publishToBuffer_i(OutputStream& os) {
//  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);

  FILE* bufferFile = OS::fopen(msgBufferFilename.c_str(), "ab");
  if (bufferFile == 0) {
//    ICC_INFO("PublishClient::publishToBuffer error: open file %s failed", msgBufferFilename.c_str());
    return -1;
  }

  time_t now = time(0);
  OS::fwrite(&now, sizeof(time_t), 1, bufferFile);
  string osStr = MsgProtocol::toString(os);
  MsgProtocol::check(osStr);
  int dataLength = osStr.size();
  OS::fwrite(&dataLength, sizeof(int), 1, bufferFile);
  OS::fwrite(osStr.c_str(), osStr.size(), 1, bufferFile);
  OS::fclose(bufferFile);

  return 0;
}

bool PublishClient::publishFromBuffer() {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);
  if(connectionErrorPending) {
    return false;
  }

  FILE* bufferFile = OS::fopen(msgBufferFilename.c_str(), "rb");
  if (bufferFile == 0) {
//    ICC_DEBUG("PublishClient::publishFromBuffer error: open file %s failed", msgBufferFilename.c_str());
    return true;
  }

  time_t pubTime = 0;
  fread(&pubTime, sizeof(time_t), 1, bufferFile);
  while (!feof(bufferFile) && pubTime != 0) {
    ICC_ERROR("pubTime: %d", pubTime);
    int dataSize = 0;
    fread(&dataSize, sizeof(int), 1, bufferFile);
    assert(dataSize > 0);
    ICC_ERROR("dataSize: %d", dataSize);
    char* data = new char[dataSize];
    fread(data, dataSize, 1, bufferFile);
    string pubData(data, dataSize);
    delete[] data;

    if (time(NULL) - pubTime <= bufferExpireTime) {
      OutputStream* os = MsgProtocol::fromString(pubData);
      auto_ptr<OutputStream*> theOs();
      MsgProtocol::check(*os);
      if (MsgProtocol::send(&handler->getStream(), os) == -1) {
        ICC_ERROR("PublishClient::publishFromBuffer error, conn: %d, fault: %s",
            (int) handler->getHandle(), OS::strerror(OS::map_errno(OS::last_error())));

        connectionErrorPending = true;
        break;
      }
    }

    pubTime = 0;
    fread(&pubTime, sizeof(time_t), 1, bufferFile);
  }

  OS::fclose(bufferFile);

  clearBufferFile_i();

  return true;
}

void PublishClient::clearBufferFile_i() {
  FILE* fp = OS::fopen(msgBufferFilename.c_str(), "wb+");
  OS::fclose(fp);
}

bool PublishClient::sendKeepAliveProbe() {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);
  assert(connectionErrorPending == false);

  if (++unackedKeepAliveProbes > keepAliveTryTimes) {
    ICC_ERROR("PublishClient::sendKeepAliveProbe error, maxTryTimes: %d, tried times: %d", keepAliveTryTimes, unackedKeepAliveProbes - 1);
    return false;
  }

  OutputStream os;
  MsgProtocol::start(Msg_KeepAlive_Probe, &os);
  std::string t("keepAlive");
  os.write_string(t);

  //ICC_INFO("sending keepalive probe");
  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1) {
    ICC_ERROR("PublishClient::sendKeepAliveProbe error, send probe failed");
    return false;
  }

  return true;
}

bool PublishClient::reestablishConnectionAndPublishFromBuffer() {
  ICC_DEBUG("PublishClient::reestablishConnectionAndPublishFromBuffer()\n");
  assert(this->getConnectionError() == true);

  unackedKeepAliveProbes = 0;
  if (handler != NULL) {
    reactor->removeHandler(handler, EventHandler::ALL_EVENTS_MASK);
    delete handler;
    handler = NULL;
  }

  InetAddr connAddr(brokerPort, brokerIp.c_str());
  while (true) {
    delete connector;
    connector = new MsgConnectorImpl<PublishClientHandler, PublishClient>(this->reactor, this);
    if (connector->connect(handler, connAddr) == 0) {
      assert(handler != 0);
      break;
    }

    OS::sleep(5);
  }

  InetAddr ia;
  handler->getStream().getLocalAddr(ia);
  ICC_DEBUG("Connection reestablished, handle: %d, local port: %d\n", handler->getHandle(), ia.getPortNumber());

  setConnectionError(false);

  if (enableBuffering) {
    return publishFromBuffer();
  }

  return true;
}

