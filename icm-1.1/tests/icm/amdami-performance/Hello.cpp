
#include <algorithm>
#include <sstream>
#include "Hello.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/Communicator.h>
#include <icc/Log.h>
#include <icc/Guard.h>

void
demo::__write(OutputStream* __os, IcmProxy::demo::MyHello* v)
{
  __os->write_obj(v);
}

void
demo::__read(InputStream* __is, IcmProxy::demo::MyHello* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::demo::MyHello;
    v->copyFrom(proxy);
  }
}

namespace demo
{
  template<typename T>
  std::string __toString(T t)
  {
    std::ostringstream oss;
    oss << t;
    return oss.str();
  }
  
  std::string& trimEnd(std::string& str)
  {
    while(isspace(str[str.size() - 1]))
    {
      str = str.substr(0, str.size()-1);
    }
    return str;
  }
}

void
demo::AMI_MyHello_sayHello::__response(InputStream* in, ACE_CDR::UInt replyStatus)
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

void
demo::AMI_MyHello_sayBye::__response(InputStream* in, ACE_CDR::UInt replyStatus)
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

static const ::std::string __demo__MyHello_ids[2] =
{
  "::Ice::Object",
  "::demo::MyHello"
};

DispatchStatus
demo::MyHello::___sayHello(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::std::string msg;
  Long u;
  __is->read_string(msg);
  __is->read_long(u);
  ::demo::AMH_MyHello_sayHello* __cb = new IcmAsync::demo::AMH_MyHello_sayHello(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello_async(__cb, msg, u);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___sayBye(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::std::string msg;
  __is->read_string(msg);
  ::demo::AMH_MyHello_sayBye* __cb = new IcmAsync::demo::AMH_MyHello_sayBye(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayBye_async(__cb, msg);
  return DispatchAsync;
}

static ::std::string __demo__MyHello_all[] =
{
  "sayBye",
  "sayHello"
};

DispatchStatus
demo::MyHello::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__MyHello_all, __demo__MyHello_all + 2, in.operation());
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
  }

  assert(false);
  return DispatchOperationNotExist;
}

::std::string
IcmProxy::demo::MyHello::sayHello(const ::std::string& msg, Long u, Long& v)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());
  int ok = _invocation.start (this->transport ());
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
    return "";
  }
  ok = _invocation.prepareHeader (1);
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
    return "";
  }
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  __os->write_long(u);
  ok = _invocation.invoke();
  if(ok != 0 && usingCachedTransport) 
  {
    ICC_ERROR("Cached transport error, retry a new connection!");
    this->transport(0);
    TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());
    ok = _invocation.start (this->transport ());
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
      return "";
    }
    ok = _invocation.prepareHeader (1);
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
      return "";
    }
    OutputStream* __os = _invocation.outStream();
    __os->write_string(msg);
    __os->write_long(u);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
    __is->read_long(v);
    __is->read_string(__ret);
    IcmProxy::setCallErrno( 0 );
    return __ret;
  }
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
    return "";
  }
  InputStream* __is = _invocation.inpStream();
  ::std::string __ret;
  __is->read_long(v);
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello_async(::demo::AMI_MyHello_sayHello* __cb, const ::std::string& msg, Long u)
{
  static const char* __operation("sayHello");
  Reference* ref = this->getReference ();
  TwowayAsynchInvocation _invocation (ref, __operation, ref->communicator (), __cb, ref->getMaxWaitTime());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
    return;
  }
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
    return;
  }
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  __os->write_long(u);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::sayBye(const ::std::string& msg, Long& v)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayBye");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());
  int ok = _invocation.start (this->transport ());
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
    return "";
  }
  ok = _invocation.prepareHeader (1);
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
    return "";
  }
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  ok = _invocation.invoke();
  if(ok != 0 && usingCachedTransport) 
  {
    ICC_ERROR("Cached transport error, retry a new connection!");
    this->transport(0);
    TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());
    ok = _invocation.start (this->transport ());
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
      return "";
    }
    ok = _invocation.prepareHeader (1);
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
      return "";
    }
    OutputStream* __os = _invocation.outStream();
    __os->write_string(msg);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
    __is->read_long(v);
    __is->read_string(__ret);
    IcmProxy::setCallErrno( 0 );
    return __ret;
  }
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
    return "";
  }
  InputStream* __is = _invocation.inpStream();
  ::std::string __ret;
  __is->read_long(v);
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayBye_async(::demo::AMI_MyHello_sayBye* __cb, const ::std::string& msg)
{
  static const char* __operation("sayBye");
  Reference* ref = this->getReference ();
  TwowayAsynchInvocation _invocation (ref, __operation, ref->communicator (), __cb, ref->getMaxWaitTime());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
    return;
  }
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
    return;
  }
  OutputStream* __os = _invocation.outStream();
  __os->write_string(msg);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

IcmAsync::demo::AMH_MyHello_sayHello::AMH_MyHello_sayHello(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello::response(const ::std::string& __ret, Long v)
{
  if(getConnectionError())
  {
    return;
  }
  this->initReply();
  this->mOutStream->write_long(v);
  this->mOutStream->write_string(__ret);
  this->sendReply();
}

IcmAsync::demo::AMH_MyHello_sayBye::AMH_MyHello_sayBye(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayBye::response(const ::std::string& __ret, Long v)
{
  if(getConnectionError())
  {
    return;
  }
  this->initReply();
  this->mOutStream->write_long(v);
  this->mOutStream->write_string(__ret);
  this->sendReply();
}
