#ifndef ICM_TCP_ACCEPTOR_IMPL_H
#define ICM_TCP_ACCEPTOR_IMPL_H

#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icc/Acceptor.h"
#include "icm/ServerStrategyFactory.h"

//class Acceptor;
//class Communicator;


template <class SVC_HANDLER>
class TcpAcceptorImpl : public Acceptor <SVC_HANDLER>
{
public:  
  TcpAcceptorImpl ()
  {
    this->mCommunicator = 0;
  }

  void communicator (Communicator* comm)
  {
    this->mCommunicator = comm;
  }

  void objectAdapter (ObjectAdapter* oa)
  {
    this->mObjectAdapter = oa;
  }

  virtual int makeSvcHandler (SVC_HANDLER *&sh)
  {
    if (sh == 0)
      sh = new SVC_HANDLER (this->mCommunicator, this->mObjectAdapter);

    sh->setReactor (this->getReactor ());

    return 0;
  }

  virtual int activateSvcHandler (SVC_HANDLER *svcHandler)
  {
    ServerStrategyFactory* f = this->mCommunicator->serverFactory ();
    if (f->activateServerConnections () == 1)
      return svcHandler->activate ();

    int result = 0;

    if (svcHandler->getStream ().enable (ACE_NONBLOCK) == -1)
      result = -1;

    if (result == 0 && svcHandler->open ((void *)this) == -1)
      result = -1;

    if (result == -1) {
      svcHandler->close (0);
      return result;
    }

    return this->mCommunicator->reactor ()->registerHandler (svcHandler, EventHandler::READ_MASK);
  }

protected:

  Communicator* mCommunicator;

  ObjectAdapter* mObjectAdapter;

};

#endif //ICM_TCP_ACCEPTOR_IMPL_H
