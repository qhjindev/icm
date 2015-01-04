
#include <algorithm>
#include <sstream>
#include "Progress.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/Communicator.h>
#include <icc/Log.h>
#include <icc/Guard.h>

void
My::__write(OutputStream* __os, IcmProxy::My::MyCallback* v)
{
  __os->write_obj(v);
}

void
My::__read(InputStream* __is, IcmProxy::My::MyCallback* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::My::MyCallback;
    v->copyFrom(proxy);
  }
}

void
My::__write(OutputStream* __os, IcmProxy::My::VmService* v)
{
  __os->write_obj(v);
}

void
My::__read(InputStream* __is, IcmProxy::My::VmService* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::My::VmService;
    v->copyFrom(proxy);
  }
}

namespace My
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

My::OpProgress::OpProgress()
{
  seq = 0;
}

bool
My::OpProgress::operator==(const OpProgress& __rhs) const
{
  return !operator!=(__rhs);
}

bool
My::OpProgress::operator!=(const OpProgress& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(status != __rhs.status)
  {
    return true;
  }
  return false;
}

void
My::OpProgress::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(status);
}

void
My::OpProgress::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(status);
}

std::string
My::OpProgress::toString() const
{
  std::string result("My::OpProgress-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("status:" + __toString(this->status) + " ");
  return trimEnd(result);
}

std::string
My::__toString(const OpProgress& arg)
{
  return "[" + arg.toString() + "]";
}

static const ::std::string __My__MyCallback_ids[2] =
{
  "::Ice::Object",
  "::My::MyCallback"
};

DispatchStatus
My::MyCallback::___reportProgress(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  ::My::OpProgress progress;
  progress.__read(__is);
  reportProgress(progress);
  __inS.initReply ();
  return DispatchOK;
}

static ::std::string __My__MyCallback_all[] =
{
  "reportProgress"
};

DispatchStatus
My::MyCallback::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__My__MyCallback_all, __My__MyCallback_all + 1, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __My__MyCallback_all)
  {
    case 0:
    {
      return ___reportProgress(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

static const ::std::string __My__VmService_ids[2] =
{
  "::Ice::Object",
  "::My::VmService"
};

DispatchStatus
My::VmService::___setCallback(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::IcmProxy::My::MyCallback* cb;
  ::My::__read(__is, cb);
  Int __ret = setCallback(cb);
  __inS.initReply ();
  __os->write_int(__ret);
  return DispatchOK;
}

static ::std::string __My__VmService_all[] =
{
  "setCallback"
};

DispatchStatus
My::VmService::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__My__VmService_all, __My__VmService_all + 1, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __My__VmService_all)
  {
    case 0:
    {
      return ___setCallback(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

void
IcmProxy::My::MyCallback::reportProgress(const ::My::OpProgress& progress)
{
  ACE_GUARD(ThreadMutex, guard, mutex);
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("reportProgress");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());
  int ok = _invocation.start (this->transport ());
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
    return;
  }
  ok = _invocation.prepareHeader (1);
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
    return;
  }
  OutputStream* __os = _invocation.outStream();
  progress.__write(__os);
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
      return;
    }
    ok = _invocation.prepareHeader (1);
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
      return;
    }
    OutputStream* __os = _invocation.outStream();
    progress.__write(__os);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return;
    }
  }
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
    return;
  }
}

Int
IcmProxy::My::VmService::setCallback(IcmProxy::My::MyCallback * cb)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("setCallback");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator (), ref->getMaxWaitTime());
  int ok = _invocation.start (this->transport ());
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
    return -1;
  }
  ok = _invocation.prepareHeader (1);
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
    return -1;
  }
  OutputStream* __os = _invocation.outStream();
  ::My::__write(__os, cb);
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
      return -1;
    }
    ok = _invocation.prepareHeader (1);
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
      return -1;
    }
    OutputStream* __os = _invocation.outStream();
    ::My::__write(__os, cb);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return -1;
    }
    InputStream* __is = _invocation.inpStream();
    Int __ret;
    __is->read_int(__ret);
    IcmProxy::setCallErrno( 0 );
    return __ret;
  }
  if (ok != 0){
    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
    this->transport(0);
    return -1;
  }
  InputStream* __is = _invocation.inpStream();
  Int __ret;
  __is->read_int(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}
