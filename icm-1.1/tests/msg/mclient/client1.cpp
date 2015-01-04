
#include "icc/SelectReactor.h"
#include "icc/Reactor.h"
#include "icc/ThreadManager.h"
#include "msg/SubscribeClient.h"
#include "msg/PublishClient.h"

#include "icc/Thread.h"
#include "icc/SynchTraits.h"

#include "mclient.h"

using namespace std;

template<class Event>
class MySub1 : public SubscribeClient<Event>, public Thread {
public:
  MySub1(Reactor* reactor) : SubscribeClient<Event>(reactor)  {

  }

  virtual void onEvent(Event* event) {
    std::cout << event->topic << std::endl;
  }

  virtual int svc(void) {
    if(!this->init())
      return -1;

    std::string topic = "topic1";
    if(!this->subscribeTopic(topic)) {
      LogMsg << "subscribeTopic: " << topic << " failed: " << perr << endl;
      return -1;
    }

    this->run();
    return 0;
  }
};

int main(int argc, char* argv[]) {

  SelectReactor selectReactor1;
  Reactor reactor1(&selectReactor1);
  MySub1<Icm::Event1>* sub = new MySub1<Icm::Event1>(&reactor1);

  sub->activate();

  OS::sleep(5);

  //SelectReactor selectReactor2;
  //Reactor reactor2(&selectReactor2);

  PublishClient<Icm::Event2> pub1;
  if(!pub1.init())
    return -1;

  Icm::Event2 event2;
  //::std::string topic;
  event2.topic = "topic2";
  event2.item1 = 123;
  event2.item2 = "item2";

  string topic2 = "topic2";
  for (int i = 0; i < 10; i++)
    pub1.publish(topic2,&event2);

  ThreadManager::instance()->wait();
}
