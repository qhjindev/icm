#ifndef ICM_TCP_CONNECTOR_IMPL_H
#define ICM_TCP_CONNECTOR_IMPL_H


#include "icm/Communicator.h"
#include "icc/Connector.h"
#include "icm/ServerStrategyFactory.h"
#include "icm/WaitStrategy.h"
//class Acceptor;
//class Communicator;


template <class SVC_HANDLER>
class TcpConnectorImpl : public Connector <SVC_HANDLER>
{
public:  
  TcpConnectorImpl ()
  {
    this->mCommunicator = 0;
  }

  void communicator (Communicator* comm)
  {
    this->mCommunicator = comm;
  }

  virtual int makeSvcHandler (SVC_HANDLER *&sh)
  {
    if (sh == 0)
      sh = new SVC_HANDLER (this->mCommunicator);

    sh->setReactor (this->getReactor ());

    return 0;
  }

  virtual int activateSvcHandler (SVC_HANDLER* sh)
  {
    if (Connector <SVC_HANDLER>::activateSvcHandler (sh) == -1)
      return -1;

    return sh->transport()->waitStrategy ()->registerHandler ();
  }

protected:

  Communicator* mCommunicator;
};

#endif //ICM_TCP_CONNECTOR_IMPL_H
