
#ifndef __MsgIdl_h__
#define __MsgIdl_h__

#include <string>
#include <vector>
#include <list>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace demo
{

struct NetEvent
{
  NetEvent();
  ::std::string ip;
  Short port;
  ::std::string event;

  bool operator==(const NetEvent&) const;
  bool operator!=(const NetEvent&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const NetEvent&);

struct MsgEvent
{
  MsgEvent();
  ::std::string seq;
  ::std::string status;

  bool operator==(const MsgEvent&) const;
  bool operator!=(const MsgEvent&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const MsgEvent&);

}

#endif
