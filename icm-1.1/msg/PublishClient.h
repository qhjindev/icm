#ifndef PUBLISHCLIENT_H_
#define PUBLISHCLIENT_H_

#include "os/OS.h"
#include "icc/Reactor.h"
#include "icc/Connector.h"
#include "icc/SelectReactor.h"
#include "icc/InetAddr.h"
#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
#include "icc/Synch.h"
#include "icc/Log.h"

#include "icc/SynchTraits.h"
#include "icc/SvcHandler.h"
#include "icc/Thread.h"
#include "Msg.h"
#include "MsgProtocol.h"
#include <icc/ThreadMutex.h>
#include <icc/ConditionThreadMutex.h>
#include <fstream>
#include <list>

class PublishClient;

class PublishClientHandler: public SvcHandler {
public:

//  PublishClientHandler();
  virtual ~PublishClientHandler();

  int open(void *);

  int close(void *);

  virtual int handleInput(ACE_HANDLE fd);

  virtual int handleClose(ACE_HANDLE h, ReactorMask mask);

  void setClient(PublishClient* client) {
    this->publishClient = client;
  }

private:
  PublishClient* publishClient;
};

class PublishClient : public Thread {
  friend class PublishClientHandler;
public:
  class KeepAlive_Timeout_Handler : public EventHandler
  {
  public:
    virtual int handleTimeout (const TimeValue &currentTime, const void *act) {
      PublishClient* client = (PublishClient*)act;
//      ICC_DEBUG("publisher keep alive time out, unackedKeepAliveProbes = %d\n", client->unackedKeepAliveProbes);

      if(client->getConnectionError() == true) {
        return 0;
      }

      if(client->sendKeepAliveProbe() == false) {
        client->setConnectionError(true);
        return -1;
      }

      return 0;
    }
  };

  PublishClient()
  : peerLastActiveTime(0), reactor(new Reactor), handler(0),
    msgBufferFilename("publisBuffer"), bufferExpireTime(300)
  {
  }

  virtual ~PublishClient() {
    if (this->handler)
      delete this->handler;
    if (this->connector)
      delete this->connector;
  }

  virtual int svc(void);
  bool init(const std::string& brokerIp, short port = 10001);
  bool registerToBroker();

  int publish(OutputStream& os);
  int publishToBuffer_i(OutputStream& os);
  bool publishFromBuffer();

  static bool getConnectionError();
  static void setConnectionError(bool arg);

  bool enableKeepAlive();
  bool disableKeepAlive();

  bool sendKeepAliveProbe();
private:
  void clearBufferFile_i();
  bool reestablishConnectionAndPublishFromBuffer();
  // template method
  void onConnOk() {
    enableKeepAlive();
    onConnectionEstablished();
  };
  void onConnError() {
    disableKeepAlive();
    onConnectionError();
  };

  // to be inherited by sub class
  virtual void onConnectionError() {
    ICC_ERROR("connection to icmmsg error!!");
  }
  virtual void onConnectionEstablished() {
    ICC_ERROR("connection to icmmsg established!!");
  }

  static ThreadMutex mutex;
  static bool connectionErrorPending;

  KeepAlive_Timeout_Handler keepAliveTimeoutHandler;
  bool keepAliveEnabled;
  int unackedKeepAliveProbes;
  time_t peerLastActiveTime;
  static int keepAliveTryTimes;
  static int keepAliveTimeSpan;
  static bool enableBuffering;

  Reactor* reactor;
  PublishClientHandler* handler;
  Connector<PublishClientHandler>* connector;
  std::string brokerIp;
  short brokerPort;

  std::list<std::pair<time_t, std::string> >buffer;
  std::string msgBufferFilename;
  int bufferExpireTime; // seconds
};

#endif /* PUBLISHCLIENT_H_ */

