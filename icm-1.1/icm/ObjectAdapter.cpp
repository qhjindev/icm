#include <string>
#include "icm/ObjectAdapter.h"
#include "icm/Communicator.h"
#include "icm/Object.h"
#include "icm/ServantManager.h"
#include "icm/ServantLocator.h"
#include "icm/ResourceFactory.h"

using namespace std;

ObjectAdapter::ObjectAdapter(Communicator* communicator,
                             const std::string& name,
                             const std::string& endpointInfo) :
  mCommunicator(communicator),
  mName(name),
  mServantManager(new ServantManager)
{
  string ip;
  unsigned short port;

  istringstream iss(endpointInfo);
  iss >> ip;
  iss >> port;

  mEndpoint.set("TCP", ip.c_str(), port);
  AcceptorRegistry* ar = this->mCommunicator->resourceFactory ()->getAcceptorRegistry ();
  ar->open (this->mCommunicator, this, &mEndpoint);
}

ObjectAdapter::ObjectAdapter(Communicator* communicator,
                             const std::string& name,
                             Endpoint* endpoint) :
  mCommunicator(communicator),
  mName(name),
  mEndpoint(*endpoint),
  mServantManager(new ServantManager)
{
  AcceptorRegistry* ar = this->mCommunicator->resourceFactory ()->getAcceptorRegistry ();
  ar->open (this->mCommunicator, this, endpoint);
}

ObjectAdapter::ObjectAdapter(Communicator* communicator,
                             const std::string& name) :
  mCommunicator(communicator),
  mName(name),
  mServantManager(new ServantManager)
{

}

ObjectAdapter::~ObjectAdapter()
{

}

std::string ObjectAdapter::getName() const
{
  return this->mName;
}

Communicator* ObjectAdapter::getCommunicator() const
{
  return this->mCommunicator;
}

bool
ObjectAdapter::add(Object* obj, Identity& ident)
{
  return this->mServantManager->addServant(obj,ident);
//  return this->createProxy(ident);
}

bool
ObjectAdapter::add(Object* obj, const std::string& name, const std::string& category)
{
  Identity ident(name, category);
  return this->mServantManager->addServant(obj,ident);
//  return this->createProxy(ident);
}

void ObjectAdapter::remove(const std::string& name, const std::string& category) {
  Identity ident(name, category);
  this->mServantManager->removeServant(ident);
}

void ObjectAdapter::remove(Identity& ident)
{
  this->mServantManager->removeServant(ident);
}

Object* ObjectAdapter::find(ServerRequest& serverRequest, Identity& ident) const
{
  if (ident.category != "") {
    ServantLocator* locator = this->mServantManager->findServantLocator(ident.category);

    if(locator == 0)
      return 0;

    return locator->locate (serverRequest, 0);
  }

  return this->mServantManager->findServant(ident);
}

void
ObjectAdapter::addServantLocator(ServantLocator* locator, const std::string& prefix)
{
  this->mServantManager->addServantLocator(locator, prefix);
}

IcmProxy::Object*
ObjectAdapter::createProxy(const Identity& id)
{
  Reference* reference = this->mCommunicator->referenceFactory()->create(id, mEndpoint);
  return this->mCommunicator->proxyFactory()->referenceToProxy(reference);
}

void
ObjectAdapter::dumpServant() const
{
  this->mServantManager->dumpServant();
}

