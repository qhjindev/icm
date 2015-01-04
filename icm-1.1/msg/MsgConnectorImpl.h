#ifndef MSG_CONNECTOR_IMPL_H
#define MSG_CONNECTOR_IMPL_H



#include "icc/Connector.h"
#include "SubscribeClient.h"


class SubscribeClient;

template <class SVC_HANDLER, class CLIENT>
class MsgConnectorImpl : public Connector <SVC_HANDLER>
{
public:
  MsgConnectorImpl (Reactor* r, CLIENT* client)
  {
    this->reactor = r;
    this->client_ = client;
  }

  void client (CLIENT* client)
  {
    this->client_ = client;
  }

  virtual int makeSvcHandler (SVC_HANDLER *&sh)
  {
    if (sh == 0)
      sh = new SVC_HANDLER ();

    sh->setClient(client_);

    //sh->setReactor (this->getReactor ());
    sh->setReactor(this->reactor);

    return 0;
  }

  /*
  virtual int activateSvcHandler (SVC_HANDLER* sh)
  {
    if (Connector <SVC_HANDLER>::activateSvcHandler (sh) == -1)
      return -1;

    return sh->transport()->waitStrategy ()->registerHandler ();
  }*/

protected:
  Reactor* reactor;
  CLIENT* client_;
};



#endif //MSG_CONNECTOR_IMPL_H
