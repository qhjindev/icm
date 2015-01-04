
#ifndef __my_amd_h__
#define __my_amd_h__

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

class AMH_MyHello_sayBye
{
public:

  virtual void response(const ::std::string&, Long) = 0;
};

}

namespace demo
{

class MyHello : virtual public Object
{
public:

  MyHello() {}

  virtual void sayBye_async(::demo::AMH_MyHello_sayBye*, const ::std::string&, Short) = 0;
  DispatchStatus ___sayBye(ServerRequest&);

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

  ::std::string sayBye(const ::std::string& msg, Short u, Long& v);
};

}

}

namespace IcmAsync
{

namespace demo
{

class AMH_MyHello_sayBye : public ::demo::AMH_MyHello_sayBye, public AmhResponseHandler
{
public:

  AMH_MyHello_sayBye(ServerRequest&);

  virtual void response(const ::std::string&, Long);
};

}

}

#endif
