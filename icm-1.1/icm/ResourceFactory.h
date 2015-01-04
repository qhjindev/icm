#ifndef ICM_RESOURCE_FACTORY_H
#define ICM_RESOURCE_FACTORY_H

#include <map>
#include <string>

#include "icm/Endpoint.h"

class Communicator;
class ObjectAdapter;
class Reactor;
class ReactorImpl;
class IcmProtocolFactory;
class IcmConnector;
class IcmAcceptor;
class IcmTransport;
class Reference;
class TimeValue;

//#include "icm/AcceptorRegistry.h"

typedef std::map<std::string, IcmProtocolFactory*> IcmProtocolFactoryMap;
typedef std::map<std::string, IcmProtocolFactory*>::iterator IcmProtocolFactoryMapIter;

typedef std::map<std::string, IcmConnector*> IcmConnectorMap;
typedef std::map<std::string, IcmConnector*>::iterator IcmConnectorMapIter;

typedef std::map<std::string, IcmAcceptor*> IcmAcceptorMap;
typedef std::map<std::string, IcmAcceptor*>::iterator IcmAcceptorMapIter;


class ConnectorRegistry 
{
public:
  ConnectorRegistry (void);

  ~ConnectorRegistry (void);

  IcmConnector* getConnector (const char* tag);

  int open (Communicator* comm);

  int closeAll (void);

  int connect (Reference* ref,
               IcmTransport*& transport,
               TimeValue* maxWaitTime = 0);

private:

  IcmConnectorMap mConnectors;

};

class AcceptorRegistry
{
public:
  AcceptorRegistry (void);

  ~AcceptorRegistry (void);

  int open (Communicator* comm, ObjectAdapter* oa, Endpoint* endpoint);

  void closeEndpoint(Endpoint* endpoint);

  int closeAll (void);

private:

  int openDefault (Communicator* comm);

  IcmAcceptorMap mAcceptors;

};

class ResourceFactory
{
public:
  ResourceFactory (void);

  ~ResourceFactory (void);

  virtual Reactor* createReactor (void);

  virtual AcceptorRegistry* getAcceptorRegistry (void);

  virtual ConnectorRegistry* getConnectorRegistry (void);

  virtual IcmProtocolFactoryMap* getProtocolFactories (void);

  virtual int initProtocolFactories (void);

  enum
  {
    ICM_REACTOR_SELECT_MT,
    ICM_REACTOR_SELECT_ST,
    ICM_REACTOR_TP
  };

protected:

  virtual ReactorImpl* allocateReactorImpl (void) const;

private:

  int mReactorType;

  IcmProtocolFactoryMap mProtocolFactories;

  AcceptorRegistry* mAcceptorRegistry;

  ConnectorRegistry* mConnectorRegistry;

};

#endif //ICM_RESOURCE_FACTORY_H
