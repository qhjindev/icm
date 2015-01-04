#ifndef ICC_SOCKET_DGRAM_H
#define ICC_SOCKET_DGRAM_H

#include "Socket.h"

class SocketDgram : public Socket
{
public:
  SocketDgram (void);

  SocketDgram (const Addr& local,
               int protocolFamily = AF_INET,
               int protocol = 0,
               int reuseAddr = 0);

  ~SocketDgram (void);

  int open (const Addr &local,
            int protocolFamily = AF_INET,
            int protocol = 0,
            int reuseAddr = 0);

  ssize_t send (const void *buf,
                size_t n,
                const InetAddr &addr,
                int flags = 0) const;

  ssize_t recv (void *buf,
                size_t n,
                InetAddr &addr,
                int flags = 0) const;

protected:
  int sharedOpen(const Addr& local, int protocolFamily);

};

#endif //ICC_SOCKET_DGRAM_H
