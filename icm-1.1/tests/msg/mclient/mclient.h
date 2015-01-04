
#ifndef __mclient_h__
#define __mclient_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace Icm
{

struct Port
{
  Int i;
  ::std::string str;

  bool operator==(const Port&) const;
  bool operator!=(const Port&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

struct Device
{
  ::std::string d;
  ::std::string a;

  bool operator==(const Device&) const;
  bool operator!=(const Device&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

typedef ::std::vector< ::Icm::Port> Ports;

class __U__Ports { };
void __write(OutputStream*, const ::Icm::Port*, const ::Icm::Port*, __U__Ports);
void __read(InputStream*, Ports&, __U__Ports);

struct Event1
{
  ::std::string topic;
  Short item1;
  ::std::string item2;
  ::Icm::Device dev;
  ::Icm::Ports conn;

  bool operator==(const Event1&) const;
  bool operator!=(const Event1&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

struct Event2
{
  ::std::string topic;
  Short item1;
  ::std::string item2;

  bool operator==(const Event2&) const;
  bool operator!=(const Event2&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
};

}

#endif
