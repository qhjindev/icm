#include <iostream>
#include <icc/ThreadManager.h>
using namespace std;

#include "icm/Communicator.h"
#include "Hello.h"

class AMI_MyHello_sayHelloI: public ::demo::AMI_MyHello_sayHello {
public:

  virtual void response(const ::std::string& ret, Long v) {
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }

};

class AMI_MyHello_sayByeI: public ::demo::AMI_MyHello_sayBye {
public:

  virtual void response(const ::std::string& ret, Long v) {
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }

};

class HelloWorker: public Thread {
public:

  explicit HelloWorker(IcmProxy::demo::MyHello& hello)
  : myHello(hello)
  { }

  virtual int svc(void) {
    unsigned long u =  1024ul * 1024 * 1024 * 12;

    u = 1;
    Long v;
    string ret;

    TimeValue tv(0,10000);
    while(true) {
    ret = myHello.sayHello("Hello, async1", u, v);
      if(ret == "") {
        cout << "call error!!" << endl;
      } else {
        cout << "call success!!" << endl;
        cout << "u:" << u << " v:" << v << " ret:" << ret << endl;
      }

      OS::sleep(tv);
//      OS::sleep(1);
    }

    return 0;
  }
private:
  IcmProxy::demo::MyHello& myHello;
};

class ByeWorker: public Thread {
public:
  explicit ByeWorker(IcmProxy::demo::MyHello& hello)
  : myHello(hello)
  { }

  virtual int svc(void) {
    Long v;
    string ret;

    TimeValue tv(0,10000);
    while(true) {
      ret = myHello.sayBye("Bye, async1", v);
      if(ret == "") {
        cout << "call error!!" << endl;
      } else {
        cout << "call success!!" << endl;
        cout << " v:" << v << " ret:" << ret << endl;
      }

      OS::sleep(tv);
//      OS::sleep(1);
    }

    return 0;
  }
private:
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

  Reference ref1(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello1;
  myHello1.setReference(&ref1);

  Reference ref2(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello2;
  myHello2.setReference(&ref2);

  Reference ref3(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello3;
  myHello3.setReference(&ref3);

  Reference ref4(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello4;
  myHello4.setReference(&ref4);

  HelloWorker worker1(myHello1);
  worker1.activate();
  TimeValue tv(0,1000);
  OS::sleep(tv);
  HelloWorker worker2(myHello2);
  worker2.activate();
  HelloWorker worker3(myHello3);
  worker3.activate();
  HelloWorker worker4(myHello4);
  worker4.activate();

  ByeWorker worker5(myHello1);
  worker5.activate();
  ByeWorker worker6(myHello2);
  worker6.activate();
  ByeWorker worker7(myHello3);
  worker7.activate();
  ByeWorker worker8(myHello4);
  worker8.activate();

  ThreadManager::instance()->wait();

  return 0;
}
