
#ifndef __cpptopic_h__
#define __cpptopic_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace CppTopic
{

struct Request1
{
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  Int cpu;
  Int mem;

  bool operator==(const Request1&) const;
  bool operator!=(const Request1&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

struct Request2
{
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  ::std::string result;
  Int state;

  bool operator==(const Request2&) const;
  bool operator!=(const Request2&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

}

#endif
