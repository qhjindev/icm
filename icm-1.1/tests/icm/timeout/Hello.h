
#ifndef __Hello_h__
#define __Hello_h__

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

class AMI_MyHello_sayHello : public ReplyHandler
{
public:

  virtual void response(const ::std::string&, Long) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello
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

  virtual void sayHello_async(::demo::AMH_MyHello_sayHello*, const ::std::string&, Short) = 0;
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
  void sayHello_async(::demo::AMI_MyHello_sayHello*, const ::std::string&, Short);
};

}

}

namespace IcmAsync
{

namespace demo
{

class AMH_MyHello_sayHello : public ::demo::AMH_MyHello_sayHello, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello(ServerRequest&);

  virtual void response(const ::std::string&, Long);
};

}

}

#endif
