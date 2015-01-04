#ifndef MSG_ACCEPTOR_IMPL_H
#define MSG_ACCEPTOR_IMPL_H



#include "icc/Acceptor.h"
#include "MsgServer.h"


class SubscribeClient;

template <class SVC_HANDLER, class SERVER>
class MsgAcceptorImpl : public Acceptor <SVC_HANDLER>
{
public:
  MsgAcceptorImpl (Reactor* r, SERVER* server)
  {
    this->reactor = r;
    this->server_ = server;
  }

  void server (SERVER* server)
  {
    this->server_ = server;
  }

  virtual int makeSvcHandler (SVC_HANDLER *&sh)
  {
    if (sh == 0)
      sh = new SVC_HANDLER;

    sh->setServer(server_);

    sh->setReactor(this->reactor);

    return 0;
  }

protected:
  Reactor* reactor;
  SERVER* server_;
};



#endif //MSG_ACCEPTOR_IMPL_H
