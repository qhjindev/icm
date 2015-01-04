#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
#include "icc/Synch.h"
#include "icc/Log.h"

#include "MsgClient.h"
#include "Msg.h"
#include "MsgProtocol.h"

#include "MsgConnectorImpl.h"

#include <string>
#include <iostream>
#include <memory> // for auto_ptr
#include <sstream>

using namespace std;

int MsgClient::keepAliveTryTimes = 3;
int MsgClient::keepAliveTimeSpan = 10;

MsgClientHandler::MsgClientHandler() :
    msgClient(0), tmpMsg(0) {
}

MsgClientHandler::~MsgClientHandler() {
  assert(tmpMsg == 0);

  if (msgClient != 0) {
    msgClient->handler = 0;
  }
}

int MsgClientHandler::open(void *) {
  if (super::open(0) == -1)
    return -1;

  //Block mode to avoid EWOULDBLOCK
  getStream().disable(ACE_NONBLOCK);

  return 0;
}

int MsgClientHandler::close(void *) {
  return 0;
}

int MsgClientHandler::handleInput(ACE_HANDLE fd) {
  assert(msgClient != 0);
  msgClient->unackedKeepAliveProbes = 0;
  msgClient->peerLastActiveTime = time(0);

  if (tmpMsg == 0) {
    tmpMsg = new Msg;
  }
  int result = MsgProtocol::parse(&stream, tmpMsg);
  if (result == -1) {
    LogError << "MsgClientHandler::handleInput error: " << perr << endl;
    msgClient->connectionErrorPending = true;
    delete tmpMsg;
    tmpMsg = 0;
    return -1;
  } else if (result == 1) {
    return 0;
  }

  assert(result == 0);
  // parse complete done
  auto_ptr<Msg> msg(tmpMsg);
  tmpMsg = 0;

  switch (msg->messageType) {

  case Msg_KeepAlive_ACK:
//      ICC_INFO("SubscribeClientHandler::handleInput, recved Msg_KeepAlive_ACK");
    break;
  case Msg_Event:
    msgClient->handleEvent(msg->is);
    break;

  default:
    LogError << "MsgClientHandler::handleInput error!!!, unknown message type: " << msg->messageType << endl;
    break;
  }

  return 0;
}

int MsgClientHandler::handleClose(ACE_HANDLE h, ReactorMask mask) {
  LogError << "MsgClientHandler::handleClose" << endl;
  msgClient->handler = 0; // self will be deleted by super::handleClose()
  return super::handleClose(h, mask);
}

MsgClient::MsgClient(Reactor* reactor_) :
    handler(0), reactor(reactor_), peerLastActiveTime(0) {
  connector = new MsgConnectorImpl<MsgClientHandler, MsgClient>(this->reactor, this);
}

MsgClient::~MsgClient() {
  if (this->handler)
    delete this->handler;
  if (this->connector)
    delete this->connector;
}

int MsgClient::publish(OutputStream& os) {

  if(handler == 0) {
    LogError << "err publish, MsgClientHandler = 0" << endl;
    return -1;
  }

  MsgProtocol::check(os);

  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1) {
    // connection broke
    LogError << "send msg err:" << perr << endl;
    return -1;
  }

  return 0;
}

bool MsgClient::init(const std::string& _serverIp, short port, int _reconnectInterval) {
  this->serverIp = _serverIp;
  this->serverPort = port;
  this->reconnectInterval = _reconnectInterval;
  connectionErrorPending = false;
  keepAliveEnabled = false;
  unackedKeepAliveProbes = 0;

  //connector = new MsgConnectorImpl<MsgClientHandler, MsgClient>(this->reactor, this);

  InetAddr connAddr(serverPort, _serverIp.c_str());
  if (connector->connect(handler, connAddr) == -1) {
    //LogError << "MsgClient::init, connect error: " << perr << endl;
    return false;
  }

  activate();

  return true;
}

int MsgClient::svc() {

  TimeValue* tv = new TimeValue(1);
  while (true) {
    onConnOk();

    while (!connectionErrorPending) {
    //while (true) {
      reactor->handleEvents(tv);
    }

    assert(connectionErrorPending);
    onConnError();
    while (reestablishConnection() == false)
      ;
  }

  return 0;
}

void MsgClient::onConnOk() {
  assert(connectionErrorPending == false);
  enableKeepAlive();
  onConnectionEstablished();
}

void MsgClient::onConnError() {
  assert(connectionErrorPending == true);
  disableKeepAlive();
  onConnectionError();
}

/////////////////////////////////////////////////////
bool MsgClient::sendKeepAliveProbe() {
  if (handler == 0)
    return false;

  if (time(0) - peerLastActiveTime < keepAliveTimeSpan) {
    return true;
  }

  if (++unackedKeepAliveProbes > keepAliveTryTimes) {
    return false;
  }

  OutputStream os;
  MsgProtocol::start(Msg_KeepAlive_Probe, &os);
  std::string t("keepAlive");
  os.write_string(t);

//  ICC_INFO("MsgClient::sendKeepAliveProbe, sending");
  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1)
    return false;

  return true;
}

bool MsgClient::reestablishConnection() {
  LogDebug << "MsgClient start reestablishConnection" << endl;

  connectionErrorPending = false;
  unackedKeepAliveProbes = 0;
  if (handler != NULL) {
    reactor->removeHandler(handler, EventHandler::ALL_EVENTS_MASK);
    delete handler;
    handler = NULL;
  }

  InetAddr connAddr(serverPort, serverIp.c_str());
  while (true) {
    //delete connector;
    //connector = new MsgConnectorImpl<MsgClientHandler, MsgClient>(this->reactor, this);
    if (connector->connect(handler, connAddr) == 0) {
      assert(handler != 0);
      break;
    }

    OS::sleep(reconnectInterval);
  }

  InetAddr ia;
  handler->getStream().getLocalAddr(ia);
  ICC_DEBUG("Connection reestablished, handle: %d, local port: %d\n", handler->getHandle(), ia.getPortNumber());
  return true;
}

