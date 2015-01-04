
#ifndef __Hello_h__
#define __Hello_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace IcmProxy
{

namespace demo
{

class Hello;

}

}

namespace demo
{

void __write(OutputStream*, IcmProxy::demo::Hello* v);
void __read(InputStream*, IcmProxy::demo::Hello* &v);

}

namespace demo
{

}

namespace demo
{

class Hello : virtual public Object
{
public:

  Hello() {}

  virtual ::std::string sayHello(const ::std::string&, Short, Long&) = 0;
  DispatchStatus ___sayHello(ServerRequest&);

  virtual ::std::string sayBye(const ::std::string&, Short, Long&) = 0;
  DispatchStatus ___sayBye(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace demo
{

class Hello : virtual public IcmProxy::Object
{
public:

  ::std::string sayHello(const ::std::string& msg, Short u, Long& v);

  ::std::string sayBye(const ::std::string& msg, Short u, Long& v);
  void sayBye_async(::demo::AMI_Hello_sayBye*, const ::std::string&, Short);
};

}

}

namespace demo
{

class AMI_Hello_sayBye : public ReplyHandler
{
public:

  virtual void response(const ::std::string&, Long) = 0;
protected:

  virtual void __response(InputStream& in, ACE_CDR::ULong replyStatus);
};

}

#endif
