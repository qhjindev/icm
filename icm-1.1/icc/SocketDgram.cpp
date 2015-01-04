
#include "Log.h"
#include "SocketDgram.h"
#include "os/OS_NS_sys_socket.h"

SocketDgram::SocketDgram() {

}

SocketDgram::SocketDgram(const Addr& local,
                         int protocolFamily,
                         int protocol,
                         int reuseAddr) {

  if (this->open(local, protocolFamily, protocol, reuseAddr) == -1)
    LogError << perr << endl;
}

SocketDgram::~SocketDgram() {

}

int
bindport(ACE_HANDLE handle, ACE_UINT32 ip_addr, int address_family) {
  InetAddr addr = InetAddr((u_short)0, ip_addr);
  return OS::bind(handle, (sockaddr*)addr.getAddr(), addr.getSize());
}

int
SocketDgram::sharedOpen(const Addr& local, int protocolFamily) {
  bool error = false;

  if(local == Addr::sapAny) {
    if(protocolFamily == PF_INET) {
      if(bindport(this->getHandle(), INADDR_ANY, protocolFamily) == -1)
        error = true;
    }
  } else if(OS::bind(this->getHandle(),
            reinterpret_cast<sockaddr*> (local.getAddr()),
            local.getSize()) == -1)
    error = true;

  if(error)
    this->close();

  return error ? -1:0;
}

int
SocketDgram::open (const Addr &local,
                   int protocolFamily,
                   int protocol,
                   int reuseAddr) {
  if(local != Addr::sapAny)
    protocolFamily = local.getType();
  else if (protocolFamily == PF_UNSPEC)
    protocolFamily = PF_INET;

  if(Socket::open(SOCK_DGRAM, protocolFamily, protocol, reuseAddr) == -1)
    return -1;
  else
    return this->sharedOpen(local, protocolFamily);
}

ssize_t
SocketDgram::send (const void *buf,
                   size_t n,
                   const InetAddr &addr,
                   int flags) const {
  sockaddr* saddr = (sockaddr*)addr.getAddr();
  int len = addr.getSize();
  return OS::sendto(this->getHandle(),
                    (const char*)buf,
                    n,
                    flags,
                    (struct sockaddr*)saddr,
                    len);
}

ssize_t
SocketDgram::recv (void *buf,
              size_t n,
              InetAddr &addr,
              int flags) const {
  sockaddr* saddr = (sockaddr*)addr.getAddr();
  int addrlen = addr.getSize();

  ssize_t const status = OS::recvfrom(this->getHandle(),
                                      (char*)buf,
                                      n,
                                      flags,
                                      (sockaddr*)saddr,
                                      &addrlen);
  addr.setSize(addrlen);
  addr.setType(saddr->sa_family);
  return status;
}


