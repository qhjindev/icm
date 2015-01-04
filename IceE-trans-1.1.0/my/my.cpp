
#include <algorithm>
#include <string>
#include "my.h"
#include <icm/ServerRequest.h>
#include <icc/CdrStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

bool
demo::package::operator==(const package& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::package::operator!=(const package& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(str1 != __rhs.str1)
  {
    return true;
  }
  if(length != __rhs.length)
  {
    return true;
  }
  if(op != __rhs.op)
  {
    return true;
  }
  return false;
}

void
demo::package::__write(OutputStream& __os) const
{
  __os.write_string(str1);
  __os.write_long(length);
  __os.write_short(op);
}

void
demo::package::__read(InputStream& __is)
{
  __is.read_string(str1);
  __is.read_long(length);
  __is.read_short(op);
}

void
demo::__write(OutputStream& __os, const ::demo::package* begin, const ::demo::package* end, ::demo::__U__MsgSeq)
{
  Short size = static_cast< Short>(end - begin);
  __os.write_short(size);
  for(int i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
demo::__read(InputStream& __is, ::demo::MsgSeq& v, ::demo::__U__MsgSeq)
{
  Short sz;
  __is.read_short(sz);
  v.resize(sz);
  for(int i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

bool
demo::MyStruct::operator==(const MyStruct& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::MyStruct::operator!=(const MyStruct& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(abc != __rhs.abc)
  {
    return true;
  }
  if(str != __rhs.str)
  {
    return true;
  }
  return false;
}

void
demo::MyStruct::__write(OutputStream& __os) const
{
  if(abc.size() == 0)
  {
    __os.write_short(0);
  }
  else
  {
    ::demo::__write(__os, &abc[0], &abc[0] + abc.size(), ::demo::__U__MsgSeq());
  }
  __os.write_string(str);
}

void
demo::MyStruct::__read(InputStream& __is)
{
  ::demo::__read(__is, abc, ::demo::__U__MsgSeq());
  __is.read_string(str);
}

void
demo::AMI_MyHello_sayHello::__response(InputStream& in, ACE_CDR::ULong replyStatus)
{
  if (replyStatus == 0)
  {
    Long v;
    ::std::string __ret;
    in.read_long(v);
    in.read_string(__ret);
    this->response(__ret, v);
  }
}

void
demo::AMI_MyHello_testHello::__response(InputStream& in, ACE_CDR::ULong replyStatus)
{
  if (replyStatus == 0)
  {
    Double v4;
    ::std::string v5;
    Long __ret;
    in.read_double(v4);
    in.read_string(v5);
    in.read_long(__ret);
    this->response(__ret, v4, v5);
  }
}

IcmAsync::demo::AMH_MyHello_sayBye::AMH_MyHello_sayBye(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayBye::response(const ::std::string& __ret, Long v)
{
  this->initReply();
  this->mOutStream.write_long(v);
  this->mOutStream.write_string(__ret);
  this->sendReply();
}

IcmAsync::demo::AMH_MyHello_testHello::AMH_MyHello_testHello(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_testHello::response(Long __ret, Double v4, const ::std::string& v5)
{
  this->initReply();
  this->mOutStream.write_double(v4);
  this->mOutStream.write_string(v5);
  this->mOutStream.write_long(__ret);
  this->sendReply();
}

static const ::std::string __demo__A_ids[2] =
{
  "::Ice::Object",
  "::demo::A"
};

DispatchStatus
demo::A::___getV(ServerRequest& __inS)
{
  OutputStream& __os = __inS.outgoing();
  ::std::string __ret = getV();
  __inS.initReply ();
  __os.write_string(__ret);
  return DispatchOK;
}

static ::std::string __demo__A_all[] =
{
  "getV"
};

DispatchStatus
demo::A::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__A_all, __demo__A_all + 1, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __demo__A_all)
  {
    case 0:
    {
      return ___getV(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

static const ::std::string __demo__MyHello_ids[2] =
{
  "::Ice::Object",
  "::demo::MyHello"
};

DispatchStatus
demo::MyHello::___sayHello(ServerRequest& __inS)
{
  InputStream& __is = __inS.incoming();
  OutputStream& __os = __inS.outgoing();
  ::std::string msg;
  Short u;
  __is.read_string(msg);
  __is.read_short(u);
  Long v;
  ::std::string __ret = sayHello(msg, u, v);
  __inS.initReply ();
  __os.write_long(v);
  __os.write_string(__ret);
  return DispatchOK;
}

DispatchStatus
demo::MyHello::___sayBye(ServerRequest& __inS)
{
  InputStream& __is = __inS.incoming();
  OutputStream& __os = __inS.outgoing();
  ::std::string msg;
  Short u;
  __is.read_string(msg);
  __is.read_short(u);
  ::demo::AMH_MyHello_sayBye* __cb = new IcmAsync::demo::AMH_MyHello_sayBye(__inS);
  sayBye_async(__cb, msg, u);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___testHello(ServerRequest& __inS)
{
  InputStream& __is = __inS.incoming();
  OutputStream& __os = __inS.outgoing();
  Long v1;
  Short v2;
  Float v3;
  __is.read_long(v1);
  __is.read_short(v2);
  __is.read_float(v3);
  ::demo::AMH_MyHello_testHello* __cb = new IcmAsync::demo::AMH_MyHello_testHello(__inS);
  testHello_async(__cb, v1, v2, v3);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___testSeq(ServerRequest& __inS)
{
  InputStream& __is = __inS.incoming();
  OutputStream& __os = __inS.outgoing();
  ::demo::MyStruct cd;
  cd.__read(__is);
  Long lvalue;
  Long __ret = testSeq(cd, lvalue);
  __inS.initReply ();
  __os.write_long(lvalue);
  __os.write_long(__ret);
  return DispatchOK;
}

static ::std::string __demo__MyHello_all[] =
{
  "sayBye",
  "sayHello",
  "testHello",
  "testSeq"
};

DispatchStatus
demo::MyHello::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__MyHello_all, __demo__MyHello_all + 4, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __demo__MyHello_all)
  {
    case 0:
    {
      return ___sayBye(in);
    }
    case 1:
    {
      return ___sayHello(in);
    }
    case 2:
    {
      return ___testHello(in);
    }
    case 3:
    {
      return ___testSeq(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

::std::string
IcmProxy::demo::A::getV()
{
  static const char* __operation("getV");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ret = _invocation.start (this->transport ());
  if (ret != 0)
    return ret;
  ret = _invocation.prepareHeader (1);
  if (ret != 0)
    return ret;
  ret = _invocation.invoke();
  if (ret != 0)
  return "";
  InputStream& __is = _invocation.inpStream();
  ::std::string __ret;
  __is.read_string(__ret);
  return __ret;
}

::std::string
IcmProxy::demo::MyHello::sayHello(const ::std::string& msg, Short u, Long& v)
{
  static const char* __operation("sayHello");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ret = _invocation.start (this->transport ());
  if (ret != 0)
    return ret;
  ret = _invocation.prepareHeader (1);
  if (ret != 0)
    return ret;
  OutputStream& __os = _invocation.outStream();
  __os.write_string(msg);
  __os.write_short(u);
  ret = _invocation.invoke();
  if (ret != 0)
  return "";
  InputStream& __is = _invocation.inpStream();
  ::std::string __ret;
  __is.read_long(v);
  __is.read_string(__ret);
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello_async(::demo::AMI_MyHello_sayHello* __cb, const ::std::string& msg, Short u)
{
  static const char* __operation("sayHello");
  Reference* ref = this->reference ();
  TwowayAsynchInvocation _invocation (ref, __operation, ref->communicator (), __cb);
  _invocation.start (this->transport ());
  _invocation.prepareHeader (1);
  OutputStream& __os = _invocation.outStream();
  __os.write_string(msg);
  __os.write_short(u);
  int  _invokeStatus = _invocation.invoke();
}

::std::string
IcmProxy::demo::MyHello::sayBye(const ::std::string& msg, Short u, Long& v)
{
  static const char* __operation("sayBye");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ret = _invocation.start (this->transport ());
  if (ret != 0)
    return ret;
  ret = _invocation.prepareHeader (1);
  if (ret != 0)
    return ret;
  OutputStream& __os = _invocation.outStream();
  __os.write_string(msg);
  __os.write_short(u);
  ret = _invocation.invoke();
  if (ret != 0)
  return "";
  InputStream& __is = _invocation.inpStream();
  ::std::string __ret;
  __is.read_long(v);
  __is.read_string(__ret);
  return __ret;
}

Long
IcmProxy::demo::MyHello::testHello(Long v1, Short v2, Float v3, Double& v4, ::std::string& v5)
{
  static const char* __operation("testHello");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ret = _invocation.start (this->transport ());
  if (ret != 0)
    return ret;
  ret = _invocation.prepareHeader (1);
  if (ret != 0)
    return ret;
  OutputStream& __os = _invocation.outStream();
  __os.write_long(v1);
  __os.write_short(v2);
  __os.write_float(v3);
  ret = _invocation.invoke();
  if (ret != 0)
    return ret;
  InputStream& __is = _invocation.inpStream();
  Long __ret;
  __is.read_double(v4);
  __is.read_string(v5);
  __is.read_long(__ret);
  return __ret;
}

void
IcmProxy::demo::MyHello::testHello_async(::demo::AMI_MyHello_testHello* __cb, Long v1, Short v2, Float v3)
{
  static const char* __operation("testHello");
  Reference* ref = this->reference ();
  TwowayAsynchInvocation _invocation (ref, __operation, ref->communicator (), __cb);
  _invocation.start (this->transport ());
  _invocation.prepareHeader (1);
  OutputStream& __os = _invocation.outStream();
  __os.write_long(v1);
  __os.write_short(v2);
  __os.write_float(v3);
  int  _invokeStatus = _invocation.invoke();
}

Long
IcmProxy::demo::MyHello::testSeq(const ::demo::MyStruct& cd, Long& lvalue)
{
  static const char* __operation("testSeq");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ret = _invocation.start (this->transport ());
  if (ret != 0)
    return ret;
  ret = _invocation.prepareHeader (1);
  if (ret != 0)
    return ret;
  OutputStream& __os = _invocation.outStream();
  cd.__write(__os);
  ret = _invocation.invoke();
  if (ret != 0)
    return ret;
  InputStream& __is = _invocation.inpStream();
  Long __ret;
  __is.read_long(lvalue);
  __is.read_long(__ret);
  return __ret;
}
