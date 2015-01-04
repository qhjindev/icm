#include "TopicManager.h"
#include "SubscriberProxy.h"
#include "icc/Guard.h"
#include "icc/Log.h"
#include <fstream>
#include <assert.h>

using namespace std;

const size_t MaxBufferSize = 100 * 1000;

TopicManager::TopicManager() {
  if(!SubscriberHandler::enableBuffering) {
    FILE* fp = OS::fopen("SubscriptionInfo", "r");
    if(fp != 0) {
      OS::fclose(fp);
//      ICC_ERROR("clearing content of file: %s", "SubscriptionInfo");
      fp = fopen("SubscriptionInfo", "wb+");
      OS::fclose(fp);
    }

    return;
  }

  ifstream ifs("SubscriptionInfo");
  if(!ifs.is_open()) {
    ICC_ERROR("no SubscriptionInfo file");
    nextAllocateClientId = 1;
    return;
  }

  nextAllocateClientId = 0;
  string tmpstr;
  ifs >> tmpstr;
//  ICC_ERROR("tmpstr: %s", tmpstr.c_str());
  ifs >> nextAllocateClientId;
  if(nextAllocateClientId == 0) {
    // first time run, info file not exist yet
    nextAllocateClientId = 1;
    return;
  }

  string topic;
  int subscriberCount = 0;
  ifs >> topic;
  while(ifs.is_open() && !ifs.eof()) {
    ifs >> subscriberCount;
    assert(!ifs.eof());

    for(int i = 0; i < subscriberCount; ++i) {
      string currentSubscriber;
      ifs >> currentSubscriber;
      assert(!ifs.eof());
      this->addSubscription_(topic, currentSubscriber, 0);
//    ICC_INFO("sub info, topic: %s, add suber: %s", topic.c_str(), currentSubscriber.c_str());
    }
    ifs >> topic;
  }
//  ICC_DEBUG("reload subscription info ok");
}

TopicManager*
TopicManager::instance() {
  static TopicManager mgr;

  return &mgr;
}

void TopicManager::addSubscription_(const string& topic, const string& subscriberId, SubscriberHandler* handler) {
  assert(topic != "");
  assert(subscriberId != "");

  subscribers[subscriberId] = handler;
  subscriptionMap[topic].insert(subscriberId);
  int clientHandle = 0;
  if(handler != 0) {
    assert(subscribers[subscriberId] == handler);
    clientHandle = handler->getHandle();
  }
//  ICC_DEBUG("TopicManager::addSubscription, topic: %s, clientId: %s, client Handle: %d, handler: %p", topic.c_str(), subscriberId.c_str(), clientHandle, handler);
}

void TopicManager::addSubscription(const string& topic, SubscriberHandler* handler) {
  assert(topic != "");
  assert(handler != 0);

  subscriptionMap1[topic].insert(handler);
  subscriptionMap2[handler].insert(topic);
}

void TopicManager::removeSubscription_(const string& topic, const string& subscriberId) {
  subscriptionMap[topic].erase(subscriberId);
}

void TopicManager::removeSubscription(const string& topic, SubscriberHandler* handler) {
  subscriptionMap1[topic].erase(handler);
}

void TopicManager::onHandlerDestroyed_(const std::string& subscriberId) {
  subscribers[subscriberId] = 0;
}

void TopicManager::onHandlerDestroyed(SubscriberHandler* handler) {
  set<string>& topics = subscriptionMap2[handler];
  for(set<string>::iterator it = topics.begin(); it != topics.end(); ++it) {
    removeSubscription(*it, handler);
  }
  subscriptionMap2.erase(handler);
}

set<string>& TopicManager::getSubscribersOfTopic_(const std::string& topic) {
  return subscriptionMap[topic];
}

void TopicManager::saveSubscriptionInfoToDisk_() {
  ofstream ofs("SubscriptionInfo");
  stringstream ss;

  ss << "nextAllocateClientId: " << nextAllocateClientId << ' ';
  subscriptionMap.erase("");
  for(map<string, set<string> >::iterator it = subscriptionMap.begin();
      it != subscriptionMap.end();
      ++it)
  {
    ss << it->first << ' '; // topic
    set<string>& theSubscribers = it->second;
    theSubscribers.erase("");
    ss << theSubscribers.size() << ' ';
//    ICC_INFO("topic %s theSubscribers size: %d", it->first.c_str(), theSubscribers.size());
    for(set<string>::iterator it1 = theSubscribers.begin();
        it1 != theSubscribers.end();
        ++it1)
    {
//      ICC_INFO("theSubscribers: %s", (*it1).c_str());
      ss << *it1 << ' ';
    }
  }
  ofs << ss.str();
}

