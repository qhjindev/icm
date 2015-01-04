#ifndef ICM_SERVANT_MANAGER_H
#define ICM_SERVANT_MANAGER_H

#include <iostream>
#include <map>
#include <string>
#include "Reference.h"
#include "ServantLocator.h"

class Object;
class ServantManager
{
  friend class ObjectAdapter;

public:

  bool addServant(Object*, Identity&);

  Object* removeServant(const Identity&);

  Object* findServant(const Identity&) const;

  bool hasServant(const Identity&) const;

  void dumpServant(void) const;

  void addServantLocator(ServantLocator* locator, const std::string&);

  ServantLocator* findServantLocator(const std::string&) const;

private:
  
  ServantManager();
  ~ServantManager();

  typedef std::map<std::string,Object*> ServantMap;

  typedef std::map<std::string, ServantLocator*> LocatorMap;
  typedef std::map<std::string, ServantLocator*>::const_iterator LocatorMapIter;

  ServantMap mServantMap;
  LocatorMap mLocatorMap;
};

#endif //ICM_SERVANT_MANAGER_H
