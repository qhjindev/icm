
#ifndef __Progress_h__
#define __Progress_h__

#include <string>
#include <vector>
#include <list>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace IcmProxy
{

namespace My
{

class MyCallback;

class VmService;

}

}

namespace My
{

void __write(OutputStream*, IcmProxy::My::MyCallback* v);
void __read(InputStream*, IcmProxy::My::MyCallback* &v);

void __write(OutputStream*, IcmProxy::My::VmService* v);
void __read(InputStream*, IcmProxy::My::VmService* &v);

}

namespace My
{

struct OpProgress
{
  OpProgress();
  Int seq;
  ::std::string status;

  bool operator==(const OpProgress&) const;
  bool operator!=(const OpProgress&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const OpProgress&);

}

namespace My
{

class MyCallback : virtual public Object
{
public:

  MyCallback() {}

  virtual void reportProgress(const ::My::OpProgress&) = 0;
  DispatchStatus ___reportProgress(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

class VmService : virtual public Object
{
public:

  VmService() {}

  virtual Int setCallback(IcmProxy::My::MyCallback *) = 0;
  DispatchStatus ___setCallback(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace My
{

class MyCallback : virtual public IcmProxy::Object
{
public:

  void reportProgress(const ::My::OpProgress& progress);
};

class VmService : virtual public IcmProxy::Object
{
public:

  Int setCallback(IcmProxy::My::MyCallback * cb);
};

}

}

#endif
