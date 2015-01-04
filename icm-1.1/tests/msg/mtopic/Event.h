
#ifndef __Event_h__
#define __Event_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace IcmProxy
{

namespace demo
{

class Network;

class Alarm;

}

}

namespace demo
{

void __write(OutputStream*, IcmProxy::demo::Network* v);
void __read(InputStream*, IcmProxy::demo::Network* &v);

void __write(OutputStream*, IcmProxy::demo::Alarm* v);
void __read(InputStream*, IcmProxy::demo::Alarm* &v);

}

namespace demo
{

struct NetEvent
{
  ::std::string ip;
  Short port;
  ::std::string event;

  bool operator==(const NetEvent&) const;
  bool operator!=(const NetEvent&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

struct AlarmEvent
{
  ::std::string id;
  ::std::string image;
  Int mds;

  bool operator==(const AlarmEvent&) const;
  bool operator!=(const AlarmEvent&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

}

namespace demo
{

class Network : virtual public Object
{
public:

  Network() {}

  virtual void reportEvent(const ::demo::NetEvent&) = 0;
  DispatchStatus ___reportEvent(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

class Alarm : virtual public Object
{
public:

  Alarm() {}

  virtual void reportAlarm(const ::demo::AlarmEvent&) = 0;
  DispatchStatus ___reportAlarm(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace demo
{

class Network : virtual public IcmProxy::Object
{
public:

  void reportEvent(const ::demo::NetEvent& event);
};

class Alarm : virtual public IcmProxy::Object
{
public:

  void reportAlarm(const ::demo::AlarmEvent& event);
};

}

}

#endif
