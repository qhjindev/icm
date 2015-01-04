
#include <algorithm>
#include <string>
#include "Event.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

void
demo::__write(OutputStream* __os, IcmProxy::demo::Network* v)
{
  __os->write_obj(v);
}

void
demo::__read(InputStream* __is, IcmProxy::demo::Network* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::demo::Network;
    v->copyFrom(proxy);
  }
}

void
demo::__write(OutputStream* __os, IcmProxy::demo::Alarm* v)
{
  __os->write_obj(v);
}

void
demo::__read(InputStream* __is, IcmProxy::demo::Alarm* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::demo::Alarm;
    v->copyFrom(proxy);
  }
}

bool
demo::NetEvent::operator==(const NetEvent& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::NetEvent::operator!=(const NetEvent& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(ip != __rhs.ip)
  {
    return true;
  }
  if(port != __rhs.port)
  {
    return true;
  }
  if(event != __rhs.event)
  {
    return true;
  }
  return false;
}

void
demo::NetEvent::__write(OutputStream* __os) const
{
  __os->write_string(ip);
  __os->write_short(port);
  __os->write_string(event);
}

void
demo::NetEvent::__read(InputStream* __is)
{
  __is->read_string(ip);
  __is->read_short(port);
  __is->read_string(event);
}

bool
demo::AlarmEvent::operator==(const AlarmEvent& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::AlarmEvent::operator!=(const AlarmEvent& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(id != __rhs.id)
  {
    return true;
  }
  if(image != __rhs.image)
  {
    return true;
  }
  if(mds != __rhs.mds)
  {
    return true;
  }
  return false;
}

void
demo::AlarmEvent::__write(OutputStream* __os) const
{
  __os->write_string(id);
  __os->write_string(image);
  __os->write_int(mds);
}

void
demo::AlarmEvent::__read(InputStream* __is)
{
  __is->read_string(id);
  __is->read_string(image);
  __is->read_int(mds);
}

static const ::std::string __demo__Network_ids[2] =
{
  "::Ice::Object",
  "::demo::Network"
};

DispatchStatus
demo::Network::___reportEvent(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  ::demo::NetEvent event;
  event.__read(__is);
  reportEvent(event);
  __inS.initReply ();
  return DispatchOK;
}

static ::std::string __demo__Network_all[] =
{
  "reportEvent"
};

DispatchStatus
demo::Network::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__Network_all, __demo__Network_all + 1, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __demo__Network_all)
  {
    case 0:
    {
      return ___reportEvent(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

static const ::std::string __demo__Alarm_ids[2] =
{
  "::Ice::Object",
  "::demo::Alarm"
};

DispatchStatus
demo::Alarm::___reportAlarm(ServerRequest& __inS)
{
  InputStream* __is = __inS.incoming();
  ::demo::AlarmEvent event;
  event.__read(__is);
  reportAlarm(event);
  __inS.initReply ();
  return DispatchOK;
}

static ::std::string __demo__Alarm_all[] =
{
  "reportAlarm"
};

DispatchStatus
demo::Alarm::__dispatch(ServerRequest& in)
{
  ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__demo__Alarm_all, __demo__Alarm_all + 1, in.operation());
  if(r.first == r.second)
  {
    return DispatchOperationNotExist;
  }

  switch(r.first - __demo__Alarm_all)
  {
    case 0:
    {
      return ___reportAlarm(in);
    }
  }

  assert(false);
  return DispatchOperationNotExist;
}

void
IcmProxy::demo::Network::reportEvent(const ::demo::NetEvent& event)
{
  static const char* __operation("reportEvent");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
    return;
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
    return;
  OutputStream* __os = _invocation.outStream();
  event.__write(__os);
  ok = _invocation.invoke();
  if (ok != 0)
    return;
}

void
IcmProxy::demo::Alarm::reportAlarm(const ::demo::AlarmEvent& event)
{
  static const char* __operation("reportAlarm");
  Reference* ref = this->getReference ();
  TwowayInvocation _invocation (ref, __operation, ref->communicator ());
  int ok = _invocation.start (this->transport ());
  if (ok != 0)
    return;
  ok = _invocation.prepareHeader (1);
  if (ok != 0)
    return;
  OutputStream* __os = _invocation.outStream();
  event.__write(__os);
  ok = _invocation.invoke();
  if (ok != 0)
    return;
}
