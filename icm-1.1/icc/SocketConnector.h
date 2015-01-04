#ifndef ICC_SOCKET_CONNECTOR_H
#define ICC_SOCKET_CONNECTOR_H

#include "icc/SocketStream.h"

class InetAddr;

class SocketConnector
{
public:
  SocketConnector (void);

  int connect (SocketStream &netStream,
               const InetAddr &remoteAddr);

private:

};

#endif //ICC_SOCKET_CONNECTOR_H
