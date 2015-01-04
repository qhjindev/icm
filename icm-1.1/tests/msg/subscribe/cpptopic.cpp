
#include <algorithm>
#include <string>
#include "cpptopic.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>

bool
CppTopic::Request1::operator==(const Request1& __rhs) const
{
  return !operator!=(__rhs);
}

bool
CppTopic::Request1::operator!=(const Request1& __rhs) const
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
CppTopic::Request1::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_int(cpu);
  __os->write_int(mem);
}

void
CppTopic::Request1::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_int(cpu);
  __is->read_int(mem);
}

bool
CppTopic::Request2::operator==(const Request2& __rhs) const
{
  return !operator!=(__rhs);
}

bool
CppTopic::Request2::operator!=(const Request2& __rhs) const
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
CppTopic::Request2::__write(OutputStream* __os) const
{
  __os->write_int(seq);
  __os->write_string(nodeId);
  __os->write_string(instanceId);
  __os->write_string(result);
  __os->write_int(state);
}

void
CppTopic::Request2::__read(InputStream* __is)
{
  __is->read_int(seq);
  __is->read_string(nodeId);
  __is->read_string(instanceId);
  __is->read_string(result);
  __is->read_int(state);
}
