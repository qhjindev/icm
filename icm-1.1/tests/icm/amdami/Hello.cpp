
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

bool
demo::S1::operator==(const S1& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::S1::operator!=(const S1& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(id != __rhs.id)
  {
    return true;
  }
  if(name != __rhs.name)
  {
    return true;
  }
  return false;
}

void
demo::S1::__write(OutputStream* __os) const
{
  __os->write_int(id);
  __os->write_string(name);
}

void
demo::S1::__read(InputStream* __is)
{
  __is->read_int(id);
  __is->read_string(name);
}

std::string
demo::S1::toString() const
{
  std::string result;
  result += ("id:" + __toString(id) + " ");
  result += ("name:" + __toString(name) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const S1& arg)
{
  return "[" + arg.toString() + "]";
}

std::string
demo::__toString(const Reqs& arg)
{
  std::string result = "{";
  for(Reqs::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
demo::__write(OutputStream* __os, const ::demo::S1* begin, const ::demo::S1* end, ::demo::__U__Reqs)
{
  UInt size = static_cast< UInt>(end - begin);
  __os->write_uint(size);
  for(int i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
demo::__read(InputStream* __is, ::demo::Reqs& v, ::demo::__U__Reqs)
{
  UInt sz;
  __is->read_uint(sz);
  v.resize(sz);
  for(int i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

std::string
demo::__toString(const Dict& arg)
{
  std::string result = "{";
  for(Dict::const_iterator it = arg.begin(); it != arg.end(); ++it)
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
demo::__write(OutputStream* __os, const ::demo::Dict& v, ::demo::__U__Dict)
{
  __os->write_int(ACE_CDR::Int(v.size()));
  ::demo::Dict::const_iterator p;
  for(p = v.begin(); p != v.end(); ++p)
  {
    __os->write_short(p->first);
    p->second.__write(__os);
  }
}

void
demo::__read(InputStream* __is, ::demo::Dict& v, ::demo::__U__Dict)
{
  ACE_CDR::Int sz;
  __is->read_int(sz);
  while(sz--)
  {
    ::std::pair<const Short, ::demo::S1> pair;
    __is->read_short(const_cast<Short&>(pair.first));
    ::demo::Dict::iterator __i = v.insert(v.end(), pair);
    __i->second.__read(__is);
  }
}

std::string
demo::__toString(const Dicta& arg)
{
  std::string result = "{";
  for(Dicta::const_iterator it = arg.begin(); it != arg.end(); ++it)
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
demo::__write(OutputStream* __os, const ::demo::Dicta& v, ::demo::__U__Dicta)
{
  __os->write_int(ACE_CDR::Int(v.size()));
  ::demo::Dicta::const_iterator p;
  for(p = v.begin(); p != v.end(); ++p)
  {
    __os->write_string(p->first);
    if(p->second.size() == 0)
    {
      __os->write_uint(0);
    }
    else
    {
      ::demo::__write(__os, &p->second[0], &p->second[0] + p->second.size(), ::demo::__U__Reqs());
    }
  }
}

void
demo::__read(InputStream* __is, ::demo::Dicta& v, ::demo::__U__Dicta)
{
  ACE_CDR::Int sz;
  __is->read_int(sz);
  while(sz--)
  {
    ::std::pair<const  ::std::string, ::demo::Reqs> pair;
    __is->read_string(const_cast< ::std::string&>(pair.first));
    ::demo::Dicta::iterator __i = v.insert(v.end(), pair);
    ::demo::__read(__is, __i->second, ::demo::__U__Reqs());
  }
}

std::string
demo::__toString(const Com& arg)
{
  std::string result = "{";
  for(Com::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
demo::__write(OutputStream* __os, const ::demo::Dicta* begin, const ::demo::Dicta* end, ::demo::__U__Com)
{
  UInt size = static_cast< UInt>(end - begin);
  __os->write_uint(size);
  for(int i = 0; i < size; ++i)
  {
    ::demo::__write(__os, begin[i], ::demo::__U__Dicta());
  }
}

void
demo::__read(InputStream* __is, ::demo::Com& v, ::demo::__U__Com)
{
  UInt sz;
  __is->read_uint(sz);
  v.resize(sz);
  for(int i = 0; i < sz; ++i)
  {
    ::demo::__read(__is, v[i], ::demo::__U__Dicta());
  }
}

bool
demo::TestReq::operator==(const TestReq& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::TestReq::operator!=(const TestReq& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(syn != __rhs.syn)
  {
    return true;
  }
  if(reqs1 != __rhs.reqs1)
  {
    return true;
  }
  return false;
}

void
demo::TestReq::__write(OutputStream* __os) const
{
  __os->write_int(syn);
  if(reqs1.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::demo::__write(__os, &reqs1[0], &reqs1[0] + reqs1.size(), ::demo::__U__Reqs());
  }
}

void
demo::TestReq::__read(InputStream* __is)
{
  __is->read_int(syn);
  ::demo::__read(__is, reqs1, ::demo::__U__Reqs());
}

std::string
demo::TestReq::toString() const
{
  std::string result;
  result += ("syn:" + __toString(syn) + " ");
  result += ("reqs1:" + __toString(reqs1) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const TestReq& arg)
{
  return "[" + arg.toString() + "]";
}

bool
demo::TestReqDict::operator==(const TestReqDict& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::TestReqDict::operator!=(const TestReqDict& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(syn != __rhs.syn)
  {
    return true;
  }
  if(dict1 != __rhs.dict1)
  {
    return true;
  }
  return false;
}

void
demo::TestReqDict::__write(OutputStream* __os) const
{
  __os->write_int(syn);
  ::demo::__write(__os, dict1, ::demo::__U__Dict());
}

void
demo::TestReqDict::__read(InputStream* __is)
{
  __is->read_int(syn);
  ::demo::__read(__is, dict1, ::demo::__U__Dict());
}

std::string
demo::TestReqDict::toString() const
{
  std::string result;
  result += ("syn:" + __toString(syn) + " ");
  result += ("dict1:" + __toString(dict1) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const TestReqDict& arg)
{
  return "[" + arg.toString() + "]";
}

bool
demo::TestReqCom::operator==(const TestReqCom& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::TestReqCom::operator!=(const TestReqCom& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(syn != __rhs.syn)
  {
    return true;
  }
  if(com1 != __rhs.com1)
  {
    return true;
  }
  return false;
}

void
demo::TestReqCom::__write(OutputStream* __os) const
{
  __os->write_int(syn);
  if(com1.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::demo::__write(__os, &com1[0], &com1[0] + com1.size(), ::demo::__U__Com());
  }
}

void
demo::TestReqCom::__read(InputStream* __is)
{
  __is->read_int(syn);
  ::demo::__read(__is, com1, ::demo::__U__Com());
}

std::string
demo::TestReqCom::toString() const
{
  std::string result;
  result += ("syn:" + __toString(syn) + " ");
  result += ("com1:" + __toString(com1) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const TestReqCom& arg)
{
  return "[" + arg.toString() + "]";
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
demo::AMI_MyHello_testSequence::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_testDictionary::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::std::string __ret;
    in->read_string(__ret);
    this->response(__ret);
  }
}

void
demo::AMI_MyHello_testCompound::__response(InputStream* in, ACE_CDR::UInt replyStatus)
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
demo::MyHello::___sayHello(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::std::string msg;
  Short u;
  __is->read_string(msg);
  __is->read_short(u);
  ::demo::AMH_MyHello_sayHello* __cb = new IcmAsync::demo::AMH_MyHello_sayHello(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  sayHello_async(__cb, msg, u);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___testSequence(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::TestReq req;
  req.__read(__is);
  ::demo::AMH_MyHello_testSequence* __cb = new IcmAsync::demo::AMH_MyHello_testSequence(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  testSequence_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___testDictionary(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::TestReqDict req;
  req.__read(__is);
  ::demo::AMH_MyHello_testDictionary* __cb = new IcmAsync::demo::AMH_MyHello_testDictionary(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  testDictionary_async(__cb, req);
  return DispatchAsync;
}

DispatchStatus
demo::MyHello::___testCompound(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::demo::TestReqCom req;
  req.__read(__is);
  ::demo::AMH_MyHello_testCompound* __cb = new IcmAsync::demo::AMH_MyHello_testCompound(__inS);
  __inS.transport()->setResponsHandler(dynamic_cast<AmhResponseHandler*>(__cb));
  testCompound_async(__cb, req);
  return DispatchAsync;
}

static ::std::string __demo__MyHello_all[] =
{
  "sayHello",
  "testCompound",
  "testDictionary",
  "testSequence"
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
      return ___sayHello(in);
    }
    case 1:
    {
      return ___testCompound(in);
    }
    case 2:
    {
      return ___testDictionary(in);
    }
    case 3:
    {
      return ___testSequence(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

::std::string
IcmProxy::demo::MyHello::sayHello(const ::std::string& msg, Short u, Long& v)
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
  __os->write_short(u);
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
    __os->write_short(u);
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
IcmProxy::demo::MyHello::sayHello_async(::demo::AMI_MyHello_sayHello* __cb, const ::std::string& msg, Short u)
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
  __os->write_short(u);
  int  _invokeStatus = _invocation.invoke();
  if(_invokeStatus != 0)
  {
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
  }
}

::std::string
IcmProxy::demo::MyHello::testSequence(const ::demo::TestReq& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("testSequence");
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
IcmProxy::demo::MyHello::testSequence_async(::demo::AMI_MyHello_testSequence* __cb, const ::demo::TestReq& req)
{
  static const char* __operation("testSequence");
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
IcmProxy::demo::MyHello::testDictionary(const ::demo::TestReqDict& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("testDictionary");
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
IcmProxy::demo::MyHello::testDictionary_async(::demo::AMI_MyHello_testDictionary* __cb, const ::demo::TestReqDict& req)
{
  static const char* __operation("testDictionary");
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
IcmProxy::demo::MyHello::testCompound(const ::demo::TestReqCom& req)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, "");
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("testCompound");
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
IcmProxy::demo::MyHello::testCompound_async(::demo::AMI_MyHello_testCompound* __cb, const ::demo::TestReqCom& req)
{
  static const char* __operation("testCompound");
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

IcmAsync::demo::AMH_MyHello_testSequence::AMH_MyHello_testSequence(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_testSequence::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
}

IcmAsync::demo::AMH_MyHello_testDictionary::AMH_MyHello_testDictionary(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_testDictionary::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
}

IcmAsync::demo::AMH_MyHello_testCompound::AMH_MyHello_testCompound(ServerRequest& serverRequest) :
  AmhResponseHandler (serverRequest)
{
}

void
IcmAsync::demo::AMH_MyHello_testCompound::response(const ::std::string& __ret)
{
  if(getConnectionError())
  {
    return;
  }
  this->initReply();
  this->mOutStream->write_string(__ret);
  this->sendReply();
}
