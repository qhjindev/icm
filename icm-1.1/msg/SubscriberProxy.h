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
#include "icc/MessageQueue.h"

#include "MsgBroker.h"

#include <vector>

class SubscriberHandler;
class Msg;
typedef Acceptor<SubscriberHandler> SubAcceptor;

class OutputStream;

const int MaxBufferMessageLifeTime = 2 * 60; // 10 minutes

class SubscriberHandler : public SvcHandler
{
  typedef SvcHandler super;
public:
  SubscriberHandler();

  ~SubscriberHandler();

  int open (void * );

  int close (void *);

  virtual int handleInput (ACE_HANDLE fd );

  virtual int handleOutput (ACE_HANDLE fd);

  virtual int handleClose (ACE_HANDLE h,ReactorMask mask);

  int sendMessage(MessageBlock* mb);
  int sendMessage(Msg* msg);

  static int keepAliveTryTimes;
  static int keepAliveTimeSpan;
  static bool enableBuffering;
  bool isActive() {
    return time(0) - recentActiveTime <= keepAliveTryTimes * keepAliveTimeSpan;
  }

private:
  std::string id;
  Msg* tmpMsg;

  std::string topic; // currently, one client subscribes just one topic
  time_t recentActiveTime;
};


