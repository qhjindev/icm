#include <string>
#include "icc/Log.h"
#include "icc/ThreadManager.h"
#include "cpptopic.h"
#include "javatopic.h"
#include "msg/SubscribeClient.h"

#include <icc/Task.h>

ThreadMutex mutex;

using namespace std;

string testClientId = "testId1";
class TestSub: public SubscribeClient {
public:
  TestSub(Reactor* reactor = Reactor::instance()) :
      SubscribeClient(reactor) {

  }

  void handleEvent(InputStream& is) {
//    ICC_ERROR("*******************************");
    {
      static int recvedEventCount = 0;
      ACE_GUARD(ThreadMutex, guard, ::mutex);
      if(++recvedEventCount % 10000 == 0)
        ICC_ERROR("total recved events: %d", recvedEventCount);
    }

    string type;
    is.read_string(type);
    ICC_DEBUG("receive event type: %s", type.c_str());
    ICC_DEBUG("msg length: %d", is.length());
    if (type == "Request1") {
      CppTopic::Request1 rq1;
      rq1.__read(&is);
      cout << "seq:" << rq1.seq << ", nodeId: " << rq1.nodeId << ", instanceId: " << rq1.instanceId;
      cout << ", cpu: " << rq1.cpu << ", mem: " << rq1.mem << endl;
    } else if (type == "Request2") {
      CppTopic::Request2 rq2;
      rq2.__read(&is);
      cout << "seq:" << rq2.seq << ", nodeId: " << rq2.nodeId << ", instanceId: " << rq2.instanceId;
      cout << ", result: " << rq2.result << ", state: " << rq2.state << endl;
    } else if (type == "Request3") {
      JavaTopic::Request3 rq3;
      rq3.__read(&is);
//      cout << "seq:" << rq3.seq << ", nodeId: " << rq3.nodeId << ", instanceId: " << rq3.instanceId;
//      cout << ", cpu: " << rq3.cpu << ", mem: " << rq3.mem << endl;
    } else if (type == "Request4") {
      JavaTopic::Request4 rq4;
      rq4.__read(&is);
      cout << "seq:" << rq4.seq << ", nodeId: " << rq4.nodeId << ", instanceId: " << rq4.instanceId;
      cout << ", result: " << rq4.result << ", state: " << rq4.state << endl;
    }
  }
};

class SubThread: public Thread {
public:
  SubThread(Reactor* reactor) :
      sub(new TestSub(reactor)) {
    //bool result = sub->init("172.16.10.23");
    bool result = sub->init("127.0.0.1");
    assert(result);
  }
  ~SubThread() {
    if (sub)
      delete sub;
  }
  int subscribeTopic(const string& topic) {
    return sub->subscribeTopic(topic);
//    return sub->subscribeTopic(topic);
  }
  virtual int svc(void) {
    sub->run();
    return 0;
  }
private:
  TestSub* sub;
};

const int Subscribers = 3;
int main(int argc, char* argv[]) {
/*  while(true) { //test memory leak
      MessageBlock* mb = new MessageBlock;
      mb->base((char *) 0, sizeof(int));
  delete mb;
  }
*/


  Log::instance()->open("", LM_ERROR);
  //Log::instance ()->open ("log");
  //Log::instance()->open("", LM_DEBUG);

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  SelectReactor* selectReactor = new SelectReactor;
  Reactor* reactor = new Reactor(selectReactor);
  SubThread* st = new SubThread(reactor);
  for (int i = 0; i < Subscribers; ++i) {
    stringstream ss;
    ss << "cpptopic" << i + 1;
    string topic = ss.str();
//    string topic = "node1";
    st->subscribeTopic(topic);
  }

  st->activate();

  ThreadManager::instance()->wait();
  return 0;
}

