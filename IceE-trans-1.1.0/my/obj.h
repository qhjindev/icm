
#ifndef __obj_h__
#define __obj_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace Test
{

}

namespace IcmProxy
{

namespace Test
{

class B : virtual public IcmProxy::Object
{
public:

  ::IcmProxy::Object* getObj();

  Int setObj(IcmProxy::Object* obj);
};

}

}

namespace Test
{

class B : virtual public Object
{
public:

  B() {}

  virtual ::IcmProxy::Object* getObj() = 0;
  DispatchStatus ___getObj(ServerRequest&);

  virtual Int setObj(IcmProxy::Object*) = 0;
  DispatchStatus ___setObj(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace Test
{

void __write(OutputStream*, IcmProxy::Test::B* v);
void __read(InputStream*, IcmProxy::Test::B* v);

}

#endif
