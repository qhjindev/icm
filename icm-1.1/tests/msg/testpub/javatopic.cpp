
#include <algorithm>
#include <string>
#include "javatopic.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>

bool
JavaTopic::Request3::operator==(const Request3& __rhs) const
{
  return !operator!=(__rhs);
}

bool
JavaTopic::Request3::operator!=(const Request3& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(cpu != __rhs.cpu)
  {
    return true;
  }
  if(mem != __rhs.mem)
  {
    return true;
  }
  return false;
}

void
JavaTopic::Request3::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(cpu);
  __os->write_int(mem);
}

void
JavaTopic::Request3::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(cpu);
  __is->read_int(mem);
}

bool
JavaTopic::Request4::operator==(const Request4& __rhs) const
{
  return !operator!=(__rhs);
}

bool
JavaTopic::Request4::operator!=(const Request4& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(seq != __rhs.seq)
  {
    return true;
  }
  if(nodeId != __rhs.nodeId)
  {
    return true;
  }
  if(instanceId != __rhs.instanceId)
  {
    return true;
  }
  if(result != __rhs.result)
  {
    return true;
  }
  if(state != __rhs.state)
  {
    return true;
  }
  return false;
}

void
JavaTopic::Request4::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_string(result);
  __os->write_int(state);
}

void
JavaTopic::Request4::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_string(result);
  __is->read_int(state);
}
