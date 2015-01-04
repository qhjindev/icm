
#include <algorithm>
#include <string>
#include "IcmEvent.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>

bool
Icm::Port::operator==(const Port& __rhs) const
{
  return !operator!=(__rhs);
}

bool
Icm::Port::operator!=(const Port& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(i != __rhs.i)
  {
    return true;
  }
  if(str != __rhs.str)
  {
    return true;
  }
  return false;
}

void
Icm::Port::__write(OutputStream* __os) const
{
  __os->write_int(i);
  __os->write_string(str);
}

void
Icm::Port::__read(InputStream* __is)
{
  __is->read_int(i);
  __is->read_string(str);
}

bool
Icm::Device::operator==(const Device& __rhs) const
{
  return !operator!=(__rhs);
}

bool
Icm::Device::operator!=(const Device& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(d != __rhs.d)
  {
    return true;
  }
  if(a != __rhs.a)
  {
    return true;
  }
  return false;
}

void
Icm::Device::__write(OutputStream* __os) const
{
  __os->write_string(d);
  __os->write_string(a);
}

void
Icm::Device::__read(InputStream* __is)
{
  __is->read_string(d);
  __is->read_string(a);
}

void
Icm::__write(OutputStream* __os, const ::Icm::Port* begin, const ::Icm::Port* end, ::Icm::__U__Ports)
{
  Short size = static_cast< Short>(end - begin);
  __os->write_short(size);
  for(int i = 0; i < size; ++i)
  {
    begin[i].__write(__os);
  }
}

void
Icm::__read(InputStream* __is, ::Icm::Ports& v, ::Icm::__U__Ports)
{
  Short sz;
  __is->read_short(sz);
  v.resize(sz);
  for(int i = 0; i < sz; ++i)
  {
    v[i].__read(__is);
  }
}

bool
Icm::Event1::operator==(const Event1& __rhs) const
{
  return !operator!=(__rhs);
}

bool
Icm::Event1::operator!=(const Event1& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(topic != __rhs.topic)
  {
    return true;
  }
  if(item1 != __rhs.item1)
  {
    return true;
  }
  if(item2 != __rhs.item2)
  {
    return true;
  }
  if(dev != __rhs.dev)
  {
    return true;
  }
  if(conn != __rhs.conn)
  {
    return true;
  }
  return false;
}

void
Icm::Event1::__write(OutputStream* __os) const
{
  __os->write_string(topic);
  __os->write_short(item1);
  __os->write_string(item2);
  dev.__write(__os);
  if(conn.size() == 0)
  {
    __os->write_short(0);
  }
  else
  {
    ::Icm::__write(__os, &conn[0], &conn[0] + conn.size(), ::Icm::__U__Ports());
  }
}

void
Icm::Event1::__read(InputStream* __is)
{
  __is->read_string(topic);
  __is->read_short(item1);
  __is->read_string(item2);
  dev.__read(__is);
  ::Icm::__read(__is, conn, ::Icm::__U__Ports());
}

bool
Icm::Event2::operator==(const Event2& __rhs) const
{
  return !operator!=(__rhs);
}

bool
Icm::Event2::operator!=(const Event2& __rhs) const
{
  if(this == &__rhs)
  {
    return false;
  }
  if(item1 != __rhs.item1)
  {
    return true;
  }
  if(item2 != __rhs.item2)
  {
    return true;
  }
  return false;
}

void
Icm::Event2::__write(OutputStream* __os) const
{
  __os->write_string(item1);
  __os->write_string(item2);
}

void
Icm::Event2::__read(InputStream* __is)
{
  __is->read_string(item1);
  __is->read_string(item2);
}
