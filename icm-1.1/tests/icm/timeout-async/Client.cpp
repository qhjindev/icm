#include <iostream>
using namespace std;

#include "icm/Communicator.h"
#include "Hello.h"
#include "icm/Invocation.h"
#include "icm/TransportMuxStrategy.h"

class AMI_MyHello_sayHelloI: public ::demo::AMI_MyHello_sayHello {
public:

  virtual void response(const ::std::string& ret, Long v) {
    ICC_DEBUG("response recved");
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }

};

class Rcver: public Thread {
public:
  Rcver(Communicator& comm, IcmProxy::demo::MyHello& hello)
  : communicator(comm), myHello(hello)
  {}

  virtual int svc(void) {
    sleep(2);
//    ICC_DEBUG("calling comm->run().......");
//    return communicator.run();
    while(true) {
      ICC_DEBUG("async calling.......");
      myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, sync1", 12);
      sleep(3);
    } // while
    return 0;
  }
private:
  Communicator& communicator;
  IcmProxy::demo::MyHello& myHello;
};

int main(int argc, char* argv[]) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  Communicator* comm = Communicator::instance();
  if (comm->init() == -1)
    return -1;

  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000), new TimeValue(3));
  //Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.30.2.70", 3000), new TimeValue(3));
  //Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.16.10.23", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference(&ref);

  Short u = 15;
  Long v;

  Rcver rcver(*comm, myHello);
  rcver.activate();

  comm->run();

//  while(true) {
//
//    ICC_DEBUG("async calling.......");
//    myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, sync1", u);
//    sleep(10);
//  } // while

  return 0;
}
