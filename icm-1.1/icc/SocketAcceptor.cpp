
#include "icc/SocketAcceptor.h"
#include "os/OS_NS_sys_socket.h"
#include "os/OS_NS_string.h"

SocketAcceptor::SocketAcceptor(void)
{
}

SocketAcceptor::~SocketAcceptor(void)
{
}

int
SocketAcceptor::open (const Addr &localAddr,
                      int reuseAddr ,
                      int protocolFamily)
{
  if (localAddr != Addr::sapAny)
    protocolFamily = localAddr.getType ();
  else if (protocolFamily == PF_UNSPEC) {
    protocolFamily = PF_INET;
  }

  if (Socket::open (SOCK_STREAM, protocolFamily, 0, reuseAddr) == -1)
    return -1;
  else
    return this->sharedOpen (localAddr, protocolFamily, 0x7fffffff);
}

int
SocketAcceptor::sharedOpen (const Addr &localAddr,
                            int protocolFamily,
                            int backlog)
{
  int error = 0;

  if (protocolFamily == PF_INET) {
    sockaddr_in localInetAddr;
    OS::memset (reinterpret_cast<void *> (&localInetAddr), 0, sizeof localInetAddr);

    if (localAddr == Addr::sapAny) {
      localInetAddr.sin_port = 0;
    } else
      localInetAddr = *reinterpret_cast<sockaddr_in *> (localAddr.getAddr ());

    /*if (localInetAddr.sin_port == 0) {
      if (OS::bindPort (this->getHandle (), ntohl (ACE_UINT32 (localInetAddr.sin_addr.s_addr))) == -1)
        error = 1;
    } else */if (OS::bind (this->getHandle (), reinterpret_cast<sockaddr *> (&localInetAddr), sizeof localInetAddr) == -1)
      error = -1;
  } else if (OS::bind (this->getHandle (), (sockaddr*) localAddr.getAddr (), localAddr.getSize ()) == -1)
    error = 1;

  if (error != 0 || OS::listen (this->getHandle (), backlog) == -1) {
    error = 1;
    this->close ();
  }

  return error ? -1 : 0;
}

int
SocketAcceptor::accept (SocketStream &newStream,
                        InetAddr *remoteAddr) const
{
  int *lenPtr = 0;
  sockaddr *addr = 0;
  int len = 0;

  if (remoteAddr != 0) {
    len = remoteAddr->getSize ();
    lenPtr = &len;
    addr = (sockaddr *) remoteAddr->getAddr ();
  }

  newStream.setHandle (OS::accept (this->getHandle (), addr, lenPtr));
  if (newStream.getHandle () != ACE_INVALID_HANDLE  && remoteAddr != 0) {
    remoteAddr->setSize (len);
    if (addr)
      remoteAddr->setType (addr->sa_family);
  }

  ACE_HANDLE newHandle = newStream.getHandle ();
  return newHandle == ACE_INVALID_HANDLE ? -1 : 0;
}

int
SocketAcceptor::close (void)
{
  return Socket::close ();
}

