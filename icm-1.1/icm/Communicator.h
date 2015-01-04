#ifndef ICM_COMMUNICATOR_H
#define ICM_COMMUNICATOR_H

#include <string>
#include <map>
#include <set>

#include "icc/Log.h"
#include "icm/Reference.h"
#include "icm/Proxy.h"
#include "IcmError.h"

class TimeValue;
class ResourceFactory;
class IcmProtocolFactory;
class ClientStrategyFactory;
class ServerStrategyFactory;
class TransportCacheManager;
class ConnectorRegistry;
class AcceptorRegistry;
class Reactor;
class ObjectAdapter;
class TransportListener;
class IcmTransport;

class Communicator
{
public:
  Communicator();

  static Communicator* instance();

  ~Communicator();

  int init (bool cacheTransport = false);

  int run (TimeValue* tv = 0);

  IcmProxy::Object* toProxy (const char* name,
                             const char* category,
                             const char* prot, 
                             const char* host, 
                             unsigned short port);

  ObjectAdapter* createObjectAdapter(const std::string&);

  ObjectAdapter* createObjectAdapterWithEndpoint(const std::string&, const std::string&);

  ObjectAdapter* createObjectAdapterWithEndpoint(const std::string&, Endpoint*);

  int stopEndpoint(Endpoint*);

  int resumeEndpoint(Endpoint*);

  ObjectAdapter* objectAdapter (void);

  ReferenceFactory* referenceFactory() const;

  ProxyFactory* proxyFactory() const;

  void defaultContext(const Context& ctx);

  Context defaultContext() const;

  ClientStrategyFactory* clientFactory (void);

  ServerStrategyFactory* serverFactory (void);

  ResourceFactory* resourceFactory (void);

  ConnectorRegistry* connectorRegistry (void);

  AcceptorRegistry* acceptorRegistry (void);

  std::map<std::string, IcmProtocolFactory*>* protocolFactories (void);

  TransportCacheManager* transportCache (void);

  bool cacheTransport(void);

  TransportListener* transportListener (void);

  void transportListener (TransportListener* listener);

  Reactor* reactor (void);

  void addValidTransport(IcmTransport* transport) {
    validTransports.insert(transport);
  }

  void removeValidTransport(IcmTransport* transport) {
    validTransports.erase(transport);
  }

  bool validateTransport(IcmTransport* transport) {
    return validTransports.find(transport) != validTransports.end();
  }
private:

  static Communicator* mInstance;

  ReferenceFactory* mReferenceFactory;

  ProxyFactory* mProxyFactory;

  Context mDefaultContext;

  ResourceFactory* mResourceFactory;

  ClientStrategyFactory* mClientFactory;

  ServerStrategyFactory* mServerFactory;

  std::map<std::string, IcmProtocolFactory*>* mProtocolFactories;

  TransportCacheManager* mTransportCacheManager;

  bool mCacheTransport;

  Reactor* mReactor;

  ObjectAdapter* mObjectAdapter;

  TransportListener* mTransportListener;

  std::set<IcmTransport*> validTransports;
};

#endif //ICM_COMMUNICATOR_H
