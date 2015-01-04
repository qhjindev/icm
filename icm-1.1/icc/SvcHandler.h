#ifndef _ICC_SVC_HANDLER_H_
#define _ICC_SVC_HANDLER_H_

#include "icc/EventHandler.h"
#include "icc/SocketStream.h"
#include "icc/Reactor.h"
#include "icc/Thread.h"

class SvcHandler : public EventHandler, public Thread
{
public:
  SvcHandler (ThreadManager* thrMgr = 0, Reactor* reactor = Reactor::instance());
  virtual ~SvcHandler();

  virtual int open(void *);

  virtual int registerWriteMask();
  virtual int unRegisterWriteMask();

  virtual int handleClose (ACE_HANDLE = ACE_INVALID_HANDLE,
                           ReactorMask = EventHandler::ALL_EVENTS_MASK);

  virtual ACE_HANDLE getHandle (void) const;

  virtual void setHandle (ACE_HANDLE);

  SocketStream &getStream ();

protected:

  void destroy (void);

  SocketStream stream;
};

#endif //_ICC_SVC_HANDLER_H_
