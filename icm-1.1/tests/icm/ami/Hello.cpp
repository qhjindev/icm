
#include <algorithm>
#include <string>
#include "Hello.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

void
demo::__write(OutputStream* __os, IcmProxy::demo::Hello* v)
{
  __os->write_obj(v);
}

void
demo::__read(InputStream* __is, IcmProxy::demo::Hello* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::demo::Hello;
    v->copyFrom(proxy);
  }
}

static const ::std::string __demo__Hello_ids[2] =
{
  "::Ice::Object",
  "::demo::Hello"
};

DispatchStatus
demo::Hello::___sayHello(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::std::string msg;
  Short u;
  __is->read_string(msg);
  __is->read_short(u);
  Long v;
  ::std::string __ret = sayHello(msg, u, v);
  __inS.initReply ();
  __os->write_long(v);
  __os->write_string(__ret);
  return DispatchOK;
}

DispatchStatus
demo::Hello::___sayBye(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::std::string msg;
  Short u;
  __is->read_string(msg);
  __is->read_short(u);
  Long v;
  ::std::string __ret = sayBye(msg, u, v);
  __inS.initReply ();
  __os->write_long(v);
  __os->write_string(__ret);
  return DispatchOK;
}

static ::std::string __demo__Hello_all[] =
{
  "sayBye",
  "sayHello"
};

DispatchStatus
demo::Hello::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__Hello_all, __demo__Hello_all + 2, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __demo__Hello_all)
  {
    case 0:
    {
      return ___sayBye(in);
    }
    case 1:
    {
      return ___sayHello(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

void
demo::AMI_Hello_sayBye::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    Long v;
    ::std::string __ret;
    in->read_long(v);
    in->read_string(__ret);
    this->response(__ret, v);
  }
}

::std::string
IcmProxy::demo::Hello::sayHello(const ::std::string& msg, Short u, Long& v)
{
  static const char* __operation("sayHello");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
    return "";
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
    return "";
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  __os->write_short(u);
  ok = _invocation.invoke();
  if (ok != 0)
    return "";
  InputStream* __is = _invocation.inpStream();
  ::std::string __ret;
  __is->read_long(v);
  __is->read_string(__ret);
  return __ret;
}

::std::string
IcmProxy::demo::Hello::sayBye(const ::std::string& msg, Short u, Long& v)
{
  static const char* __operation("sayBye");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
    return "";
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
    return "";
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  __os->write_short(u);
  ok = _invocation.invoke();
  if (ok != 0)
    return "";
  InputStream* __is = _invocation.inpStream();
  ::std::string __ret;
  __is->read_long(v);
  __is->read_string(__ret);
  return __ret;
}

void
IcmProxy::demo::Hello::sayBye_async(::demo::AMI_Hello_sayBye* __cb, const ::std::string& msg, Short u)
{
  static const char* __operation("sayBye");
  Reference* ref = this->getReference ();
  TwowayAsynchInvocation _invocation (ref, __operation, ref->communicator (), __cb);
  _invocation.start (this->transport ());
  _invocation.prepareHeader (1);
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  __os->write_short(u);
  int  _invokeStatus = _invocation.invoke();
}
