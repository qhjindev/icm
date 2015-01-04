#include "icc/ThreadManager.h"
#include "MsgIdl.h"
#include "msg/MsgClient.h"

using namespace std;

#define MAX_COUNT 100000

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

class ClientThread: public Thread {
public:
  int svc() {

    TimeValue tv1 = OS::gettimeofday();

    for (int i = 0; i < MAX_COUNT; i++) {
      demo::MsgReq req;
      req.seq = i;
      req.ip = "192.168.0.1";
      req.port = 12345;
      req.event = "hi";

      OutputStream os;
      MsgProtocol::start(Msg_Event, &os);
      os.write_string("topic"); //write topic
      os.write_string("MsgReq");
      req.__write(&os);
      if(client->publish(os) ==  -1) {

        if(errno == EWOULDBLOCK) {
          LogDebug << "server busy, sleep a second" << endl;
          sleep(10);
          continue;
        } else {
          LogError << "server broken" << endl;
          break;
        }
      } else {
        LogDebug << "send msg:" << req.toString() << endl;
      }
    }

    TimeValue tv2 = OS::gettimeofday();
    TimeValue delta = tv2 - tv1;
    LogDebug << "send msg count:" << MAX_COUNT << " use time:" << delta.sec() << endl;

    return 0;
  }

  Client* client;
};

int main(int argc, char* argv[]) {

  Client* client = new Client;
  if (!client->init("localhost", 50002)) {
    LogError << "init client failed" << endl;
    return -1;
  }

  ClientThread* t = new ClientThread;
  t->client = client;
  t->activate();

  ThreadManager::instance()->wait();

  return 0;
}
