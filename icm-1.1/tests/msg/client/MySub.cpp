
#include <string>
#include "icc/Log.h"
#include "icc/ThreadManager.h"
#include "IcmEvent.h"
#include "msg/SubscribeClient.h"

using namespace std;

class MySub : public SubscribeClient {
public:
  MySub(Reactor* reactor = Reactor::instance())
    : SubscribeClient(reactor) {

  }

  void handleEvent(InputStream& is) {
    string type;
    is.read_string(type);
    if(type == "type1") {
      Icm::Event1 event1;
      event1.__read(&is);
      cout << "receive event type:" << type << endl;
      int i = 5;
    } else if(type == "type2") {
      Icm::Event2 event2;
      event2.__read(&is);
      cout << "receive event type:" << type << endl;
      int j = 10;
    }

    //Event event;
    //event.__read(state.is);
    //onEvent(&event);
  }
};


int main(int argc, char* argv[]) {
  SelectReactor selectReactor1;
  Reactor reactor1(&selectReactor1);
  MySub* sub = new MySub(&reactor1);

  if(!sub->init("127.0.0.1"))
    return -1;

  std::string topic = "topic1";
  if(!sub->subscribeTopic(topic)) {
    LogMsg << "subscribeTopic: " << topic << " failed: " << perr << endl;
    return -1;
  }

  sub->run();

  ThreadManager::instance()->wait();
}

