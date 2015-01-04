
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

struct S1
{
  Int id;
  ::std::string name;

  bool operator==(const S1&) const;
  bool operator!=(const S1&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const S1&);

typedef ::std::vector< ::demo::S1> Reqs;

class __U__Reqs { };
std::string __toString(const Reqs&);
void __write(OutputStream*, const ::demo::S1*, const ::demo::S1*, __U__Reqs);
void __read(InputStream*, Reqs&, __U__Reqs);

typedef ::std::map<Short, ::demo::S1> Dict;

class __U__Dict { };
void __write(OutputStream*, const Dict&, __U__Dict);
void __read(InputStream*, Dict&, __U__Dict);
std::string __toString(const Dict&);

typedef ::std::map< ::std::string, ::demo::Reqs> Dicta;

class __U__Dicta { };
void __write(OutputStream*, const Dicta&, __U__Dicta);
void __read(InputStream*, Dicta&, __U__Dicta);
std::string __toString(const Dicta&);

typedef ::std::vector< ::demo::Dicta> Com;

class __U__Com { };
std::string __toString(const Com&);
void __write(OutputStream*, const ::demo::Dicta*, const ::demo::Dicta*, __U__Com);
void __read(InputStream*, Com&, __U__Com);

struct TestReq
{
  Int syn;
  ::demo::Reqs reqs1;

  bool operator==(const TestReq&) const;
  bool operator!=(const TestReq&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const TestReq&);

struct TestReqDict
{
  Int syn;
  ::demo::Dict dict1;

  bool operator==(const TestReqDict&) const;
  bool operator!=(const TestReqDict&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const TestReqDict&);

struct TestReqCom
{
  Int syn;
  ::demo::Com com1;

  bool operator==(const TestReqCom&) const;
  bool operator!=(const TestReqCom&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const TestReqCom&);

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

class AMI_MyHello_testSequence : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_testSequence
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_testDictionary : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_testDictionary
{
public:

  virtual void response(const ::std::string&) = 0;
};

class AMI_MyHello_testCompound : public ReplyHandler
{
public:

  virtual void response(const ::std::string&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

class AMH_MyHello_testCompound
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

  virtual void sayHello_async(::demo::AMH_MyHello_sayHello*, const ::std::string&, Short) = 0;
  DispatchStatus ___sayHello(ServerRequest&);

  virtual void testSequence_async(::demo::AMH_MyHello_testSequence*, const ::demo::TestReq&) = 0;
  DispatchStatus ___testSequence(ServerRequest&);

  virtual void testDictionary_async(::demo::AMH_MyHello_testDictionary*, const ::demo::TestReqDict&) = 0;
  DispatchStatus ___testDictionary(ServerRequest&);

  virtual void testCompound_async(::demo::AMH_MyHello_testCompound*, const ::demo::TestReqCom&) = 0;
  DispatchStatus ___testCompound(ServerRequest&);

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

  ::std::string testSequence(const ::demo::TestReq& req);
  void testSequence_async(::demo::AMI_MyHello_testSequence*, const ::demo::TestReq&);

  ::std::string testDictionary(const ::demo::TestReqDict& req);
  void testDictionary_async(::demo::AMI_MyHello_testDictionary*, const ::demo::TestReqDict&);

  ::std::string testCompound(const ::demo::TestReqCom& req);
  void testCompound_async(::demo::AMI_MyHello_testCompound*, const ::demo::TestReqCom&);
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

class AMH_MyHello_testSequence : public ::demo::AMH_MyHello_testSequence, public AmhResponseHandler
{
public:

  AMH_MyHello_testSequence(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_testDictionary : public ::demo::AMH_MyHello_testDictionary, public AmhResponseHandler
{
public:

  AMH_MyHello_testDictionary(ServerRequest&);

  virtual void response(const ::std::string&);
};

class AMH_MyHello_testCompound : public ::demo::AMH_MyHello_testCompound, public AmhResponseHandler
{
public:

  AMH_MyHello_testCompound(ServerRequest&);

  virtual void response(const ::std::string&);
};

}

}

#endif
