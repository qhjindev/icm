#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
//#include "icc/Synch.h"
#include "icc/Log.h"

#include "icm/IcmStream.h"
#include "icc/Acceptor.h"
#include "icc/Reactor.h"

#include "MsgServer.h"
#include "MsgProtocol.h"
#include "Msg.h"
#include "MsgAcceptorImpl.h"
#include <iostream>

using namespace std;

MsgServerHandler::MsgServerHandler() :
    msg(0) {
}

MsgServerHandler::~MsgServerHandler() {
  assert(msg == 0);
  msgServer->onConnectionError();
}

void MsgServerHandler::setServer(MsgServer* server) {
  this->msgServer = server;
  server->setHandler(this);
}

int MsgServerHandler::open(void *) {
  if (super::open(0) == -1)
    return -1;

  InetAddr peerAddr;
  if (this->getStream().getRemoteAddr(peerAddr) == 0) {
    std::string strPeer = peerAddr.toString();
    LogDebug << "MsgServerHandler::open remote connection from " << strPeer << endl;
  }

  //Block mode to avoid EWOULDBLOCK
  getStream().disable(ACE_NONBLOCK);

  msg = 0;
  return 0;
}

int MsgServerHandler::close(void *) {
  return 0;
}

int MsgServerHandler::handleInput(ACE_HANDLE fd) {
  if (msg == 0) {
    msg = new Msg;
  }

  int result = MsgProtocol::parse(&stream, msg);
  if (result == -1) {
    LogError << "MsgServerHandler::handleInput error: " << perr << endl;
    delete msg;
    msg = 0;
    return -1;
  } else if (result == 1) {
    return 0;
  }

  assert(result == 0);

  if (msg->messageType == Msg_KeepAlive_Probe) {
    OutputStream os;
    MsgProtocol::start(Msg_KeepAlive_ACK, &os);
    std::string t("keepAlive");
    os.write_string(t);

    InetAddr peerAddr;
    this->getStream().getRemoteAddr(peerAddr);

//    ICC_DEBUG("recved publisher keep alive probe, sending ack, peer: %s", peerAddr.toString().c_str());
    int result = MsgProtocol::send(&this->getStream(), &os);
    if (result == 0) {
//      ICC_DEBUG("send keep alive probe ack succeeded, peer: %s", peerAddr.toString().c_str());
    }

    delete msg;
    msg = 0;

  } else if(msg->messageType == Msg_Event) {

    // print log , temporary variable "type" is for info,
    string type;
    InputStream is(msg->is);
    is.read_string(type);
    //ICC_DEBUG("recved publish event, topic: %s, type: %s", msg->topic.c_str(), type.c_str());

    msgServer->handleEvent(msg->is);

    msg = 0;
  }

  return result;
}

int MsgServerHandler::handleClose(ACE_HANDLE h, ReactorMask mask) {
  msgServer->setHandler(0);
  return super::handleClose(h, mask);
}

MsgServer::MsgServer(Reactor* reactor_)
:handler(0), reactor(reactor_)
{

}

MsgServer::~MsgServer() {
  if (this->handler)
    delete this->handler;
  if (this->acceptor)
    delete this->acceptor;
}

int MsgServer::publish(OutputStream& os) {

  MsgProtocol::check(os);

  int result = MsgProtocol::send(&handler->getStream(), &os);
  if (result == -1) {
    // connection broke
    LogError << "send msg err:" << perr << endl;
  }

  return 0;
}

bool MsgServer::init(short port) {
  //this->brokerIp = brokerIp;
  this->brokerPort = port;

  acceptor = new MsgAcceptorImpl<MsgServerHandler, MsgServer>(this->reactor, this);

  InetAddr listenAddr(brokerPort);
  if (acceptor->open(listenAddr, reactor) == -1) {
    LogError << "MsgServer::init acceptor error:" << perr << endl;
    return false;
  }

  activate();

  return true;
}

int MsgServer::svc()  {

  TimeValue timeout(1);
  while(true) {
    reactor->handleEvents();
  }

  return 0;
}
