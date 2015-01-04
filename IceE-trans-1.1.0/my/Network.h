
#ifndef __Network_h__
#define __Network_h__

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

}

}

namespace demo
{

void __write(OutputStream*, IcmProxy::demo::Network* v);
void __read(InputStream*, IcmProxy::demo::Network* &v);

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

}

}

#endif
