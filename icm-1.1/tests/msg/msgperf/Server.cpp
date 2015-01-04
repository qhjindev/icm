
#include "icc/Log.h"
#include "msg/Msg.h"
#include "MsgIdl.h"
#include "icc/ThreadManager.h"
#include "msg/MsgProtocol.h"
#include "msg/MsgServer.h"

using namespace std;

class Server: public MsgServer {
public:
  Server(Reactor* reactor) :
      MsgServer(reactor) {

  }

  virtual ~Server() {

  }

  void handleEvent(InputStream& is) {
    string type;
    is.read_string(type);
    if (type == "MsgReq") {
      demo::MsgReq* req = new demo::MsgReq;
      req->__read(&is);
      LogDebug << "recv evt: " << req->toString() << endl;

      demo::MsgRes res;
      res.seq = req->seq;
      res.status = "progress";
      res.result = "ok";

      OutputStream os;
      MsgProtocol::start(Msg_Event, &os);
      os.write_string("topic"); //write topic
      os.write_string("MsgRes");
      res.__write(&os);
      this->publish(os);

      delete req;
    }

  }

};

int main(int argc, char* argv[]) {

  Server server(Reactor::instance());
  if (!server.init(50002)) {
    LogError << "err init server" << endl;
    return -1;
  }

  ThreadManager::instance()->wait();

  return 0;
}

