#ifndef SUBSCRIBECLENT_H_
#define SUBSCRIBECLENT_H_

#include "os/OS.h"
#include "icc/Reactor.h"
#include "icc/Connector.h"
#include "icc/SelectReactor.h"
#include "icc/InetAddr.h"
#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
#include "icc/Synch.h"
#include "icc/Log.h"
#include "icc/ConditionThreadMutex.h"

#include "Msg.h"
#include "MsgProtocol.h"

#include <string>
#include <iostream>
#include <set>

#include "icc/SynchTraits.h"
#include "icc/SvcHandler.h"
#include "icc/Task.h"
#include "PublishClient.h"

class SubscribeClient;

class SubscribeClientHandler: public SvcHandler {
public:
  typedef SvcHandler super;

  SubscribeClientHandler();
  virtual ~SubscribeClientHandler();

  int open(void *);

  int close(void *);

  virtual int handleInput(ACE_HANDLE fd);

  virtual int handleClose(ACE_HANDLE h, ReactorMask mask);

  virtual void onSubScribeTopicOk(const std::string& topic, const std::string& clientId);
  virtual void onUnSubScribeTopicOk(const std::string& topic);

  void setClient(SubscribeClient* client) {
    this->subscribeClient = client;
  }

private:
  SubscribeClient* subscribeClient;
  Msg* tmpMsg;
};

class SubscribeClient {
  friend class SubscribeClientHandler;
public:
  class KeepAlive_Timeout_Handler: public EventHandler {
  public:
    virtual int handleTimeout(const TimeValue &currentTime, const void *act) {
      SubscribeClient* client = (SubscribeClient*) act;
//	 ICC_DEBUG("subscriber keep alive time out, unackedKeepAliveProbes = %d\n", client->unackedKeepAliveProbes);

      if (client->sendKeepAliveProbe() == false) {
        ICC_ERROR("keep alive error!\n");
        client->connectionErrorPending = true;
        return -1;
      }

      return 0;
    }
  };

  explicit SubscribeClient(Reactor* reactor = Reactor::instance());
  virtual ~SubscribeClient();

  bool init(const std::string& brokerIp, short port = 10002);

  bool subscribeTopic(const std::string& topic);

  bool unSubscribeTopic(const std::string& topic);

  virtual void handleEvent(InputStream& is) {
    //Event event;
    //event.__read(state.is);
    //onEvent(&event);
  }

  void run();

  bool enableKeepAlive(int timeSpec = keepAliveTimeSpan) {
//	  ICC_DEBUG("enableKeepAlive begin\n");
    if (reactor == NULL
      )
      return false;
    if (keepAliveEnabled == true)
      return true;

    TimeValue tv1(timeSpec);
    TimeValue tv2(timeSpec);
    if (reactor->scheduleTimer(&keepAliveTimeoutHandler, this, tv1, tv2) == -1) {
      ICC_ERROR("enableKeepAlive() failed!!\n");
      return false;
    }

    keepAliveEnabled = true;
    return true;
  }

  bool disableKeepAlive() {
    assert(reactor != NULL);

    reactor->cancelTimer(&keepAliveTimeoutHandler);
    keepAliveEnabled = false;
    return true;
  }

  Reactor* getReactor() {
    return reactor;
  }

  static int keepAliveTryTimes;
  static int keepAliveTimeSpan; // seconds
  bool sendKeepAliveProbe();

  static std::string getProgName();
  static std::string infoFileName;
private:
  bool reestablishConnection();
  bool realSubTopic(const std::string& topic);
  bool realUnSubTopic(const std::string& topic);
  bool processAllSubAndUnSub();
  // template method
  void onConnOk();
  void onConnError();

  // to be inherited by sub class
  virtual void onConnectionError() {
    ICC_ERROR("connection to icmmsg error!!");
  }
  virtual void onConnectionEstablished() {
    ICC_ERROR("connection to icmmsg established!!");
  }

  SubscribeClientHandler* handler;
  Reactor* reactor;
  Connector<SubscribeClientHandler>* connector;

  ThreadMutex mutex; // mutex of sub info(the following 3 sets)
  std::set<std::string> subcribedTopics;
  std::set<std::string> toSubcribeTopics;
  std::set<std::string> toUnSubcribeTopics;

  std::string brokerIp;
  short brokerPort;
  std::string id;

  KeepAlive_Timeout_Handler keepAliveTimeoutHandler;
  bool connectionErrorPending;
  bool keepAliveEnabled;
  int unackedKeepAliveProbes;
  time_t peerLastActiveTime;

  static ThreadMutex idInfofileMutex;
};

#endif /* SUBSCRIBECLENT_H_ */

