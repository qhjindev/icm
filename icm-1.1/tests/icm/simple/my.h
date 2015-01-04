
#ifndef __my_h__
#define __my_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace IcmProxy
{

namespace demo
{

class MyHello;

}

}

namespace demo
{

void __write(OutputStream*, IcmProxy::demo::MyHello* v);
void __read(InputStream*, IcmProxy::demo::MyHello* &v);

}

namespace demo
{

}

namespace demo
{

class MyHello : virtual public Object
{
public:

  MyHello() {}

  virtual ::std::string sayHello(const ::std::string&, Short, Long&) = 0;
  DispatchStatus ___sayHello(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace demo
{

class MyHello : virtual public IcmProxy::Object
{
public:

  ::std::string sayHello(const ::std::string& msg, Short u, Long& v);
};

}

}

#endif
