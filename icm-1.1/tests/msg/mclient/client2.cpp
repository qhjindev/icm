
#include "icc/SelectReactor.h"
#include "icc/Reactor.h"
#include "icc/ThreadManager.h"
#include "msg/SubscribeClient.h"
#include "msg/PublishClient.h"

#include "icc/Thread.h"

#include "mclient.h"

template<class Event>
class MySub2 : public SubscribeClient<Event>, public Thread {

public:
  MySub2(Reactor* reactor) : SubscribeClient<Event>(reactor)  {

  }

  virtual void onEvent(Event* event) {
    std::cout << event->topic << std::endl;
  }

  virtual int svc(void) {
    if(!this->init())
      return -1;

    std::string topic = "topic2";
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
  MySub2<Icm::Event2>* sub = new MySub2<Icm::Event2>(&reactor1);

  sub->activate();

  OS::sleep(5);

  //SelectReactor selectReactor2;
  //Reactor reactor2(&selectReactor2);

  PublishClient<Icm::Event1> pub1;
  if(!pub1.init())
    return -1;

  Icm::Event1 event1;
  //::std::string topic;
  event1.topic = "topic1";
  event1.item1 = 123;
  event1.item2 = "item2";
  event1.dev.d = "d";
  event1.dev.a = "a";

  ::Icm::Port port;
  port.i = 12;
  port.str = "123.456.789";
  event1.conn.push_back(port);

  std::string topic1("topic1");
  for (int i = 0; i < 10; i++)
    pub1.publish(topic1,&event1);

  ThreadManager::instance()->wait();
}
