#ifndef ICM_TCP_FACTORY_H
#define ICM_TCP_FACTORY_H

#include "icm/Pluggable.h"
#include "TcpAcceptor.h"
#include "TcpConnector.h"

//class IcmProtocolFactory;
//class TcpAcceptor;
//class TcpConnector;
//class IcmAcceptor;
//class IcmConnector;

class TcpProtocolFactory : public IcmProtocolFactory
{

public:

  virtual IcmAcceptor* makeAcceptor (void)
  {
    return new TcpAcceptor;
  }

  virtual IcmConnector* makeConnector (void)
  {
    return new TcpConnector;
  }

  virtual int matchTag (const std::string& tag)
  {
    return (tag == "TCP") ? 1 : 0;
  }

};

#endif //ICM_TCP_FACTORY_H
