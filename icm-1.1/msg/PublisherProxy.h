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

#include <map>

class PublisherHandler;
class Msg;
typedef Acceptor<PublisherHandler> PubAcceptor;

class PublisherHandler : public SvcHandler
{
  typedef SvcHandler super;
public:
  PublisherHandler();
  ~PublisherHandler();
  int open (void * );

  int close (void *);

  virtual int handleInput (ACE_HANDLE fd );

  virtual int handleClose (ACE_HANDLE h,ReactorMask mask);

private:
  Msg* msg;
};


