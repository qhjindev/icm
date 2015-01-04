
#ifndef __javatopic_h__
#define __javatopic_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace JavaTopic
{

struct Request3
{
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  Int cpu;
  Int mem;

  bool operator==(const Request3&) const;
  bool operator!=(const Request3&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

struct Request4
{
  Int seq;
  ::std::string nodeId;
  ::std::string instanceId;
  ::std::string result;
  Int state;

  bool operator==(const Request4&) const;
  bool operator!=(const Request4&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

}

#endif
