#ifndef ICM_OBJECT_ADAPTER_H
#define ICM_OBJECT_ADAPTER_H

#include <string>
#include "icm/Reference.h"
#include "icm/Endpoint.h"

class Communicator;
class ServantManager;
class Object;
class ServerRequest;
class ServantLocator;

namespace IcmProxy
{
class Object;
}

class ObjectAdapter
{
public:
  ObjectAdapter(Communicator* communicator, const std::string& name, const std::string& endpointInfo);

  ObjectAdapter(Communicator* communicator, const std::string& name, Endpoint* endpoint);

  ObjectAdapter(Communicator* communicator, const std::string& name);

  ~ObjectAdapter();

  std::string getName() const;
  Communicator* getCommunicator() const;

  Endpoint* getEndpoint() {
    return &mEndpoint;
  }

  bool add(Object*, Identity&);

  bool add(Object*, const std::string& name, const std::string& category = "");

  void remove(Identity&);

  void remove(const std::string& name, const std::string& category = "");

  IcmProxy::Object* createProxy(const Identity&);

  Object* find(ServerRequest& , Identity&) const;

  void addServantLocator(ServantLocator* locator, const std::string& prefix);

  void dumpServant() const;

private:
  Communicator* mCommunicator;

  const std::string mName;
  const std::string mId;

  Endpoint mEndpoint;

  ServantManager* mServantManager;
};

#endif //ICM_OBJECT_ADAPTER_H
