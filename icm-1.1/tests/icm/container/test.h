
#ifndef __test_h__
#define __test_h__

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

typedef ::std::vector< ::std::string> Req1;

class __U__Req1 { };
std::string __toString(const Req1&);

typedef ::std::vector<Int> Req1I;

class __U__Req1I { };
std::string __toString(const Req1I&);

typedef ::std::map<Int, ::std::string> Req2;

class __U__Req2 { };
void __write(OutputStream*, const Req2&, __U__Req2);
void __read(InputStream*, Req2&, __U__Req2);
std::string __toString(const Req2&);

struct Req3
{
  Req3();
  Int syn;
  ::demo::Req1 request1;

  bool operator==(const Req3&) const;
  bool operator!=(const Req3&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const Req3&);

struct Req4
{
  Req4();
  Int syn;
  ::demo::Req1 request1;
  Int syn2;

  bool operator==(const Req4&) const;
  bool operator!=(const Req4&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const Req4&);

typedef ::std::vector< ::demo::Req3> Req1R;

class __U__Req1R { };
std::string __toString(const Req1R&);
void __write(OutputStream*, const ::demo::Req3*, const ::demo::Req3*, __U__Req1R);
void __read(InputStream*, Req1R&, __U__Req1R);

}

namespace demo
{

class AMI_MyHello_sayHello1 : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello1
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_sayHello1I : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello1I
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_sayHello1R : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello1R
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_sayHello2 : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello2
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_sayHello3 : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello3
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_sayHello4 : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_sayHello4
{
public:

  virtual void response(const ::std::string&) = 0;
};

}

namespace demo
{

class MyHello : virtual public Object
{
public:

  MyHello() {}

  virtual void sayHello1_async(::demo::AMH_MyHello_sayHello1*, const ::demo::Req1&) = 0;
  DispatchStatus ___sayHello1(ServerRequest&);

  virtual void sayHello1I_async(::demo::AMH_MyHello_sayHello1I*, const ::demo::Req1I&) = 0;
  DispatchStatus ___sayHello1I(ServerRequest&);

  virtual void sayHello1R_async(::demo::AMH_MyHello_sayHello1R*, const ::demo::Req1R&) = 0;
  DispatchStatus ___sayHello1R(ServerRequest&);

  virtual void sayHello2_async(::demo::AMH_MyHello_sayHello2*, const ::demo::Req2&) = 0;
  DispatchStatus ___sayHello2(ServerRequest&);

  virtual void sayHello3_async(::demo::AMH_MyHello_sayHello3*, const ::demo::Req3&) = 0;
  DispatchStatus ___sayHello3(ServerRequest&);

  virtual void sayHello4_async(::demo::AMH_MyHello_sayHello4*, const ::demo::Req4&) = 0;
  DispatchStatus ___sayHello4(ServerRequest&);

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

  ::std::string sayHello1(const ::demo::Req1& req);
  void sayHello1_async(::demo::AMI_MyHello_sayHello1*, const ::demo::Req1&);

  ::std::string sayHello1I(const ::demo::Req1I& req);
  void sayHello1I_async(::demo::AMI_MyHello_sayHello1I*, const ::demo::Req1I&);

  ::std::string sayHello1R(const ::demo::Req1R& req);
  void sayHello1R_async(::demo::AMI_MyHello_sayHello1R*, const ::demo::Req1R&);

  ::std::string sayHello2(const ::demo::Req2& req);
  void sayHello2_async(::demo::AMI_MyHello_sayHello2*, const ::demo::Req2&);

  ::std::string sayHello3(const ::demo::Req3& req);
  void sayHello3_async(::demo::AMI_MyHello_sayHello3*, const ::demo::Req3&);

  ::std::string sayHello4(const ::demo::Req4& req);
  void sayHello4_async(::demo::AMI_MyHello_sayHello4*, const ::demo::Req4&);
};

}

}

namespace IcmAsync
{

namespace demo
{

class AMH_MyHello_sayHello1 : public ::demo::AMH_MyHello_sayHello1, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello1(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_sayHello1I : public ::demo::AMH_MyHello_sayHello1I, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello1I(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_sayHello1R : public ::demo::AMH_MyHello_sayHello1R, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello1R(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_sayHello2 : public ::demo::AMH_MyHello_sayHello2, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello2(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_sayHello3 : public ::demo::AMH_MyHello_sayHello3, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello3(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_sayHello4 : public ::demo::AMH_MyHello_sayHello4, public AmhResponseHandler
{
public:

  AMH_MyHello_sayHello4(ServerRequest&);

  virtual void response(const ::std::string&);
};

}

}

#endif
