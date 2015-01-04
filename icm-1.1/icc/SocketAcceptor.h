#ifndef ICC_SOCKET_ACCEPTOR_H
#define ICC_SOCKET_ACCEPTOR_H

#include "icc/SocketStream.h"

class SocketAcceptor : public Socket
{
public:
  SocketAcceptor (void);

  ~SocketAcceptor (void);

  int open (const Addr &localAddr,
            int reuseAddr = 0,
            int protocolFamily = PF_UNSPEC);

  int close (void);

  int accept (SocketStream &newStream,
              InetAddr *remoteAddr = 0) const;

protected:
  int sharedOpen (const Addr &localAddr, 
                  int protocolFamily,
                  int backlog);

/*
  int sharedAcceptStart (TimeValue *timeout,
                         int restart,
                         int &inBlockingMode) const;


  int sharedAcceptFinish (SocketStream newStream,
                          int &inBlockingMode,
                          int resetNewHandle) const;
*/
};

#endif //ICC_SOCKET_ACCEPTOR_H
