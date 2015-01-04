#include "os/OS.h"
#include "icc/Reactor.h"
#include "icc/Connector.h"
#include "icc/SelectReactor.h"
#include "icc/InetAddr.h"
#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
#include "icc/Synch.h"
#include "icc/Log.h"

#include "SubscribeClient.h"
#include "Msg.h"
#include "MsgProtocol.h"

#include "MsgConnectorImpl.h"

#include <string>
#include <iostream>
#include <memory> // for auto_ptr
#include <sstream>

using namespace std;

ThreadMutex SubscribeClient::idInfofileMutex;
string SubscribeClient::infoFileName(SubscribeClient::getProgName() + "ClientIdInfo");

int SubscribeClient::keepAliveTryTimes = 3;
int SubscribeClient::keepAliveTimeSpan = 10;

SubscribeClientHandler::SubscribeClientHandler()
:subscribeClient(0), tmpMsg(0)
{
}

SubscribeClientHandler::~SubscribeClientHandler()
{
  assert(tmpMsg == 0);

  if(subscribeClient != 0) {
    subscribeClient->handler = 0;
  }
}

int SubscribeClientHandler::open(void *) {
  if (super::open(0) == -1)
    return -1;

  return 0;
}

int SubscribeClientHandler::close(void *) {
  return 0;
}

int SubscribeClientHandler::handleInput(ACE_HANDLE fd) {
  assert(subscribeClient != 0);
  subscribeClient->unackedKeepAliveProbes = 0;
  subscribeClient->peerLastActiveTime = time(0);

  if(tmpMsg == 0) {
    tmpMsg = new Msg;
  }
  int result = MsgProtocol::parse(&stream, tmpMsg);
  if (result == -1) {
    ICC_ERROR("SubscribeClientHandler::handleInput error: %s", OS::strerror(OS::map_errno(OS::last_error())));
    subscribeClient->connectionErrorPending = true;
    delete tmpMsg;
    tmpMsg = 0;
    return -1;
  } else if(result == 1) {
    return 0;
  }

  assert(result == 0); // parse complete done
  auto_ptr<Msg> msg(tmpMsg);
  tmpMsg = 0;

  switch(msg->messageType) {
    case Msg_Subscribe_OK:
      SubscribeClient::keepAliveTryTimes = msg->keepAliveTryTimes;
      SubscribeClient::keepAliveTimeSpan = msg->keepAliveTimeSpan;
//      ICC_ERROR("recved keepAliveTryTimes = %d, keepAliveTimeSpan = %d", SubscribeClient::keepAliveTryTimes, SubscribeClient::keepAliveTimeSpan);
//      subscribeClient->enableKeepAlive();
      onSubScribeTopicOk(msg->topic, msg->subscriberId);
      break;
    case Msg_UnSubscribe_OK:
      onUnSubScribeTopicOk(msg->topic);
      break;
    case Msg_KeepAlive_ACK:
//      ICC_INFO("SubscribeClientHandler::handleInput, recved Msg_KeepAlive_ACK");
      break;
    case Msg_Event:
      subscribeClient->handleEvent(msg->is);
      break;

    default:
      ICC_ERROR("SubscribeClientHandler::handleInput error!!!, unknown message type: %d", msg->messageType);
      break;
  }

  return 0;
}

int SubscribeClientHandler::handleClose(ACE_HANDLE h, ReactorMask mask) {
  ICC_ERROR("SubscribeClientHandler::handleClose\n");
  subscribeClient->handler = 0; // self will be deleted by super::handleClose()
  return super::handleClose(h, mask);
}

