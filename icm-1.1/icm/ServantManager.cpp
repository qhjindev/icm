
#include "icm/ServantManager.h"
#include "icm/Reference.h"
#include <iostream>

using namespace std;

ServantManager::ServantManager (void)
{
}

ServantManager::~ServantManager (void)
{
}

void ServantManager::dumpServant() const
{
  ServantMap::const_iterator it = this->mServantMap.begin();
  while(it != this->mServantMap.end())
  {
    cout << "ident.name: " << it->first << " obj:" << it->second << endl;
    it++;
  }
}

bool ServantManager::addServant(Object *obj, Identity &ident)
{
  ServantMap::iterator p = this->mServantMap.find(ident.name);
  if(p == this->mServantMap.end()) {
    this->mServantMap.insert(p, make_pair(ident.name,obj));
  } else {
    return false;
  }

  return true;
}

Object* 
ServantManager::removeServant(const Identity& ident)
{
  Object* servant = 0;
  ServantMap::iterator p = this->mServantMap.find(ident.name);
  if(p != this->mServantMap.end()) {
    servant = p->second;
    this->mServantMap.erase(p);
  }

  return servant;
}

Object* 
ServantManager::findServant(const Identity& ident) const
{
  Object* servant = 0;
  ServantMap::const_iterator p = this->mServantMap.find(ident.name);
  if(p != this->mServantMap.end()) {
    servant = p->second;    
  }

  return servant;
}

bool 
ServantManager::hasServant(const Identity& ident) const
{
//  Object* servant = 0;
  ServantMap::const_iterator p = this->mServantMap.find(ident.name);
  return p != this->mServantMap.end();
}

void
ServantManager::addServantLocator(ServantLocator *locator, const std::string &category)
{
  this->mLocatorMap.insert(pair<const string, ServantLocator*> (category, locator));
}

ServantLocator*
ServantManager::findServantLocator(const std::string &category) const
{
  LocatorMapIter iter = this->mLocatorMap.find(category);
  if (iter != this->mLocatorMap.end())
    return iter->second;

  return 0;
}
