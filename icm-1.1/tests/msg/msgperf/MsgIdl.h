
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

struct MsgReq
{
  MsgReq();
  Long seq;
  ::std::string ip;
  Short port;
  ::std::string event;

  bool operator==(const MsgReq&) const;
  bool operator!=(const MsgReq&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const MsgReq&);

struct MsgRes
{
  MsgRes();
  Long seq;
  ::std::string result;
  ::std::string status;

  bool operator==(const MsgRes&) const;
  bool operator!=(const MsgRes&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const MsgRes&);

}

#endif
