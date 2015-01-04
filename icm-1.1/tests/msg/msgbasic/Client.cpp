#include "icc/ThreadManager.h"
#include "MsgIdl.h"
#include "msg/MsgClient.h"

using namespace std;

class Client: public MsgClient {
public:

  virtual void handleEvent(InputStream& is) {
    string type;
    is.read_string(type);
    if (type == "MsgRes") {
      demo::MsgRes* event = new demo::MsgRes;
      event->__read(&is);

      LogDebug << "recv:" << event->toString() << endl;
    }
  }

  // to be inherited by sub class
  virtual void onConnectionError() {
    LogError << "connection to icmmsg error!!" << endl;
  }

  virtual void onConnectionEstablished() {
    LogError << ("connection to icmmsg established!!") << endl;
  }
};

int main(int argc, char* argv[]) {

  Client client;
  if (!client.init("localhost", 50002)) {
    LogError << "init client failed" << endl;
    return -1;
  }

  TimeValue tv(5);
  ThreadManager::instance()->wait(&tv);

  demo::MsgReq req;
  req.seq = 100;
  req.ip = "192.168.0.1";
  req.port = 12345;
  req.event = "hi";

  OutputStream os;
  MsgProtocol::start(Msg_Event, &os);
  os.write_string("topic"); //write topic
  os.write_string("MsgReq");
  req.__write(&os);
  client.publish(os);

  ThreadManager::instance()->wait();

  return 0;
}
