#ifndef MSG_CLIENT_H
#define MSG_CLIENT_H

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

#include "icc/SynchTraits.h"
#include "icc/SvcHandler.h"
#include "icc/Task.h"

class MsgClient;

class MsgClientHandler: public SvcHandler {
public:
  typedef SvcHandler super;

  MsgClientHandler();
  virtual ~MsgClientHandler();

  int open(void *);

  int close(void *);

  virtual int handleInput(ACE_HANDLE fd);

  virtual int handleClose(ACE_HANDLE h, ReactorMask mask);

  void setClient(MsgClient* client) {
    this->msgClient = client;
  }

private:
  MsgClient* msgClient;
  Msg* tmpMsg;
};

class MsgClient : public Thread {

  friend class MsgClientHandler;

public:

  explicit MsgClient(Reactor* reactor = Reactor::instance());
  virtual ~MsgClient();

  bool init(const std::string& serverIp, short port = 50000, int reconnectInterval = 5);

  int publish(OutputStream& os);

  virtual void handleEvent(InputStream& is) {
  }

  int svc();

  class KeepAlive_Timeout_Handler: public EventHandler {
  public:
    virtual int handleTimeout(const TimeValue &currentTime, const void *act) {
      MsgClient* client = (MsgClient*) act;

      if (client->sendKeepAliveProbe() == false) {
        ICC_ERROR("keep alive error!\n");
        client->connectionErrorPending = true;
        return -1;
      }

      return 0;
    }
  };

  bool enableKeepAlive(int timeSpec = keepAliveTimeSpan) {
//    ICC_DEBUG("enableKeepAlive begin\n");
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
  int reconnectInterval; //seconds

  bool sendKeepAliveProbe();

protected:
    // to be inherited by sub class
  virtual void onConnectionError() {
    ICC_ERROR("connection to icmmsg error!!");
  }

  virtual void onConnectionEstablished() {
    ICC_ERROR("connection to icmmsg established!!");
  }

private:

  bool reestablishConnection();
  void onConnOk();
  void onConnError();

  MsgClientHandler* handler;
  Reactor* reactor;
  Connector<MsgClientHandler>* connector;

  std::string serverIp;
  short serverPort;
  std::string id;

  KeepAlive_Timeout_Handler keepAliveTimeoutHandler;
  bool connectionErrorPending;
  bool keepAliveEnabled;
  int unackedKeepAliveProbes;
  time_t peerLastActiveTime;

};

#endif //MSG_CLIENT_H
