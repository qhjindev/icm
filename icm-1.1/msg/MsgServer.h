#ifndef MSG_SERVER_H
#define MSG_SERVER_H

#include "os/OS.h"
#include "icc/Reactor.h"
#include "icc/Acceptor.h"
#include "icc/SelectReactor.h"
#include "icc/SocketAcceptor.h"
#include "icc/InetAddr.h"
#include "icc/SocketStream.h"
#include "icc/SvcHandler.h"
#include "icc/Synch.h"
#include "icc/Log.h"
#include "icc/Task.h"
#include "icm/IcmStream.h"
#include "msg/MsgAcceptorImpl.h"
#include <map>


class MsgServer;
class Msg;

class MsgServerHandler : public SvcHandler
{
  typedef SvcHandler super;
public:
  MsgServerHandler();
  ~MsgServerHandler();
  int open (void * );

  int close (void *);

  virtual int handleInput (ACE_HANDLE fd );

  virtual int handleClose (ACE_HANDLE h,ReactorMask mask);

  void setServer(MsgServer* server);

private:
  Msg* msg;
  MsgServer* msgServer;
};

class MsgServer : public Thread {

public:
  explicit MsgServer(Reactor* reactor = Reactor::instance());
  virtual ~MsgServer();

  bool init(short port = 50000);

  int publish(OutputStream& os);

  virtual void handleEvent(InputStream& is) {

  }

  void setHandler(MsgServerHandler* h) {
    this->handler = h;
  }

  // to be inherited by sub class
  virtual void onConnectionError() {
    LogError << ("connection from peer closed!!") << endl;
  }

  virtual int svc();

private:

  MsgServerHandler* handler;
  Reactor* reactor;
  Acceptor<MsgServerHandler>* acceptor;

  //std::string brokerIp;
  short brokerPort;

};

#endif //MSG_SERVER_H

