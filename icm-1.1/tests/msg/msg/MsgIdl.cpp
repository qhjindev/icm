
#include <algorithm>
#include <sstream>
#include "MsgIdl.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/Communicator.h>
#include <icc/Log.h>
#include <icc/Guard.h>

namespace demo
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

demo::NetEvent::NetEvent()
{
  port = 0;
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

std::string
demo::NetEvent::toString() const
{
  std::string result("demo::NetEvent-> ");
  result += ("ip:" + __toString(this->ip) + " ");
  result += ("port:" + __toString(this->port) + " ");
  result += ("event:" + __toString(this->event) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const NetEvent& arg)
{
  return "[" + arg.toString() + "]";
}

demo::MsgEvent::MsgEvent()
{
}

bool
demo::MsgEvent::operator==(const MsgEvent& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::MsgEvent::operator!=(const MsgEvent& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(status != __rhs.status)
  {
    return true;
  }
  return false;
}

void
demo::MsgEvent::__write(OutputStream* __os) const
{
  __os->write_string(seq);
  __os->write_string(status);
}

void
demo::MsgEvent::__read(InputStream* __is)
{
  __is->read_string(seq);
  __is->read_string(status);
}

std::string
demo::MsgEvent::toString() const
{
  std::string result("demo::MsgEvent-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("status:" + __toString(this->status) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const MsgEvent& arg)
{
  return "[" + arg.toString() + "]";
}
