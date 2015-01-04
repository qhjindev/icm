#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "icc/EventHandler.h"
#include "icc/SocketAcceptor.h"

template <class SVC_HANDLER>
class Acceptor : public EventHandler
{
public:
  virtual int open (const InetAddr &localAddr, Reactor *reactor = Reactor::instance ());
  
  virtual ~Acceptor (void);

  virtual ACE_HANDLE getHandle (void) const;

  virtual int close (void);

  int getLocalAddr(InetAddr &sa) {
    return m_acceptor.getLocalAddr(sa);
  }

protected:
  virtual int makeSvcHandler (SVC_HANDLER *&sh);

  virtual int acceptSvcHandler (SVC_HANDLER *svcHandler);

  virtual int activateSvcHandler (SVC_HANDLER *svcHandler);

  virtual int handleClose (ACE_HANDLE = ACE_INVALID_HANDLE, ReactorMask = EventHandler::ALL_EVENTS_MASK);

  virtual int handleInput (ACE_HANDLE);

protected:
  SocketAcceptor m_acceptor;
  SVC_HANDLER *m_svcHandler;
};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "Acceptor.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#endif //_ACCEPTOR_H_

