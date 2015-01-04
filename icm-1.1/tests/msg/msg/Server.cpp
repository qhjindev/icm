#include "Server.h"
#include "msg/Msg.h"
#include "MsgIdl.h"
#include "icc/ThreadManager.h"
#include "msg/MsgProtocol.h"

using namespace std;

void Server::handleEvent(InputStream& is) {
  string type;
  is.read_string(type);
  if (type == "NetEvent") {
    demo::NetEvent* event = new demo::NetEvent;
    event->__read(&is);
    LogDebug << "recv evt: " << event->toString() << endl;
  }

  if (!startSender) {
    MsgSender* sender = new MsgSender;
    sender->server = this;
    sender->activate();
    startSender = true;
  }
}

int MsgSender::svc() {
  sleep(5);
  demo::MsgEvent msgEvent;

  for (int i = 0; i < 10000; i++) {
    ostringstream oss;
    oss << i;
    msgEvent.seq = oss.str();
    msgEvent.status = "success";

    OutputStream os;
    MsgProtocol::start(Msg_Event, &os);
    os.write_string("topic"); //write topic
    os.write_string("MsgEvent");
    msgEvent.__write(&os);
    server->publish(os);

    sleep(3);
  }

  return 0;
}

int main(int argc, char* argv[]) {

  Server server(Reactor::instance());
  if (!server.init(10002)) {
    LogError << "err init server" << endl;
    return -1;
  }

  server.activate();

  ThreadManager::instance()->wait();

  return 0;
}

