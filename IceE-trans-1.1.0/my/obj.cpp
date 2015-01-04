
#include <algorithm>
#include <string>
#include "obj.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

::IcmProxy::Object*
IcmProxy::Test::B::getObj()
{
  static const char* __operation("getObj");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
    return 0;
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
    return 0;
  ok = _invocation.invoke();
  if (ok != 0)
    return 0;
  InputStream& __is = _invocation.inpStream();
  ::IcmProxy::Object* __ret;
  __is->read_obj(__ret);
  return __ret;
}

Int
IcmProxy::Test::B::setObj(IcmProxy::Object* obj)
{
  static const char* __operation("setObj");
  Reference* ref = this->reference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
    return -1;
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
    return -1;
  OutputStream& __os = _invocation.outStream();
  __os->write_obj(obj);
  ok = _invocation.invoke();
  if (ok != 0)
    return 0;
  InputStream& __is = _invocation.inpStream();
  Int __ret;
  __is.read_int(__ret);
  return __ret;
}

static const ::std::string __Test__B_ids[2] =
{
  "::Ice::Object",
  "::Test::B"
};

DispatchStatus
Test::B::___getObj(ServerRequest& __inS)
{
  OutputStream& __os = __inS.outgoing();
  ::IcmProxy::Object* __ret = getObj();
  __inS.initReply ();
  __os->write_obj(__ret);
  return DispatchOK;
}

DispatchStatus
Test::B::___setObj(ServerRequest& __inS)
{
  InputStream& __is = __inS.incoming();
  OutputStream& __os = __inS.outgoing();
  ::IcmProxy::Object* obj;
  __is->read_obj(obj);
  Int __ret = setObj(obj);
  __inS.initReply ();
  __os.write_int(__ret);
  return DispatchOK;
}

static ::std::string __Test__B_all[] =
{
  "getObj",
  "setObj"
};

DispatchStatus
Test::B::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__Test__B_all, __Test__B_all + 2, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __Test__B_all)
  {
    case 0:
    {
      return ___getObj(in);
    }
    case 1:
    {
      return ___setObj(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

void
Test::__write(OutputStream* __os, IcmProxy::Test::B* v)
{
  __os->write_obj(v);
}

void
Test::__read(InputStream* __is, IcmProxy::Test::B* v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::Test::B;
    v->copyFrom(proxy);
  }
}
