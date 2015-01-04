#ifndef ICM_OBJECT_H
#define ICM_OBJECT_H

#include <string>
#include <map>
#include <vector>
//#include "Current.h"

class ServerRequest;

enum DispatchStatus
{
  DispatchOK,
  DispatchUserException,
  DispatchObjectNotExist,
  DispatchFacetNotExist,
  DispatchOperationNotExist,
  DispatchUnknownLocalException,
  DispatchUnknownUserException,
  DispatchUnknownException,
  DispatchAsync
// "Pseudo dispatch status", used internally only to indicate async dispatch.
};

class Object
{
public:

  virtual bool ping() const
  {
    return true;
  }

  virtual DispatchStatus __dispatch(ServerRequest&) = 0;
};

class Blobject : virtual public Object
{
public:

    // Returns true if ok, false if user exception.
    virtual bool icm_invoke(ServerRequest& request) = 0;
    virtual DispatchStatus __dispatch(ServerRequest&);
};


#endif //ICM_OBJECT_H
