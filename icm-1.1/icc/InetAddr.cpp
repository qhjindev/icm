
#include "icc/InetAddr.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_errno.h"
#include "os/OS_NS_stdlib.h"
#include "os/OS_NS_arpa_inet.h"
#include "os/OS_NS_netdb.h"
#include "os/Default_Constants.h"
#include <sstream>


InetAddr::InetAddr (void)
  : Addr (AF_INET, sizeof (m_inetAddr))
{
  this->reset ();
}

InetAddr::InetAddr (u_short portNumber, const char hostName[])
  : Addr (AF_INET, sizeof (m_inetAddr))
{
  OS::memset (&this->m_inetAddr, 0, sizeof (this->m_inetAddr));
  this->set (portNumber, hostName);
}

InetAddr::InetAddr (u_short portNumber, ACE_UINT32 ipAddr)
	: Addr (AF_INET, sizeof (m_inetAddr))
{
  this->reset ();
  if (this->set (portNumber, ipAddr) == -1) {
	  //error
  }
}

InetAddr::InetAddr (const char address[])
: Addr(AF_INET, sizeof (m_inetAddr))
{
  this->reset();
  this->set (address);
}

int 
InetAddr::set (const char address[])
{
  return this->stringToAddr (address);
}

int 
InetAddr::stringToAddr (const char s[])
{
  int result;
  char* ipBuf = 0;
  char* ipAddr = 0;

  ipBuf = OS::strdup (s);
  ipAddr = ipBuf;

  char* portP = OS::strrchr(ipAddr, ':');
  if (portP == 0) { // Assume it's a port number.
    char *endp = 0;
    long const port = OS::strtol (ipAddr, &endp, 10);

    if (*endp == '\0') { // strtol scanned the entire string - all digits
      if (port < 0 || port > ACE_MAX_DEFAULT_PORT)
        result = -1;
      else
        result = this->set(u_short(port), ACE_UINT32(INADDR_ANY));
    } else { //port name, but not implemented yet.
      //result = this->set (ipAddr, ACE_UINT32 (INADDR_ANY));
      result = -1;
    }
  } else {
    *portP = '\0';
    ++portP; // skip over ':'

    char* endp = 0;
    long port = OS::strtol (portP, &endp, 10);

    if (*endp == '\0') { // strtol scanned the entire string - all digits
      if (port < 0 || port > ACE_MAX_DEFAULT_PORT)
        result = -1;
      else
        result = this->set (u_short(port), ipAddr);
    } else {
      result = -1;
    }
  }

  OS::free ((void*)ipBuf);
  return result;
}

int
InetAddr::set (u_short portNumber, const char hostName[])
{
  if (hostName == 0) {
    errno = EINVAL;
    return -1;
  }

  OS::memset ((void*) &this->m_inetAddr, 0, sizeof this->m_inetAddr);

  this->m_inetAddr.sin_family = AF_INET;
  struct in_addr addrv4;
  if (OS::inet_aton (hostName, &addrv4) == 1)
    return this->set (portNumber,ACE_NTOHL(addrv4.s_addr));
  else {
    hostent hentry;
    ACE_HOSTENT_DATA buf;
    int h_error;

    hostent* hp = OS::gethostbyname_r(hostName, &hentry,buf,&h_error);
    if (hp == 0) {
      return -1;
    } else {
      OS::memcpy ((void*)&addrv4.s_addr, hp->h_addr, hp->h_length);
      return this->set (portNumber, ACE_NTOHL (addrv4.s_addr));
    }
  }
}

int
InetAddr::set (u_short portNumber, ACE_UINT32 inetAddress)
{
  this->setAddress (inetAddress, sizeof inetAddress);
  this->setPortNumber (portNumber);

  return 0;
}

void
InetAddr::setPortNumber (u_short portNumber)
{
  portNumber = ACE_HTONS (portNumber);
  this->m_inetAddr.sin_port = portNumber;
}

u_short
InetAddr::getPortNumber (void) const
{
  return ntohs (this->m_inetAddr.sin_port);
}

const char *
InetAddr::getHostAddr (void) const
{
  return OS::inet_ntoa (this->m_inetAddr.sin_addr);
}

int
InetAddr::setAddress (ACE_UINT32 ipAddr, int len)
{
  if (len != 4) {
    errno = EAFNOSUPPORT;
    return -1;
  }

  ipAddr = ACE_HTONL(ipAddr);

  this->baseSet (AF_INET, sizeof (this->m_inetAddr));
  this->m_inetAddr.sin_family = AF_INET;
  this->setSize (sizeof (this->m_inetAddr));
  OS::memcpy (&this->m_inetAddr.sin_addr, &ipAddr, len);

  return 0;
}

void
InetAddr::reset (void)
{
  OS::memset (&this->m_inetAddr, 0, sizeof (this->m_inetAddr));

  this->m_inetAddr.sin_family = AF_INET;
}

void *
InetAddr::getAddr (void) const
{
  return (void*)&this->m_inetAddr;
}

bool
InetAddr::operator == (const InetAddr &addr) const
{
  if (this->getType () != addr.getType ()
      || this->getSize () != addr.getSize ())
    return false;

  return (OS::memcmp (&this->m_inetAddr, &addr.m_inetAddr, this->getSize ()) == 0);
}

bool
InetAddr::operator != (const InetAddr &addr) const
{
  return !((*this) == addr);
}

bool
InetAddr::operator < (const InetAddr &addr) const
{
  return this->m_inetAddr.sin_addr.s_addr < addr.m_inetAddr.sin_addr.s_addr;
}

std::string
InetAddr::toString () const
{
  std::ostringstream hostAndPortStream;
  hostAndPortStream << this->getHostAddr () << ":" << this->getPortNumber ();
  return hostAndPortStream.str ();
}
