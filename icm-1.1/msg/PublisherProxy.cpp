#include "icm/IcmStream.h"
#include "PublisherProxy.h"
#include "SubscriberProxy.h"
#include "TopicManager.h"
#include "MsgProtocol.h"
#include "Msg.h"
#include <iostream>

using namespace std;

PublisherHandler::PublisherHandler() :
    msg(0) {
}

PublisherHandler::~PublisherHandler() {
  assert(msg == 0);
}

int PublisherHandler::open(void *) {
  if (super::open(0) == -1)
    return -1;

  InetAddr peerAddr;
  if (this->getStream().getRemoteAddr(peerAddr) == 0) {
    std::string strPeer = peerAddr.toString();
    ICC_DEBUG("connection from %s", strPeer.c_str());
  }

  msg = 0;
  return 0;
}

int PublisherHandler::close(void *) {
  return 0;
}

int PublisherHandler::handleInput(ACE_HANDLE fd) {
  if (msg == 0) {
    msg = new Msg;
  }

  int result = MsgProtocol::parse(&stream, msg);
  if (result == -1) {
    ICC_ERROR("PublisherHandler::handleInput error: %s", OS::strerror(OS::map_errno(OS::last_error())));
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
    return result;
  } else if(msg->messageType == Msg_Publish) {
    OutputStream os;
    MsgProtocol::start(Msg_Publish_OK, &os);
    os.write_string("publishOK");
    os.write_int(SubscriberHandler::keepAliveTryTimes);
    os.write_int(SubscriberHandler::keepAliveTimeSpan);
    os.write_boolean(SubscriberHandler::enableBuffering);

    InetAddr peerAddr;
    this->getStream().getRemoteAddr(peerAddr);

    ICC_DEBUG("sending Msg_Publish_OK, peer: %s", peerAddr.toString().c_str());
    result = MsgProtocol::send(&this->getStream(), &os);
    delete msg;
    msg = 0;
    return result;
  }

  assert(msg->messageType == Msg_Event);
  assert(msg->topic != "");

  // print log , temporary variable "type" is for info,
  string type;
  InputStream is(msg->is);
  is.read_string(type);
  ICC_DEBUG("recved publish event, topic: %s, type: %s", msg->topic.c_str(), type.c_str());

  if(SubscriberHandler::enableBuffering) {
    TopicManager::instance()->addMessage_(msg);
  } else {
    TopicManager::instance()->addMessage(msg, SubscriberHandler::enableBuffering);
  }
  msg = 0;

  return result;
}

int PublisherHandler::handleClose(ACE_HANDLE h, ReactorMask mask) {
  return super::handleClose(h, mask);
}
