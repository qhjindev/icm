
#include "icc/Acceptor.h"
#include "icc/SvcHandler.h"
#include "icc/Reactor.h"

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::open(const InetAddr &localAddr, Reactor *reactor)
{
  if (reactor == 0)
  {
    errno = EINVAL;
    return -1;
  }

  if (this->m_acceptor.open (localAddr, 1) == -1)
    return -1;

  (void)this->m_acceptor.enable (ACE_NONBLOCK);

  int const result = reactor->registerHandler (this, EventHandler::ACCEPT_MASK);
  if (result != -1)
    this->setReactor (reactor);
  else
    this->m_acceptor.close ();

  return result;
}

template <class SVC_HANDLER>
Acceptor<SVC_HANDLER>::~Acceptor ()
{
}

template <class SVC_HANDLER> ACE_HANDLE
Acceptor<SVC_HANDLER>::getHandle (void) const
{
  return this->m_acceptor.getHandle ();
}

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::close (void)
{
  return this->handleClose ();
}

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::handleClose (ACE_HANDLE, ReactorMask)
{
  if (this->getReactor () != 0) {
//    ACE_HANDLE handle = this->getHandle ();

    this->getReactor ()->removeHandler (this, EventHandler::ACCEPT_MASK | EventHandler::DONT_CALL);

    if (this->m_acceptor.close () == -1) {
    }

    this->setReactor (0);
  }

  return 0;
}

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::makeSvcHandler (SVC_HANDLER *&sh)
{
  if (sh == 0)
    sh = new SVC_HANDLER;

  sh->setReactor (this->getReactor ());

  return 0;
}

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::acceptSvcHandler (SVC_HANDLER *svcHandler)
{
  if (this->m_acceptor.accept (svcHandler->getStream ()) == -1) {
    svcHandler->close (0);
    return -1;
  } else
    return 0;
}

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::activateSvcHandler (SVC_HANDLER *svcHandler)
{
  int result = 0;

  if (svcHandler->getStream ().enable (ACE_NONBLOCK) == -1)
    result = -1;

  if (result == 0 && svcHandler->open ((void *)this) == -1)
    result = -1;

  if (result == -1)
    svcHandler->close (0);

  return result;
}

template <class SVC_HANDLER>
int Acceptor<SVC_HANDLER>::handleInput (ACE_HANDLE listener)
{
  SVC_HANDLER *svcHandler = 0;

  if (this->makeSvcHandler (svcHandler) == -1) {
    return 0;
  } else if (this->acceptSvcHandler (svcHandler) == -1) {
    return 0;
  } else if (this->activateSvcHandler (svcHandler) == -1) {
    return 0;
  }

  return 0;
}

