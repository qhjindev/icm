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
      sleep(10);
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

  //Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000));
  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.16.10.23", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference(&ref);

  Short u = 15;
  Long v;

  while(true) {

  ICC_DEBUG("sync calling.......");
  sleep(1);
  string ret = myHello.sayHello("Hello, sync1", u, v);
  if(ret == "") {
    cout << "call error!!" << endl;
  } else {
    cout << "call success!!" << endl;
    cout << "u:" << u << " v:" << v << " ret:" << ret << endl;
  }

  } // while

//  ICC_DEBUG("sync calling.......");
//  ret = myHello.sayHello("Hello, sync2", u, v);
//  cout << "u:" << u << " v:" << v << " ret:" << ret << endl;

//  ICC_DEBUG("async calling.......");
//  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async1", u);
//
//  ICC_DEBUG("async calling.......");
//  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async2", u);
//
//  ICC_DEBUG("async calling.......");
//  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async3", u);

//  static const char* __operation("sayHello");
//  TwowayAsynchInvocation _invocation (&ref, __operation, ref.communicator (), new AMI_MyHello_sayHelloI, ref.getMaxWaitTime());
//  IcmTransport * p = 0;
//  int ok = _invocation.start (p);
//  if (ok != 0)
//  {
//    IcmProxy::setCallErrno( ICM_INVOCATION_START_FAILED );
//    ICC_ERROR("invocation start ERROR!!!!!!");
//    return -1;
//  }
//  ok = _invocation.prepareHeader (1);
//  if (ok != 0)
//  {
//    IcmProxy::setCallErrno( ICM_INVOCATION_PREPAREHEADER_FAILED );
//    return -1;
//  }
//  OutputStream* __os = _invocation.outStream();
//  __os->write_string("Hello, asynctest");
//  __os->write_short(u);
////  ok = _invocation.invoke();
////  if(ok != 0)
////  {
////    IcmProxy::setCallErrno( ICM_INVOCATION_INVOKE_FAILED );
//////    this->transport(0);
////    ICC_ERROR("invocation start ERROR!!!!!!");
////    return -1;
////  }
//  int retval = _invocation.mTransport->tms()->bindDispatcher (_invocation.requestId(), _invocation.mRd);
//  if (retval == -1) {
////    this->closeConnection ();
//    return -1;
//  }
//  if(_invocation.mTransport->sendRequest(&ref,
//                                   comm,
//                                   *__os,
//                                   false,
//                                   0) == -1) {
//
//    return -1;
//  }


//  Rcver rcver(*comm, myHello);
//  rcver.activate();
//
//  comm->run();

//  while(true) {
//
//    ICC_DEBUG("async calling.......");
//    myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, sync1", u);
//    sleep(10);
//  } // while

  return 0;
}
