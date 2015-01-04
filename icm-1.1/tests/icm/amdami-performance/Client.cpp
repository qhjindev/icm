#include <iostream>
using namespace std;

#include "icm/Communicator.h"
#include "Hello.h"
#include <icc/ThreadManager.h>

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

class Rcver: public Thread {
public:
  Rcver(Communicator& comm)
  : communicator(comm)
  {}

  virtual int svc(void) {
    return communicator.run();
  }
private:
  Communicator& communicator;
};

class Caller: public Thread {
public:
  Caller(IcmProxy::demo::MyHello& myHello_, const string& helloString_, const string& byeString_)
  : myHello(myHello_), helloString(helloString_), byeString(byeString_)
  {}

  virtual int svc(void) {
    unsigned long u =  1024ul * 1024 * 1024 * 12;
    long v;
    TimeValue tv(0,10000);
    while(true) {
  //    myHello.sayHello("Hello, async1", u, v);
  //    myHello.sayBye("Bye, async1", v);
      OS::sleep(tv);
      myHello.sayHello_async(new AMI_MyHello_sayHelloI, helloString, u);
      myHello.sayBye_async(new AMI_MyHello_sayByeI, byeString);
    }
    return -1;
  }
private:
  IcmProxy::demo::MyHello& myHello;
  string helloString;
  string byeString;
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
  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference(&ref);

  Reference ref2(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000));
  IcmProxy::demo::MyHello myHello2;
  myHello2.setReference(&ref2);

  unsigned long u =  1024ul * 1024 * 1024 * 12;
//  ICC_DEBUG("uuuu: %lu", u);
//  cout << "######  uuuu:" << u << endl;
//  printf("***********  uuuu: %lu\n", u);

  Long v;
  string ret;

  Rcver rcver(*comm);
  rcver.activate();
  Caller caller1(myHello, "Hello, async1", "Bye, async1");
  caller1.activate();

  TimeValue tv(0,1000);
  OS::sleep(tv);
  Caller caller2(myHello2, "Hello, async2", "Bye, async2");
  caller2.activate();
//  TimeValue tv(0,10000);
//  while(true) {
////    myHello.sayHello("Hello, async1", u, v);
////    myHello.sayBye("Bye, async1", v);
//    OS::sleep(tv);
//    myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async1", u);
//    myHello.sayBye_async(new AMI_MyHello_sayByeI, "Bye, async1");
//  }

  ThreadManager::instance()->wait();
  return 0;
}
