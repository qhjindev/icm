
#include <algorithm>
#include <sstream>
#include "test.h"
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

std::string
demo::__toString(const Req1& arg)
{
  std::string result = "{";
  for(Req1::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

std::string
demo::__toString(const Req1I& arg)
{
  std::string result = "{";
  for(Req1I::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

std::string
demo::__toString(const Req2& arg)
{
  std::string result = "{";
  for(Req2::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += "<";
    result += __toString(it->first);
    result += ",";
    result += __toString(it->second);
    result += ">";
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
demo::__write(OutputStream* __os, const ::demo::Req2& v, ::demo::__U__Req2)
{
  __os->write_int(ACE_CDR::Int(v.size()));
  ::demo::Req2::const_iterator p;
  for(p = v.begin(); p != v.end(); ++p)
  {
    __os->write_int(p->first);
    __os->write_string(p->second);
  }
}

void
demo::__read(InputStream* __is, ::demo::Req2& v, ::demo::__U__Req2)
{
  ACE_CDR::Int sz;
  __is->read_int(sz);
  while(sz--)
  {
    ::std::pair<const Int, ::std::string> pair;
    __is->read_int(const_cast<Int&>(pair.first));
    ::demo::Req2::iterator __i = v.insert(v.end(), pair);
    __is->read_string(__i->second);
  }
}

demo::Req3::Req3()
{
  syn = 0;
}

bool
demo::Req3::operator==(const Req3& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::Req3::operator!=(const Req3& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(syn != __rhs.syn)
  {
    return true;
  }
  if(request1 != __rhs.request1)
  {
    return true;
  }
  return false;
}

void
demo::Req3::__write(OutputStream* __os) const
{
  __os->write_int(syn);
  __os->write_string_seq(request1);
}

void
demo::Req3::__read(InputStream* __is)
{
  __is->read_int(syn);
  __is->read_string_seq(request1);
}

std::string
demo::Req3::toString() const
{
  std::string result("demo::Req3-> ");
  result += ("syn:" + __toString(this->syn) + " ");
  result += ("request1:" + __toString(this->request1) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const Req3& arg)
{
  return "[" + arg.toString() + "]";
}

demo::Req4::Req4()
{
  syn = 0;
  syn2 = 0;
}

bool
demo::Req4::operator==(const Req4& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::Req4::operator!=(const Req4& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(syn != __rhs.syn)
  {
    return true;
  }
  if(request1 != __rhs.request1)
  {
    return true;
  }
  if(syn2 != __rhs.syn2)
  {
    return true;
  }
  return false;
}

void
demo::Req4::__write(OutputStream* __os) const
{
  __os->write_int(syn);
  __os->write_string_seq(request1);
  __os->write_int(syn2);
}

void
demo::Req4::__read(InputStream* __is)
{
  __is->read_int(syn);
  __is->read_string_seq(request1);
  __is->read_int(syn2);
}

std::string
demo::Req4::toString() const
{
  std::string result("demo::Req4-> ");
  result += ("syn:" + __toString(this->syn) + " ");
  result += ("request1:" + __toString(this->request1) + " ");
  result += ("syn2:" + __toString(this->syn2) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const Req4& arg)
{
  return "[" + arg.toString() + "]";
}

std::string
demo::__toString(const Req1R& arg)
{
  std::string result = "{";
  for(Req1R::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
demo::__write(OutputStream* __os, const ::demo::Req3* begin, const ::demo::Req3* end, ::demo::__U__Req1R)
{
  UInt size = static_cast< UInt>(end - begin);
  __os->write_uint(size);
  for(UInt i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
demo::__read(InputStream* __is, ::demo::Req1R& v, ::demo::__U__Req1R)
{
  UInt sz;
  __is->read_uint(sz);
  v.resize(sz);
  for(UInt i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

void
demo::AMI_MyHello_sayHello1::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_sayHello1I::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_sayHello1R::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_sayHello2::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_sayHello3::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_sayHello4::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

static const ::std::string __demo__MyHello_ids[2] =
{
  "::Ice::Object",
  "::demo::MyHello"
};

DispatchStatus
demo::MyHello::___sayHello1(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::Req1 req;
  __is->read_string_seq(req);
  ::demo::AMH_MyHello_sayHello1* __cb = new IcmAsync::demo::AMH_MyHello_sayHello1(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello1_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___sayHello1I(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::Req1I req;
  __is->read_int_seq(req);
  ::demo::AMH_MyHello_sayHello1I* __cb = new IcmAsync::demo::AMH_MyHello_sayHello1I(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello1I_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___sayHello1R(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::Req1R req;
  ::demo::__read(__is, req, ::demo::__U__Req1R());
  ::demo::AMH_MyHello_sayHello1R* __cb = new IcmAsync::demo::AMH_MyHello_sayHello1R(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello1R_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___sayHello2(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::Req2 req;
  ::demo::__read(__is, req, ::demo::__U__Req2());
  ::demo::AMH_MyHello_sayHello2* __cb = new IcmAsync::demo::AMH_MyHello_sayHello2(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello2_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___sayHello3(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::Req3 req;
  req.__read(__is);
  ::demo::AMH_MyHello_sayHello3* __cb = new IcmAsync::demo::AMH_MyHello_sayHello3(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello3_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___sayHello4(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::Req4 req;
  req.__read(__is);
  ::demo::AMH_MyHello_sayHello4* __cb = new IcmAsync::demo::AMH_MyHello_sayHello4(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello4_async(__cb, req);
  return DispatchAsync;
}

static ::std::string __demo__MyHello_all[] =
{
  "sayHello1",
  "sayHello1I",
  "sayHello1R",
  "sayHello2",
  "sayHello3",
  "sayHello4"
};

DispatchStatus
demo::MyHello::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__MyHello_all, __demo__MyHello_all + 6, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __demo__MyHello_all)
  {
    case 0:
    {
      return ___sayHello1(in);
    }
    case 1:
    {
      return ___sayHello1I(in);
    }
    case 2:
    {
      return ___sayHello1R(in);
    }
    case 3:
    {
      return ___sayHello2(in);
    }
    case 4:
    {
      return ___sayHello3(in);
    }
    case 5:
    {
      return ___sayHello4(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

::std::string
IcmProxy::demo::MyHello::sayHello1(const ::demo::Req1& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello1");
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
  __os->write_string_seq(req);
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
    __os->write_string_seq(req);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
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
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello1_async(::demo::AMI_MyHello_sayHello1* __cb, const ::demo::Req1& req)
{
  static const char* __operation("sayHello1");
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
  __os->write_string_seq(req);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::sayHello1I(const ::demo::Req1I& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello1I");
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
  __os->write_int_seq(req);
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
    __os->write_int_seq(req);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
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
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello1I_async(::demo::AMI_MyHello_sayHello1I* __cb, const ::demo::Req1I& req)
{
  static const char* __operation("sayHello1I");
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
  __os->write_int_seq(req);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::sayHello1R(const ::demo::Req1R& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello1R");
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
  if(req.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::demo::__write(__os, &req[0], &req[0] + req.size(), ::demo::__U__Req1R());
  }
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
    if(req.size() == 0)
    {
      __os->write_uint(0);
    }
    else
    {
      ::demo::__write(__os, &req[0], &req[0] + req.size(), ::demo::__U__Req1R());
    }
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
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
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello1R_async(::demo::AMI_MyHello_sayHello1R* __cb, const ::demo::Req1R& req)
{
  static const char* __operation("sayHello1R");
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
  if(req.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::demo::__write(__os, &req[0], &req[0] + req.size(), ::demo::__U__Req1R());
  }
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::sayHello2(const ::demo::Req2& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello2");
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
  ::demo::__write(__os, req, ::demo::__U__Req2());
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
    ::demo::__write(__os, req, ::demo::__U__Req2());
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
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
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello2_async(::demo::AMI_MyHello_sayHello2* __cb, const ::demo::Req2& req)
{
  static const char* __operation("sayHello2");
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
  ::demo::__write(__os, req, ::demo::__U__Req2());
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::sayHello3(const ::demo::Req3& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello3");
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
  req.__write(__os);
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
    req.__write(__os);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
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
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello3_async(::demo::AMI_MyHello_sayHello3* __cb, const ::demo::Req3& req)
{
  static const char* __operation("sayHello3");
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
  req.__write(__os);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::sayHello4(const ::demo::Req4& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("sayHello4");
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
  req.__write(__os);
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
    req.__write(__os);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return "";
    }
    InputStream* __is = _invocation.inpStream();
    ::std::string __ret;
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
  __is->read_string(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::demo::MyHello::sayHello4_async(::demo::AMI_MyHello_sayHello4* __cb, const ::demo::Req4& req)
{
  static const char* __operation("sayHello4");
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
  req.__write(__os);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

IcmAsync::demo::AMH_MyHello_sayHello1::AMH_MyHello_sayHello1(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello1::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    delete this;
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
  delete this;
}

IcmAsync::demo::AMH_MyHello_sayHello1I::AMH_MyHello_sayHello1I(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello1I::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    delete this;
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
  delete this;
}

IcmAsync::demo::AMH_MyHello_sayHello1R::AMH_MyHello_sayHello1R(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello1R::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    delete this;
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
  delete this;
}

IcmAsync::demo::AMH_MyHello_sayHello2::AMH_MyHello_sayHello2(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello2::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    delete this;
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
  delete this;
}

IcmAsync::demo::AMH_MyHello_sayHello3::AMH_MyHello_sayHello3(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello3::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    delete this;
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
  delete this;
}

IcmAsync::demo::AMH_MyHello_sayHello4::AMH_MyHello_sayHello4(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_sayHello4::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    delete this;
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
  delete this;
}
