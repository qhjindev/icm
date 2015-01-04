#include "icc/ThreadManager.h"
#include "MsgIdl.h"
#include "msg/MsgClient.h"

using namespace std;

class Client: public MsgClient {
public:

  virtual void handleEvent(InputStream& is) {
    string type;
    is.read_string(type);
    if (type == "MsgEvent") {
      demo::MsgEvent* event = new demo::MsgEvent;
      event->__read(&is);

      LogDebug << "recv:" << event->toString() << endl;

      static short port = 1;
      demo::NetEvent event2;
      event2.ip = "192.168.0.1";
      event2.port = port++;
      event2.event = "abc";

      OutputStream os;
      MsgProtocol::start(Msg_Event, &os);
      os.write_string("topic"); //write topic
      os.write_string("NetEvent");
      event2.__write(&os);
      this->publish(os);
    }
  }
};

int main(int argc, char* argv[]) {

  Client client;
  if (!client.init("localhost", 10002)) {
    LogError << "init client failed" << endl;
    return -1;
  }

  demo::NetEvent event;
  event.ip = "192.1680.1";
  event.port = 1234;
  event.event = "abc";

  OutputStream os;
  MsgProtocol::start(Msg_Event, &os);
  os.write_string("topic"); //write topic
  os.write_string("NetEvent");
  event.__write(&os);
  client.publish(os);
  std::string logStr = event.toString();

  ThreadManager::instance()->wait();

  return 0;
}
