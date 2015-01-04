#include "SubscriberProxy.h"
#include "PublisherProxy.h" // TODO, move together
#include <iostream>
#include <memory> // for auto_ptr

#include "TopicManager.h"
#include "Msg.h"
#include "MsgProtocol.h"

#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"
#include "icc/Configuration.h"

using namespace std;

int SubscriberHandler::keepAliveTryTimes(3);
int SubscriberHandler::keepAliveTimeSpan(10);
bool SubscriberHandler::enableBuffering(false);

SubscriberHandler::SubscriberHandler()
: id("0"), tmpMsg(0), topic(""), recentActiveTime(time(0))
{
//  ICC_ERROR("makeing new SubscriberHandler, this = %p", this);
}

SubscriberHandler::~SubscriberHandler() {
//  ICC_ERROR("detor of SubscriberHandler, this = %p", this);
  if(enableBuffering) {
    if(id != "") {
      TopicManager::instance()->onHandlerDestroyed_(id);
    }
  } else {
    TopicManager::instance()->onHandlerDestroyed(this);
  }
  assert(tmpMsg == 0);
}

int SubscriberHandler::open(void *) {
  if (super::open(0) == -1)
    return -1;

  InetAddr peerAddr;
  if (this->getStream().getRemoteAddr(peerAddr) == 0) {
    ICC_DEBUG("SubscriberHandler::open, connection from: %s", peerAddr.toString().c_str());
  }

  return 0;
}

int SubscriberHandler::close(void *) {
  return 0;
}

int SubscriberHandler::handleClose(ACE_HANDLE h, ReactorMask mask) {
  return super::handleClose(h, mask);
}

int SubscriberHandler::handleInput(ACE_HANDLE fd) {
  this->recentActiveTime = time(0);

  if(tmpMsg == 0) {
    tmpMsg = new Msg;
  }
  int result = MsgProtocol::parse(&stream, tmpMsg);
  if (result == -1) {
//    ICC_ERROR("SubscriberHandler::handleInput error");
    delete tmpMsg;
    tmpMsg = 0;
    return -1;
  } else if(result == 1) {
    return 0;
  }

  assert(result == 0); // parse complete done
  auto_ptr<Msg> msg(tmpMsg);
  tmpMsg = 0;

  if (msg->messageType == Msg_Subscribe) {
    this->topic = msg->topic;
    if(enableBuffering) {
      this->id = msg->subscriberId;
      assert(id != "");

      if(id == "0") {
        int allocatedClientId = TopicManager::instance()->allocateClientId_();
        stringstream ss;
        ss << allocatedClientId;
        id = ss.str();
        assert(id.size() == 1);
      }

      TopicManager::instance()->addSubscription_(msg->topic, id, this);
      TopicManager::instance()->saveSubscriptionInfoToDisk_();
    } else  {
      TopicManager::instance()->addSubscription(msg->topic, this);
    }

    InetAddr peerAddr;
    if (this->getStream().getRemoteAddr(peerAddr) == 0) {
      ICC_DEBUG("recved subscribe topic: %s, from: %s, address: %s", msg->topic.c_str(), id.c_str(), peerAddr.toString().c_str());
    }

    OutputStream os;
    MsgProtocol::start(Msg_Subscribe_OK, &os);
    os.write_string(msg->topic);
    os.write_string(id);
    os.write_int(keepAliveTryTimes);
    os.write_int(keepAliveTimeSpan);

    int result = MsgProtocol::send(&this->getStream(), &os);

    if (result == -1) {
      return -1;
    }

    InetAddr peerAddr1;
    if (this->getStream().getRemoteAddr(peerAddr1) == 0) {
      ICC_DEBUG("acked subscribe topic: %s, to: %s, address: %s", msg->topic.c_str(), id.c_str(), peerAddr1.toString().c_str());
    }

    this->registerWriteMask();
    return 0;
  } else if (msg->messageType == Msg_KeepAlive_Probe) {
    OutputStream os;
    MsgProtocol::start(Msg_KeepAlive_ACK, &os);
    std::string t("keepAlive");
    os.write_string(t);

    InetAddr peerAddr;
    this->getStream().getRemoteAddr(peerAddr);

//    ICC_INFO("recved subscriber keep alive probe, sending ack, peer: %s", peerAddr.toString().c_str());
    int result = MsgProtocol::send(&this->getStream(), &os);
    if (result == -1) {
      return -1;
    }

//    ICC_INFO("send keep alive probe ack succeeded, peer: %s", peerAddr.toString().c_str());

  } else if (msg->messageType == Msg_UnSubscribe) {
    ICC_ERROR("process unsubscribe request, this->id: %s, msgState.subscriberId: %s", id.c_str(), msg->subscriberId.c_str());
    assert(this->id == msg->subscriberId);

    if(enableBuffering) {
      TopicManager::instance()->removeSubscription_(msg->topic, id);
      TopicManager::instance()->saveSubscriptionInfoToDisk_();
    } else {
      TopicManager::instance()->removeSubscription(msg->topic, this);
    }

    ICC_DEBUG("recved unsubscribe topic: %s", msg->topic.c_str());

    OutputStream os;
    MsgProtocol::start(Msg_UnSubscribe_OK, &os);
    std::string t(msg->topic);
    os.write_string(t);

    int result = MsgProtocol::send(&this->getStream(), &os);
    if (result == -1) {
      return -1;
    }
  } else {
    assert(false);
  }

  return 0;
}

