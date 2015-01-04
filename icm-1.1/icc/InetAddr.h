#ifndef _ICC_INET_ADDR_H
#define _ICC_INET_ADDR_H

#include "icc/Addr.h"
#include "os/Basic_Types.h"
#include "os/OS_NS_arpa_inet.h"
#include <string>

class InetAddr : public Addr
{
public:
  // Initialization methods.
  InetAddr (void);

  virtual ~InetAddr (void) {};

  // Create an InetAddr from a <portNumber> and the remote <hostName>.
  InetAddr (u_short portNumber, const char hostName[]);

  InetAddr (u_short portNumber, ACE_UINT32 ipAddr = INADDR_ANY);

  InetAddr (const char address[]);

  int set (u_short portNumber, const char hostName[]);

  //assume the portNumber and inetAddress is host byte order
  int set (u_short portNumber, ACE_UINT32 inetAddress);

  int set (const char address[]);

  void setPortNumber (u_short portNumber);

  // Return the port number, converting it into host byte-order.
  u_short getPortNumber (void) const;

  // Return the "dotted decimal" Internet address representation of the hostname.
  const char *getHostAddr (void) const;

  //assume the ipAddr is host byte order
  int setAddress (ACE_UINT32 ipAddr, int len);

  int stringToAddr (const char address[]);

  void* getAddr (void) const;

  bool operator == (const InetAddr &addr) const;

  bool operator != (const InetAddr &addr) const;

  bool operator < (const InetAddr &addr) const;

  std::string toString () const;

protected:
  void reset (void);

  // Get the type of the address.
  //int getType (void) const;

  // Set the type of the address.
  //void setType (int type);

  // e.g., AF_UNIX,AF_INET,etc.
  //int mAddrType;

  //int mAddrSize;

  /*
  union
  {
    sockaddr_in in4;
#if defined (ACE_HAS_IPV6)
    sockaddr_in in6;
#endif //ACE_HAS_IPV6
  } mInetAddr;*/
  sockaddr_in m_inetAddr;
};

#endif //_ICC_INET_ADDR_H
