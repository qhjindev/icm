
#include "os/OS.h"
#include "icc/Reactor.h"
#include "icc/ReactorImpl.h"
#include "icc/SelectReactor.h"
#include "icc/InetAddr.h"

#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/ResourceFactory.h"
#include "icm/TcpFactory.h"
#include "icm/Pluggable.h"
#include "icm/Reference.h"

ConnectorRegistry::ConnectorRegistry (void)
{
}

ConnectorRegistry::~ConnectorRegistry (void)
{
  this->closeAll ();
}

IcmConnector*
ConnectorRegistry::getConnector (const char* tag)
{
  IcmConnectorMapIter iter = this->mConnectors.find (tag);
  if (iter != this->mConnectors.end ())
    return iter->second;
  else 
    return 0;
}

int
ConnectorRegistry::open (Communicator* comm)
{
  IcmConnector* connector = 0;

  IcmProtocolFactoryMap* ipfm = comm->protocolFactories ();

  for (IcmProtocolFactoryMapIter iter = ipfm->begin ();
       iter != ipfm->end ();
       iter++)
  {
    IcmProtocolFactory* factory = iter->second;
    connector = factory->makeConnector ();

    if (connector != 0) {
      if (connector->open (comm) != 0) {
        delete connector;

        return -1;
      }

      this->mConnectors.insert (std::make_pair (connector->tag (), connector));
    } else {
      return -1;
    }
  }

  return 0;
}

int
ConnectorRegistry::closeAll (void)
{
  IcmConnectorMapIter iter = this->mConnectors.begin ();
  for (; iter != this->mConnectors.end (); iter++) {
    IcmConnector* connector = iter->second;

    if (connector == 0)
      continue;

    connector->close ();

    delete connector;
  }

  return 0;
}

int
ConnectorRegistry::connect (Reference* ref,
                            IcmTransport*& transport,
                            TimeValue* maxWaitTime)
{
  IcmConnector* connector = this->getConnector(ref->endpoint()->protocol.c_str());

  if (connector == 0)
    return -1;

  Endpoint* endpoint = ref->endpoint();
  InetAddr addr(endpoint->port, endpoint->host.c_str ());
  return connector->connect (addr, transport, maxWaitTime);
}

//// --- AcceptorRegistry


AcceptorRegistry::AcceptorRegistry (void)
{
}

AcceptorRegistry::~AcceptorRegistry (void)
{
  this->closeAll ();
}

int
AcceptorRegistry::open (Communicator* comm, ObjectAdapter* oa, Endpoint* endpoint)
{
  IcmProtocolFactoryMap* ipfm = comm->protocolFactories ();

  for (IcmProtocolFactoryMapIter iter = ipfm->begin ();
       iter != ipfm->end ();
       iter++)
  {
    IcmProtocolFactory* factory = iter->second;

    if (!factory->matchTag (endpoint->protocol))
      continue;

    IcmAcceptor* acceptor = factory->makeAcceptor ();

    if (acceptor != 0) {
      if (acceptor->open (comm, oa, endpoint->port) != 0) {
        delete acceptor;

        return -1;
      }

      if(endpoint != 0 && endpoint->port <= 0)
        endpoint->port = acceptor->getListenPort();

      this->mAcceptors.insert (std::make_pair (acceptor->tag (), acceptor));

    } else {
      return -1;
    }
  }

  return 0;
}

void AcceptorRegistry::closeEndpoint(Endpoint* endpoint) {
  IcmAcceptorMapIter iter = this->mAcceptors.begin ();
  for (; iter != this->mAcceptors.end (); iter++) {
    IcmAcceptor* acceptor = iter->second;

    if (acceptor == 0)
      continue;

    if(acceptor->getListenPort() == endpoint->port) {
      acceptor->close ();
      delete acceptor;
      mAcceptors.erase(iter);
      return;
    }
  }
}

int
AcceptorRegistry::closeAll (void)
{
  IcmAcceptorMapIter iter = this->mAcceptors.begin ();
  for (; iter != this->mAcceptors.end (); iter++) {
    IcmAcceptor* acceptor = iter->second;

    if (acceptor == 0)
      continue;

    acceptor->close ();

    delete acceptor;
  }

  return 0;
}

ResourceFactory::ResourceFactory (void)
: mReactorType (ICM_REACTOR_SELECT_MT),
  mAcceptorRegistry(0),
  mConnectorRegistry(0)
{
}

ResourceFactory::~ResourceFactory (void)
{
}

int
ResourceFactory::initProtocolFactories (void)
{
  TcpProtocolFactory* tcpFactory = new TcpProtocolFactory;
  this->mProtocolFactories.insert (std::make_pair("TCP",tcpFactory));
  return 0;
}

IcmProtocolFactoryMap*
ResourceFactory::getProtocolFactories (void)
{
  return &this->mProtocolFactories;
}

AcceptorRegistry*
ResourceFactory::getAcceptorRegistry (void)
{
  if (this->mAcceptorRegistry == 0)
    this->mAcceptorRegistry = new AcceptorRegistry;

  return this->mAcceptorRegistry;
}

ConnectorRegistry*
ResourceFactory::getConnectorRegistry (void)
{
  if (this->mConnectorRegistry == 0)
    this->mConnectorRegistry = new ConnectorRegistry;

  return this->mConnectorRegistry;
}

ReactorImpl*
ResourceFactory::allocateReactorImpl (void) const
{
  ReactorImpl* impl = 0;
  switch (this->mReactorType) {
    default:
    case ICM_REACTOR_SELECT_MT:
      impl = new SelectReactor();
      break;

    case ICM_REACTOR_SELECT_ST:
      break;

    case ICM_REACTOR_TP:
      //impl = new TpReactor;
      break;
  }

  return impl;
}

Reactor*
ResourceFactory::createReactor (void)
{
  Reactor* reactor = 0;
  reactor = new Reactor (this->allocateReactorImpl ());

  return reactor;
}
