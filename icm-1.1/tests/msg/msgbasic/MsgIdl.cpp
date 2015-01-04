
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

demo::MsgReq::MsgReq()
{
  seq = 0;
  port = 0;
}

bool
demo::MsgReq::operator==(const MsgReq& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::MsgReq::operator!=(const MsgReq& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
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
demo::MsgReq::__write(OutputStream* __os) const
{
  __os->write_long(seq);
  __os->write_string(ip);
  __os->write_short(port);
  __os->write_string(event);
}

void
demo::MsgReq::__read(InputStream* __is)
{
  __is->read_long(seq);
  __is->read_string(ip);
  __is->read_short(port);
  __is->read_string(event);
}

std::string
demo::MsgReq::toString() const
{
  std::string result("demo::MsgReq-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("ip:" + __toString(this->ip) + " ");
  result += ("port:" + __toString(this->port) + " ");
  result += ("event:" + __toString(this->event) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const MsgReq& arg)
{
  return "[" + arg.toString() + "]";
}

demo::MsgRes::MsgRes()
{
  seq = 0;
}

bool
demo::MsgRes::operator==(const MsgRes& __rhs) const
{
  return !operator!=(__rhs);
}

bool
demo::MsgRes::operator!=(const MsgRes& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(result != __rhs.result)
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
demo::MsgRes::__write(OutputStream* __os) const
{
  __os->write_long(seq);
  __os->write_string(result);
  __os->write_string(status);
}

void
demo::MsgRes::__read(InputStream* __is)
{
  __is->read_long(seq);
  __is->read_string(result);
  __is->read_string(status);
}

std::string
demo::MsgRes::toString() const
{
  std::string result("demo::MsgRes-> ");
  result += ("seq:" + __toString(this->seq) + " ");
  result += ("result:" + __toString(this->result) + " ");
  result += ("status:" + __toString(this->status) + " ");
  return trimEnd(result);
}

std::string
demo::__toString(const MsgRes& arg)
{
  return "[" + arg.toString() + "]";
}
