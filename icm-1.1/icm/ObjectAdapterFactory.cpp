
#include <string>
#include <map>

#include "icm/ObjectAdapterFactory.h"
#include "icm/ObjectAdapter.h"
#include "icm/Communicator.h"

using namespace std;

ObjectAdapterFactory::ObjectAdapterFactory(Communicator* comm) :
  mCommunicator(comm)
{

}

ObjectAdapterFactory::~ObjectAdapterFactory()
{

}

ObjectAdapter* 
ObjectAdapterFactory::createObjectAdapter(const string& name, const string& endpoints)
{
  std::map<std::string,ObjectAdapter*>::iterator p = this->mAdapters.find(name);
  if (p != this->mAdapters.end()) {
    return 0;
  }

  ObjectAdapter* adapter = new ObjectAdapter(this->mCommunicator,name,endpoints);
  this->mAdapters.insert(make_pair(name,adapter));
  return adapter;
}

ObjectAdapter* 
ObjectAdapterFactory::findObjectAdapter(const Object*)
{
  if(!this->mCommunicator) {
    return 0;
  }

  for(std::map<std::string,ObjectAdapter*>::iterator p = this->mAdapters.begin();
    p != this->mAdapters.end();
    ++p)
  {
  }

  return 0;
}

ObjectAdapter*
ObjectAdapterFactory::findObjectAdapter(const char *name)
{
  std::map<std::string, ObjectAdapter*>::iterator p = this->mAdapters.find (name);
  if (p != this->mAdapters.end ())
    return p->second;

  return 0;
}
