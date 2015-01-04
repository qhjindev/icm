#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "icc/EventHandler.h"
#include "icc/SocketConnector.h"
#include "icc/Reactor.h"

template <class SVC_HANDLER>
class Connector : public EventHandler
{
public:
  Connector(Reactor* reactor = Reactor::instance());

  virtual int connect (SVC_HANDLER* &svcHandler,
                       const InetAddr &remoteAddr);

  Reactor* getReactor()
  {
    return this->m_reactor;
  }

  void setReactor(Reactor* reactor)
  {
    this->m_reactor = reactor;
  }

protected:
  virtual int makeSvcHandler (SVC_HANDLER *&sh);

  virtual int connectSvcHandler (SVC_HANDLER *&svcHandler,
                                 const InetAddr &remoteAddr);

  virtual int activateSvcHandler (SVC_HANDLER *svcHandler);

private:
  SocketConnector connector;
  Reactor* m_reactor;
};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "Connector.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#endif //_CONNECTOR_H_