void SubscribeClientHandler::onSubScribeTopicOk(const std::string& topic, const std::string& clientId) {
  assert(topic != "");
//  assert(clientId != "0");
//  ICC_DEBUG("SubscribeClientHandler::onSubScribeTopicOk, topic: %s, clientId: %s", topic.c_str(), clientId.c_str());

  ACE_GUARD(ThreadMutex, guard, subscribeClient->mutex);

  if(subscribeClient->toSubcribeTopics.find(topic) == subscribeClient->toSubcribeTopics.end()) {
    ICC_ERROR("SubscribeClientHandler::onSubScribeTopicOk error!!!!, not find topic %s in toSubcribeTopics", topic.c_str());
    ICC_ERROR("subscribeClient->toSubcribeTopics's size: %d", subscribeClient->toSubcribeTopics.size());
    for(set<string>::iterator it = subscribeClient->toSubcribeTopics.begin();
        it != subscribeClient->toSubcribeTopics.end();
        ++it)
    {
      ICC_ERROR("member: %s", it->c_str());
    }
    assert(false);
  }
  assert(subscribeClient->toSubcribeTopics.find(topic) != subscribeClient->toSubcribeTopics.end());
  subscribeClient->toSubcribeTopics.erase(topic);
  assert(subscribeClient->subcribedTopics.find(topic) == subscribeClient->subcribedTopics.end());
  subscribeClient->subcribedTopics.insert(topic);

  if(clientId == "0") { // broker disabled buffering
    // clear info file
    ACE_GUARD(ThreadMutex, guard, SubscribeClient::idInfofileMutex);
    FILE* fp = OS::fopen(SubscribeClient::infoFileName.c_str(), "r");
    if(fp != 0) {
      OS::fclose(fp);
//      ICC_ERROR("clearing content of file: %s", SubscribeClient::infoFileName.c_str());
      fp = fopen(SubscribeClient::infoFileName.c_str(), "wb+");
      OS::fclose(fp);
    }
  } else if(subscribeClient->id == "0") {
    subscribeClient->id = clientId;
    ACE_GUARD(ThreadMutex, guard, SubscribeClient::idInfofileMutex);
    ofstream ofs(SubscribeClient::infoFileName.c_str());
    ofs << clientId;
  }

  ICC_DEBUG("Subscribe topic %s success, id: %s", topic.c_str(), clientId.c_str());
}

void SubscribeClientHandler::onUnSubScribeTopicOk(const std::string& topic) {
  assert(topic != "");
  ACE_GUARD(ThreadMutex, guard, subscribeClient->mutex);

  assert(subscribeClient->subcribedTopics.find(topic) != subscribeClient->subcribedTopics.end());
  subscribeClient->subcribedTopics.erase(topic);

  ICC_DEBUG("UnSubscribe topic %s success", topic.c_str());
}

SubscribeClient::SubscribeClient(Reactor* reactor_)
:handler(0), reactor(reactor_), mutex(), id("0"), peerLastActiveTime(0)
{
  ACE_GUARD(ThreadMutex, guard, idInfofileMutex);
  ifstream ifs(infoFileName.c_str());
  ifs >> id;
}

SubscribeClient::~SubscribeClient() {
  if (this->handler)
    delete this->handler;
  if (this->connector)
    delete this->connector;
}

bool SubscribeClient::init(const std::string& brokerIp, short port) {
  this->brokerIp = brokerIp;
  this->brokerPort = port;
  connectionErrorPending = false;
  keepAliveEnabled = false;
  unackedKeepAliveProbes = 0;

  connector = new MsgConnectorImpl<SubscribeClientHandler, SubscribeClient>(this->reactor, this);

  InetAddr connAddr(brokerPort, brokerIp.c_str());
  if (connector->connect(handler, connAddr) == -1) {
    ICC_ERROR("SubscribeClient::init, connect error: %s", OS::strerror(OS::map_errno(OS::last_error())));
    return false;
  }

  return true;
}

void SubscribeClient::run()  {

  TimeValue* tv = new TimeValue(1);
  while (true) {
    onConnOk();

    while (!connectionErrorPending) {
      if (processAllSubAndUnSub() == true) {
        //LogMsg << "before handle event" << endl;
        reactor->handleEvents(tv);
        //LogMsg << "after handle event" << endl;
      }
    }

    assert(connectionErrorPending);
    onConnError();
    while (reestablishConnection() == false)
      ;
  }
}

bool SubscribeClient::subscribeTopic(const std::string& topic) {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);
  if(subcribedTopics.find(topic) != subcribedTopics.end()) {
    ICC_DEBUG("SubscribeClient::subscribeTopic, topic %s already subscribed successfully", topic.c_str());
    return true;
  }

  toSubcribeTopics.insert(topic);
  toUnSubcribeTopics.erase(topic);
  return true;
}

bool SubscribeClient::unSubscribeTopic(const std::string& topic) {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);
  if(toSubcribeTopics.find(topic) != toSubcribeTopics.end()) {
    toSubcribeTopics.erase(topic);
    return true;
  }

  if(subcribedTopics.find(topic) == subcribedTopics.end()) {
    ICC_ERROR("SubscribeClient::unSubscribeTopic error, topic not subscribed yet");
    return true;
  }

  toUnSubcribeTopics.insert(topic);
  return true;
}

