
#include <algorithm>
#include <sstream>
#include "MsgBroker.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/Communicator.h>
#include <icc/Log.h>
#include <icc/Guard.h>

void
MsgBroker::__write(OutputStream* __os, IcmProxy::MsgBroker::QueryState* v)
{
  __os->write_obj(v);
}

void
MsgBroker::__read(InputStream* __is, IcmProxy::MsgBroker::QueryState* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::MsgBroker::QueryState;
    v->copyFrom(proxy);
  }
}

namespace MsgBroker
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
MsgBroker::__toString(const Topics& arg)
{
  std::string result = "{";
  for(Topics::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

MsgBroker::QueryStateReq::QueryStateReq()
{
}

bool
MsgBroker::QueryStateReq::operator==(const QueryStateReq& __rhs) const
{
  return !operator!=(__rhs);
}

bool
MsgBroker::QueryStateReq::operator!=(const QueryStateReq& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(ts != __rhs.ts)
  {
    return true;
  }
  return false;
}

void
MsgBroker::QueryStateReq::__write(OutputStream* __os) const
{
  __os->write_string_seq(ts);
}

void
MsgBroker::QueryStateReq::__read(InputStream* __is)
{
  __is->read_string_seq(ts);
}

std::string
MsgBroker::QueryStateReq::toString() const
{
  std::string result("MsgBroker::QueryStateReq-> ");
  result += ("ts:" + __toString(this->ts) + " ");
  return trimEnd(result);
}

std::string
MsgBroker::__toString(const QueryStateReq& arg)
{
  return "[" + arg.toString() + "]";
}

MsgBroker::State::State()
{
  s = false;
}

bool
MsgBroker::State::operator==(const State& __rhs) const
{
  return !operator!=(__rhs);
}

bool
MsgBroker::State::operator!=(const State& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(topic != __rhs.topic)
  {
    return true;
  }
  if(s != __rhs.s)
  {
    return true;
  }
  return false;
}

void
MsgBroker::State::__write(OutputStream* __os) const
{
  __os->write_string(topic);
  __os->write_boolean((ACE_CDR::Boolean&)s);
}

void
MsgBroker::State::__read(InputStream* __is)
{
  __is->read_string(topic);
  __is->read_boolean((ACE_CDR::Boolean&)s);
}

std::string
MsgBroker::State::toString() const
{
  std::string result("MsgBroker::State-> ");
  result += ("topic:" + __toString(this->topic) + " ");
  result += ("s:" + __toString(this->s) + " ");
  return trimEnd(result);
}

std::string
MsgBroker::__toString(const State& arg)
{
  return "[" + arg.toString() + "]";
}

std::string
MsgBroker::__toString(const States& arg)
{
  std::string result = "{";
  for(States::const_iterator it = arg.begin(); it != arg.end(); ++it)
  {
    result += __toString(*it);
    result += " ";
  }
  trimEnd(result);
  result += "}";
  return result;
}

void
MsgBroker::__write(OutputStream* __os, const ::MsgBroker::State* begin, const ::MsgBroker::State* end, ::MsgBroker::__U__States)
{
  UInt size = static_cast< UInt>(end - begin);
  __os->write_uint(size);
  for(UInt i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
MsgBroker::__read(InputStream* __is, ::MsgBroker::States& v, ::MsgBroker::__U__States)
{
  UInt sz;
  __is->read_uint(sz);
  v.resize(sz);
  for(UInt i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

MsgBroker::QueryStateRes::QueryStateRes()
{
}

bool
MsgBroker::QueryStateRes::operator==(const QueryStateRes& __rhs) const
{
  return !operator!=(__rhs);
}

bool
MsgBroker::QueryStateRes::operator!=(const QueryStateRes& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(ss != __rhs.ss)
  {
    return true;
  }
  return false;
}

void
MsgBroker::QueryStateRes::__write(OutputStream* __os) const
{
  if(ss.size() == 0)
  {
    __os->write_uint(0);
  }
  else
  {
    ::MsgBroker::__write(__os, &ss[0], &ss[0] + ss.size(), ::MsgBroker::__U__States());
  }
}

void
MsgBroker::QueryStateRes::__read(InputStream* __is)
{
  ::MsgBroker::__read(__is, ss, ::MsgBroker::__U__States());
}

std::string
MsgBroker::QueryStateRes::toString() const
{
  std::string result("MsgBroker::QueryStateRes-> ");
  result += ("ss:" + __toString(this->ss) + " ");
  return trimEnd(result);
}

std::string
MsgBroker::__toString(const QueryStateRes& arg)
{
  return "[" + arg.toString() + "]";
}

void
MsgBroker::AMI_QueryState_queryStates::__response(InputStream* in, ACE_CDR::UInt replyStatus)
{
  if (replyStatus == 0)
  {
    ::MsgBroker::QueryStateRes res;
    Int __ret;
    res.__read(in);
    in->read_int(__ret);
    this->response(__ret, res);
  }
}

static const ::std::string __MsgBroker__QueryState_ids[2] =
{
  "::Ice::Object",
  "::MsgBroker::QueryState"
};

DispatchStatus
MsgBroker::QueryState::___queryStates(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  OutputStream* __os = __inS.outgoing();
  ::MsgBroker::QueryStateReq req;
  req.__read(__is);
  ::MsgBroker::QueryStateRes res;
  Int __ret = queryStates(req, res);
  __inS.initReply ();
  res.__write(__os);
  __os->write_int(__ret);
  return DispatchOK;
}

static ::std::string __MsgBroker__QueryState_all[] =
{
  "queryStates"
};

DispatchStatus
MsgBroker::QueryState::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__MsgBroker__QueryState_all, __MsgBroker__QueryState_all + 1, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __MsgBroker__QueryState_all)
  {
    case 0:
    {
      return ___queryStates(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

Int
IcmProxy::MsgBroker::QueryState::queryStates(const ::MsgBroker::QueryStateReq& req, ::MsgBroker::QueryStateRes& res)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);
  bool usingCachedTransport = (this->transport() != 0);
  static const char* __operation("queryStates");
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
      return -1;
    }
    ok = _invocation.prepareHeader (1);
    if (ok != 0) 
    {
      IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
      return -1;
    }
    OutputStream* __os = _invocation.outStream();
    req.__write(__os);
    ok = _invocation.invoke();
    if (ok != 0){
      IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
      this->transport(0);
      return -1;
    }
    InputStream* __is = _invocation.inpStream();
    Int __ret;
    res.__read(__is);
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
  res.__read(__is);
  __is->read_int(__ret);
  IcmProxy::setCallErrno( 0 );
  return __ret;
}

void
IcmProxy::MsgBroker::QueryState::queryStates_async(::MsgBroker::AMI_QueryState_queryStates* __cb, const ::MsgBroker::QueryStateReq& req)
{
  static const char* __operation("queryStates");
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
