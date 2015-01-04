#ifndef ICM_TCP_CONNECTOR_H
#define ICM_TCP_CONNECTOR_H

#include "icc/Connector.h"

#include "icm/Pluggable.h"
#include "icm/TcpConnection.h"
#include "icm/TcpConnectorImpl.h"

class Reference;
class Communicator;
//class TcpClientConnectionHandler;

class TcpConnector : public IcmConnector
{
public:
  TcpConnector ();

  ~TcpConnector (void);

  int open (Communicator* comm);

  int close (void);

  int connect (InetAddr& addr, 
               IcmTransport*& transport, 
               TimeValue* maxWaitTime);

private:
  TcpConnectorImpl<TcpConnectionHandler> mBaseConnector;

};

#endif //ICM_TCP_CONNECTOR_H
