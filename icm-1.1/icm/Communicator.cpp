#include "icm/Communicator.h"
#include "icc/InetAddr.h"
#include "icc/TimeValue.h"
#include "icc/Reactor.h"
#include "icm/ResourceFactory.h"
#include "icm/ClientStrategyFactory.h"
#include "icm/ServerStrategyFactory.h"
#include "icm/TransportCacheManager.h"
#include "icm/ObjectAdapter.h"
#include "icm/Proxy.h"

Communicator* Communicator::mInstance = 0;

Communicator*
Communicator::instance()
{
	if(mInstance == 0)
		mInstance = new Communicator;

	return mInstance;
}

Communicator::Communicator()
{
  this->mReactor = 0;

  this->mReferenceFactory = new ReferenceFactory();

  this->mProxyFactory = new ProxyFactory(this);

  this->mResourceFactory = new ResourceFactory;

  this->mClientFactory = new ClientStrategyFactory;

  this->mServerFactory = new ServerStrategyFactory;

  this->mCacheTransport = false;
  this->mTransportCacheManager = 0;

  this->mTransportListener = 0;
}

Communicator::~Communicator()
{
  if(mReferenceFactory)
    delete mReferenceFactory;

  if(mProxyFactory)
    delete mProxyFactory;

  if(mResourceFactory)
    delete mResourceFactory;

  if(mClientFactory)
    delete mClientFactory;

  if(mServerFactory)
    delete mServerFactory;

  if(mTransportCacheManager)
    delete mTransportCacheManager;
}

int
Communicator::run (TimeValue* tv)
{
  Reactor* r = this->reactor ();

  bool deleteTv = false;
  if(tv == 0) {
    tv = new TimeValue(3);
    deleteTv = true;
  }
  while (1) {
    r->handleEvents (tv);
  }

  if(deleteTv == true){
    delete tv;
  }
  return 0;
}

IcmProxy::Object*
Communicator::toProxy (const char* name,
                       const char* category,
                       const char* prot, 
                       const char* host, 
                       unsigned short port)
{
  return this->mProxyFactory->toProxy (name, category, prot, host, port);
}

ObjectAdapter* 
Communicator::createObjectAdapterWithEndpoint(const std::string& name, const std::string& endpointInfo)
{
  mObjectAdapter = new ObjectAdapter (this, name, endpointInfo);
  return mObjectAdapter;
}

ObjectAdapter* 
Communicator::createObjectAdapterWithEndpoint(const std::string& name, Endpoint* endpoint)
{
  mObjectAdapter = new ObjectAdapter (this, name, endpoint);
  return mObjectAdapter;
}

ObjectAdapter* 
Communicator::createObjectAdapter(const std::string& name)
{
  mObjectAdapter = new ObjectAdapter (this, name);
  return mObjectAdapter;
}

int Communicator::stopEndpoint(Endpoint* endpoint) {
  if(endpoint == 0) {
    return 0;
  }

  assert(mObjectAdapter != 0);
  AcceptorRegistry* ar = resourceFactory ()->getAcceptorRegistry ();
  ar->closeEndpoint(endpoint);
  return 0;
}

int Communicator::resumeEndpoint(Endpoint* endpoint) {
  if(endpoint == 0) {
    return 0;
  }

  assert(mObjectAdapter != 0);
  AcceptorRegistry* ar = resourceFactory ()->getAcceptorRegistry ();
  return ar->open (this, this->mObjectAdapter, endpoint);
}

ObjectAdapter* 
Communicator::objectAdapter (void)
{
  return this->mObjectAdapter;
}

ReferenceFactory* 
Communicator::referenceFactory() const
{
  return this->mReferenceFactory;
}

ProxyFactory* 
Communicator::proxyFactory() const
{
  return this->mProxyFactory;
}

void 
Communicator::defaultContext(const Context& ctx)
{
  this->mDefaultContext = ctx;
}

Context 
Communicator::defaultContext() const
{
  return this->mDefaultContext;
}

int
Communicator::init (bool cacheTransport)
{
  //OS::socket_init (ACE_WSOCK_VERSION);

//  this->mCacheTransport = cacheTransport;
  this->mCacheTransport = false;

  if(cacheTransport)
    this->mTransportCacheManager = new TransportCacheManager;

  ResourceFactory* rf = this->resourceFactory ();

  Reactor* reactor = this->reactor ();
  if (reactor == 0) {
    return -1;
  }

  ServerStrategyFactory* ssf = this->serverFactory ();

  if (ssf == 0) {
    return -1;
  }
  //ssf->open (this);

  if (rf->initProtocolFactories () == -1)
    return -1;

  this->mProtocolFactories = rf->getProtocolFactories ();

  if (this->connectorRegistry ()->open (this) != 0)
    return -1;

  return 0;
}

Reactor*
Communicator::reactor (void)
{
  if (this->mReactor == 0) {
    this->mReactor = this->resourceFactory ()->createReactor ();
  }

  return this->mReactor;
}

ResourceFactory*
Communicator::resourceFactory (void)
{
  if (this->mResourceFactory == 0) 
    this->mResourceFactory = new ResourceFactory;

  return this->mResourceFactory;
}

AcceptorRegistry*
Communicator::acceptorRegistry (void)
{
  return this->resourceFactory ()->getAcceptorRegistry ();
}

ConnectorRegistry*
Communicator::connectorRegistry (void)
{
  return this->resourceFactory ()->getConnectorRegistry ();
}

ClientStrategyFactory*
Communicator::clientFactory (void)
{
  if (this->mClientFactory == 0) 
    this->mClientFactory = new ClientStrategyFactory;

  return this->mClientFactory;
}

ServerStrategyFactory*
Communicator::serverFactory (void)
{
  if (this->mServerFactory == 0) 
    this->mServerFactory = new ServerStrategyFactory;

  return this->mServerFactory;
}

IcmProtocolFactoryMap*
Communicator::protocolFactories (void)
{
  return this->mResourceFactory->getProtocolFactories ();
}

TransportCacheManager*
Communicator::transportCache (void)
{
  if (this->mTransportCacheManager == 0)
    this->mTransportCacheManager = new TransportCacheManager;

  return this->mTransportCacheManager;
}

bool
Communicator::cacheTransport (void)
{
  return this->mCacheTransport;
}

TransportListener*
Communicator::transportListener(void)
{
  return this->mTransportListener;
}

void
Communicator::transportListener(TransportListener *listener)
{
  this->mTransportListener = listener;
}
