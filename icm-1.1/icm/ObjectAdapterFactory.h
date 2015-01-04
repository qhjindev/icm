#ifndef ICM_OBJECT_ADAPTER_FACTORY_H
#define ICM_OBJECT_ADAPTER_FACTORY_H

#include <string>
#include <map>

//#include "ObjectAdapter.h"

class ObjectAdapter;
class Communicator;
class Object;

class ObjectAdapterFactory
{
public:
  ObjectAdapter* createObjectAdapter(const std::string&,const std::string&);
  ObjectAdapter* findObjectAdapter(const Object*);
  ObjectAdapter* findObjectAdapter (const char* name);

private:
  ObjectAdapterFactory(Communicator*);
  virtual ~ObjectAdapterFactory();

  Communicator* mCommunicator;
  std::map<std::string, ObjectAdapter*> mAdapters;
};

#endif //ICM_OBJECT_ADAPTER_FACTORY_H
