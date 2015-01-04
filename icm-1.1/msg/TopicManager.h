#ifndef MSG_BROKER_H
#define MSG_BROKER_H

#include <set>
#include <list>
#include <string>
#include <map>
#include <icc/SocketStream.h>
#include "MsgBroker.h"
#include "Msg.h"
#include "icc/Log.h"

class PublisherHandler;
class SubscriberHandler;

class TopicManager {
public:
  TopicManager();

  static TopicManager* instance();

  std::vector<MsgBroker::State> queryState(const std::vector<std::string>& topics);

public:
  void addSubscription_(const std::string& topic, const std::string& subscriberId, SubscriberHandler* handler);

  void removeSubscription_(const std::string& topic, const std::string& subscriberId);

  void onHandlerDestroyed_(const std::string& subscriberId);

  std::set<std::string>& getSubscribersOfTopic_(const std::string& topic); // subscriber id

  void saveSubscriptionInfoToDisk_();

  int allocateClientId_();

  SubscriberHandler* getHandlerOfClient_(const std::string& clientId);

  void addMessage_(Msg* msg);

  std::set<Msg*> getClientMessages_(const std::string& clientId) {
    assert(clientId != "");
    return PubMap1[clientId];
  }

  void onMessageSent_(Msg* msg, const std::string& clientId) {
    PubMap1[clientId].erase(msg);
    PubMap2[msg].erase(clientId);
    if(PubMap2[msg].empty()) {
      PubMap2.erase(msg);
      delete msg;
    }
  }

  bool isClientOnline_(const std::string& topic); // currently, one client corresponds to one topic

private:
  void checkBufferSize_(); // if full, remove part content
  std::map<std::string, SubscriberHandler*> subscribers; // subscriber id, handler

  std::map<std::string, std::set<std::string> > subscriptionMap; // topic, subscriber Ids
  int nextAllocateClientId;

  std::map<std::string, std::set<Msg*> > PubMap1; // subscriber id, messages to send
  std::map<Msg*, std::set<std::string> > PubMap2; // message to send, subscriber ids

  //////////////////////////////////////
public:
  void addSubscription(const std::string& topic, SubscriberHandler* handler);
  void removeSubscription(const std::string& topic, SubscriberHandler* handler);
  void onHandlerDestroyed(SubscriberHandler* handler);
  void addMessage(Msg* msg, bool enableBuffering);
  std::set<Msg*> getClientMessages(SubscriberHandler* handler) {
    assert(handler != 0);
    return PubMap4[handler];
  }
  void onMessageSent(Msg* msg, SubscriberHandler* handler) {
    PubMap3[msg].erase(handler);
    PubMap4[handler].erase(msg);
    if(PubMap3[msg].empty()) {
      PubMap3.erase(msg);
      delete msg;
    }
  }
  bool isClientOnline(const std::string& topic, bool enableBuffering);
private:
  void checkBufferSize(bool enableBuffering);

  std::map<std::string, std::set<SubscriberHandler*> > subscriptionMap1; // topic, handler
  std::map<SubscriberHandler*, std::set<std::string> > subscriptionMap2; // handler, topic
  std::map<Msg*, std::set<SubscriberHandler*> > PubMap3; // msg, handlers
  std::map<SubscriberHandler*, std::set<Msg*> > PubMap4; // handler, msgs
};

#endif //MSG_BROKER_H
