#ifndef ICM_SERVANT_LOCATOR_H
#define ICM_SERVANT_LOCATOR_H

#include "icm/Object.h"

class ServantLocator
{
public:
  virtual Object* locate(ServerRequest& serverRequest, void* cookie) = 0;

  virtual void finished(ServerRequest& serverRequest, const Object*& servant, void* cookie) = 0;

  virtual void deactivate(const ::std::string&) = 0;
};

#endif //ICM_SERVANT_LOCATOR_H