int TopicManager::allocateClientId_() {
  return nextAllocateClientId++;
}

SubscriberHandler* TopicManager::getHandlerOfClient_(const std::string& clientId) {
  assert(clientId != "");
  return subscribers[clientId];
}

void TopicManager::addMessage_(Msg* msg) {
  assert(msg != 0);
  checkBufferSize_();
  std::set<std::string>& clients = getSubscribersOfTopic_(msg->topic);
  for(std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it) {
    std::string subscriberId = *it;
    assert(subscriberId != "");
    SubscriberHandler* subscriberHandler = subscribers[subscriberId];
    if(subscriberHandler != 0) {
//      ICC_INFO("TopicManager::addMessage, register wr mask for clientId: %s, topic: %s", subscriberId.c_str(), msg->topic.c_str());
      subscriberHandler->registerWriteMask();
    } else {
//      ICC_INFO("TopicManager::addMessage, no active handler for clientId: %s, topic: %s", subscriberId.c_str(), msg->topic.c_str());
    }

    PubMap1[subscriberId].insert(msg);
    PubMap2[msg].insert(subscriberId);
  }
}

void TopicManager::addMessage(Msg* msg, bool) {
  assert(msg != 0);
  checkBufferSize(SubscriberHandler::enableBuffering);

  set<SubscriberHandler*>& handlers = subscriptionMap1[msg->topic];
  for(set<SubscriberHandler*>::iterator it = handlers.begin(); it != handlers.end(); ++it) {
    if(*it != 0 && (*it)->isActive()) {
      (*it)->registerWriteMask();

      PubMap3[msg].insert(*it);
      PubMap4[*it].insert(msg);
    }
  }
}

vector<MsgBroker::State> TopicManager::queryState(const vector<string>& topics) {
  vector<MsgBroker::State> result;
  for(vector<string>::const_iterator it = topics.begin(); it != topics.end(); ++it) {
    MsgBroker::State state;
    state.topic = *it;
    if(SubscriberHandler::enableBuffering) {
      state.s = isClientOnline_(*it);
    } else {
      state.s = isClientOnline(*it, SubscriberHandler::enableBuffering);
    }
    result.push_back(state);
  }

  return result;
}

bool TopicManager::isClientOnline_(const string& topic) {
  set<std::string>& subscribers = getSubscribersOfTopic_(topic);
  if(subscribers.empty()) {
    return false;
  }

  for(set<string>::iterator it = subscribers.begin(); it != subscribers.end(); ++it) {
    SubscriberHandler* handler = getHandlerOfClient_(*it);
    if(handler != 0 && handler->isActive()) {
      return true;
    }
  }

  return false;
}

bool TopicManager::isClientOnline(const string& topic, bool) {
  set<SubscriberHandler*>& subscribers = subscriptionMap1[topic];
  if(subscribers.empty()) {
    return false;
  }

  for(set<SubscriberHandler*>::iterator it = subscribers.begin(); it != subscribers.end(); ++it) {
    SubscriberHandler* handler = *it;
    if(handler != 0 && handler->isActive()) {
      return true;
    }
  }

  return false;
}

void TopicManager::checkBufferSize_() {
  if(PubMap2.size() >= MaxBufferSize) {
    ICC_ERROR("Buffer full, maybe resource leak?!");
    for(map<Msg*, set<string> >::iterator it = PubMap2.begin(); it != PubMap2.end(); ++it) {
      delete it->first;
    }
    PubMap1.clear();
    PubMap2.clear();
  }
}

void TopicManager::checkBufferSize(bool) {
  if(PubMap3.size() >= MaxBufferSize) {
    ICC_ERROR("Buffer full, maybe resource leak?!");
    for(map<Msg*, set<SubscriberHandler*> >::iterator it = PubMap3.begin(); it != PubMap3.end(); ++it) {
      delete it->first;
    }
    PubMap3.clear();
    PubMap4.clear();
  }
}