int SubscriberHandler::handleOutput (ACE_HANDLE fd) {
  set<Msg*> messages;
  if(enableBuffering) {
    messages = TopicManager::instance()->getClientMessages_(this->id);
  } else {
    messages = TopicManager::instance()->getClientMessages(this);
  }
  for(set<Msg*>::iterator it = messages.begin(); it != messages.end(); ++it) {
    assert(*it != 0);
    ICC_DEBUG("SubscriberHandler::handleOutput sending message, clientId: %s, topic: %s", id.c_str(), (*it)->topic.c_str());
    if(sendMessage(*it) == -1) {
//      ICC_DEBUG("SubscriberHandler::handleOutput sendMessage returns -1");
      return -1;
    } else {
      this->recentActiveTime = time(0);
      if(enableBuffering) {
        TopicManager::instance()->onMessageSent_(*it, this->id);
      } else {
        TopicManager::instance()->onMessageSent(*it, this);
      }
    }
  }

  this->unRegisterWriteMask();
  return 0;
}

int SubscriberHandler::sendMessage(Msg* msg) {
  MessageBlock* mb = (MessageBlock*) msg->is.start();
  mb->rdPtr(mb->base());

  int result = 0;
  time_t now = time(0);
  int life = now - msg->creatTime;
  if(life <= MaxBufferMessageLifeTime) {
    result = sendMessage(mb);
  }

  return result;
}

int SubscriberHandler::sendMessage(MessageBlock* mb) {
  assert(id != "");
  if(enableBuffering) {
    assert(TopicManager::instance()->getHandlerOfClient_(id) == this);
  }

  size_t bytesSent = 0;
  MessageBlock* curmb = mb;
//  ICC_DEBUG("SubscriberHandler::sendMessage, real sending");
  while ((bytesSent = stream.send(curmb->rdPtr(), curmb->length())) == curmb->length()) {
    curmb = curmb->cont();
    if (curmb == 0)
      break;

    curmb->rdPtr(curmb->base());
  }
  if (curmb != NULL) {
    string peerAddrString = "release mode, commented";
          InetAddr peerAddr;
          if (stream.getRemoteAddr(peerAddr) == 0) {
            peerAddrString = peerAddr.toString();
          }
    ICC_ERROR(
        "SubscriberProxy::svc, send topic failed!!!, peer : %s, bytes to send: %d, send result: %d, reason: %s, this: %p",
        peerAddrString.c_str(), curmb->length(), bytesSent, OS::strerror(OS::map_errno(OS::last_error())), this);

    return -1;
  } else {
//    ICC_INFO("send topic succeed");
  }

  return 0;
}


