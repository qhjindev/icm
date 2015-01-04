
#ifndef __my_h__
#define __my_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace demo
{

struct package
{
  ::std::string str1;
  Long length;
  Short op;

  bool operator==(const package&) const;
  bool operator!=(const package&) const;

  void __write(OutputStream&) const;
  void __read(InputStream&);
};

typedef ::std::vector< ::demo::package> MsgSeq;

class __U__MsgSeq { };
void __write(OutputStream&, const ::demo::package*, const ::demo::package*, __U__MsgSeq);
void __read(InputStream&, MsgSeq&, __U__MsgSeq);

struct MyStruct
{
  ::demo::MsgSeq abc;
  ::std::string str;

  bool operator==(const MyStruct&) const;
  bool operator!=(const MyStruct&) const;

  void __write(OutputStream&) const;
  void __read(InputStream&);
};

}

namespace demo
{

class AMI_MyHello_sayHello : public ReplyHandler
{
public:

  virtual void response(const ::std::string&, Long) = 0;
protected:

  virtual void __response(InputStream& in, ACE_CDR::ULong replyStatus);
};

class AMH_MyHello_sayBye
{
public:

  virtual void response(const ::std::string&, Long) = 0;
};

class AMI_MyHello_testHello : public ReplyHandler
{
public:

  virtual void response(Long, Double, const ::std::string&) = 0;
protected:

  virtual void __response(InputStream& in, ACE_CDR::ULong replyStatus);
};

class AMH_MyHello_testHello
{
public:

  virtual void response(Long, Double, const ::std::string&) = 0;
};

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

class AMH_MyHello_testHello : public ::demo::AMH_MyHello_testHello, public AmhResponseHandler
{
public:

  AMH_MyHello_testHello(ServerRequest&);

  virtual void response(Long, Double, const ::std::string&);
};

}

}

namespace demo
{

class A : virtual public Object
{
public:

  A() {}

  virtual ::std::string getV() = 0;
  DispatchStatus ___getV(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

class MyHello : virtual public Object
{
public:

  MyHello() {}

  virtual ::std::string sayHello(const ::std::string&, Short, Long&) = 0;
  DispatchStatus ___sayHello(ServerRequest&);

  virtual void sayBye_async(::demo::AMH_MyHello_sayBye*, const ::std::string&, Short) = 0;
  DispatchStatus ___sayBye(ServerRequest&);

  virtual void testHello_async(::demo::AMH_MyHello_testHello*, Long, Short, Float) = 0;
  DispatchStatus ___testHello(ServerRequest&);

  virtual Long testSeq(const ::demo::MyStruct&, Long&) = 0;
  DispatchStatus ___testSeq(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace demo
{

class A : virtual public IcmProxy::Object
{
public:

  ::std::string getV();
};

class MyHello : virtual public IcmProxy::Object
{
public:

  ::std::string sayHello(const ::std::string& msg, Short u, Long& v);
  void sayHello_async(::demo::AMI_MyHello_sayHello*, const ::std::string&, Short);

  ::std::string sayBye(const ::std::string& msg, Short u, Long& v);

  Long testHello(Long v1, Short v2, Float v3, Double& v4, ::std::string& v5);
  void testHello_async(::demo::AMI_MyHello_testHello*, Long, Short, Float);

  Long testSeq(const ::demo::MyStruct& cd, Long& lvalue);
};

}

}

#endif
