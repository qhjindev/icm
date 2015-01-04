
#include "icc/Connector.h"

template <class SVC_HANDLER>
Connector<SVC_HANDLER>::Connector(Reactor* reactor)
{
  this->m_reactor = reactor;
}

template <class SVC_HANDLER>
int Connector<SVC_HANDLER>::connect(SVC_HANDLER *&svcHandler, const InetAddr &remoteAddr)
{
  if (this->makeSvcHandler (svcHandler) == -1)
    return -1;

  int result = 0;
  result = this->connectSvcHandler (svcHandler,remoteAddr);

  if (result != -1) {
    return this->activateSvcHandler (svcHandler);
  } else {
    delete svcHandler;
    svcHandler = 0;
    return result;
  }
}

template <class SVC_HANDLER>
int Connector<SVC_HANDLER>::makeSvcHandler (SVC_HANDLER *&sh)
{
  if (sh == 0)
    sh = new SVC_HANDLER;

  sh->setReactor (this->getReactor ());

  return 0;
}

template <class SVC_HANDLER>
int Connector<SVC_HANDLER>::activateSvcHandler(SVC_HANDLER *svcHandler)
{
  int error = 0;

  if (svcHandler->getStream ().enable (ACE_NONBLOCK) == -1)
    error = -1;

  if (error || svcHandler->open ((void *)this) == -1) {
    svcHandler->close (0);
    return -1;
  } else
    return 0;
}

template <class SVC_HANDLER>
int Connector<SVC_HANDLER>::connectSvcHandler(SVC_HANDLER *&svcHandler, const InetAddr &remoteAddr)
{
  return this->connector.connect (svcHandler->getStream (), remoteAddr);
}
