
#include "Addr.h"

#include "os/os_include/sys/os_socket.h"

const Addr Addr::sapAny (AF_ANY, -1);

Addr::Addr(int type, int size) :
  m_addrType (type),m_addrSize (size)
{
}

int
Addr::getSize (void) const
{
  return this->m_addrSize;
}

void
Addr::setSize (int size)
{
  this->m_addrSize = size;
}

int
Addr::getType (void) const
{
  return this->m_addrType;
}

void
Addr::setType (int type)
{
  this->m_addrType = type;
}

void
Addr::setAddr (void *, int)
{
}

void *
Addr::getAddr(void) const
{
  return 0;
}

void
Addr::baseSet (int type, int size)
{
  this->m_addrType = type;
  this->m_addrSize = size;
}

bool
Addr::operator == (const Addr &addr) const
{
  return (addr.m_addrType == this->m_addrType &&
          addr.m_addrSize == this->m_addrSize   );
}

bool
Addr::operator != (const Addr &addr) const
{
  return (addr.m_addrType != this->m_addrType ||
          addr.m_addrSize != this->m_addrSize   );
}


