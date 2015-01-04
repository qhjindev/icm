#ifndef ICC_SOCKET_H
#define ICC_SOCKET_H

#include "icc/InetAddr.h"

class Socket
{
public:
  Socket ();
  ~Socket ();

  int open (int type,
            int protocolFamily,
            int protocol,
            int reuseAddr);

  int close (void);

  ACE_HANDLE getHandle (void) const;

  void setHandle (ACE_HANDLE handle);

  int enable (int value) const;

  int disable (int value) const;

  int control (int cmd, void *arg) const;

  int getLocalAddr (InetAddr &);

  int getRemoteAddr (InetAddr &) const;

  int setOption (int level,
                 int option,
                 void *optval,
                 int optlen) const;

  int getOption (int level,
                 int iptin,
                 void *optval,
                 int *optlen) const;

protected:
  ACE_HANDLE m_handle;
};

#endif //ICC_SOCKET_H