bool SubscribeClient::realSubTopic(const std::string& topic) {
  ICC_DEBUG("Subscribing topic: %s, id: %s", topic.c_str(), id.c_str());
  if(connectionErrorPending) {
    return false;
  }

  assert(handler != 0);

  OutputStream os;
  MsgProtocol::start(Msg_Subscribe, &os);
  os.write_string(topic);
  os.write_string(id);

  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1) {
    ICC_ERROR("SubscribeClient::subscribeTopic, MsgProtocol::send error: %s",
        OS::strerror(OS::map_errno(OS::last_error())));

    connectionErrorPending = true;
    return false;
  }

  return true;
}

bool SubscribeClient::realUnSubTopic(const std::string& topic) {
  if(connectionErrorPending) {
    return false;
  }

  assert(handler != 0);

  OutputStream os;
  MsgProtocol::start(Msg_UnSubscribe, &os);
  os.write_string(topic);
  os.write_string(id);

  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1) {
    ICC_ERROR("SubscribeClient::unSubscribeTopic, MsgProtocol::send error: %s",
        OS::strerror(OS::map_errno(OS::last_error())));

    return false;
  }

  return true;
}

bool SubscribeClient::processAllSubAndUnSub() {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, false);
  for(set<string>::iterator it = toSubcribeTopics.begin(); it != toSubcribeTopics.end(); ++it) {
    if(realSubTopic(*it) == false) {
      connectionErrorPending = true;
      return false;
    }
  }

  for(set<string>::iterator it = toUnSubcribeTopics.begin(); it != toUnSubcribeTopics.end(); ++it) {
    if(realUnSubTopic(*it) == false) {
      connectionErrorPending = true;
      return false;
    }
  }

  return true;
}

void SubscribeClient::onConnOk() {
  assert(connectionErrorPending == false);
  enableKeepAlive();
  onConnectionEstablished();
}

void SubscribeClient::onConnError() {
  assert(connectionErrorPending == true);
  disableKeepAlive();
  onConnectionError();

  ACE_GUARD(ThreadMutex, guard, mutex);
  for(set<string>::iterator it = subcribedTopics.begin(); it != subcribedTopics.end(); ++it) {
    if(toUnSubcribeTopics.find(*it) == toUnSubcribeTopics.end()) {
      toSubcribeTopics.insert(*it);
    }
  }

  subcribedTopics.clear();
}

/////////////////////////////////////////////////////
bool SubscribeClient::sendKeepAliveProbe() {
  if (handler == 0)
    return false;

  if(time(0) - peerLastActiveTime < keepAliveTimeSpan) {
    return true;
  }

  if (++unackedKeepAliveProbes > keepAliveTryTimes) {
    return false;
  }

  OutputStream os;
  MsgProtocol::start(Msg_KeepAlive_Probe, &os);
  std::string t("keepAlive");
  os.write_string(t);

//  ICC_INFO("SubscribeClient::sendKeepAliveProbe, sending");
  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1)
    return false;

  return true;
}

string SubscribeClient::getProgName() {
  pid_t pid = OS::getpid();

  string result;

  stringstream cmd;
  cmd << "ps -ef | grep -v grep | grep " << pid << " | awk \'{ print $8 }\' > " << pid;
  string tmp = cmd.str();
  OS::system(cmd.str().c_str());

  stringstream pidStr;
  pidStr << pid;
  ifstream ifs(pidStr.str().c_str());
  string fullPathName;
  ifs >> fullPathName;
  stringstream rmCmd;
  rmCmd << "rm " << pid;
  OS::system(rmCmd.str().c_str());

  size_t pos = fullPathName.rfind('/');
  result = fullPathName.substr(pos + 1);

  return result;
}

bool SubscribeClient::reestablishConnection() {
  ICC_ERROR("SubscribeClient::reestablishConnection()\n");

  connectionErrorPending = false;
  unackedKeepAliveProbes = 0;
  if (handler != NULL) {
    reactor->removeHandler(handler, EventHandler::ALL_EVENTS_MASK);
    delete handler;
    handler = NULL;
  }

  InetAddr connAddr(brokerPort, brokerIp.c_str());
  while (true) {
    delete connector;
    connector = new MsgConnectorImpl<SubscribeClientHandler, SubscribeClient>(this->reactor, this);
    if (connector->connect(handler, connAddr) == 0){
      assert(handler != 0);
      break;
    }

    OS::sleep(5);
  }

  InetAddr ia;
  handler->getStream().getLocalAddr(ia);
  ICC_DEBUG("Connection reestablished, handle: %d, local port: %d\n",handler->getHandle(), ia.getPortNumber());
  return true;
}


