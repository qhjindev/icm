
#include "msg/MsgServer.h"

class Server: public MsgServer {
public:
  Server(Reactor* reactor) :
    MsgServer(reactor) {
    startSender = false;
  }

  virtual ~Server() {

  }

  void handleEvent(InputStream& is);

  bool startSender;
};

class MsgSender : public Thread {
public:
  Server* server;

protected:
  virtual int svc();
};
