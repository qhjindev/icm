/*
 * example usage of async rpc call
 *
 * This example demonstrates:
 *
 * 1) How to define callback class, and implement the response() callback function
 *
 * 2) How to configure server ip, port
 *      
 * 3) How to invoke async call
 *
 */

#include <iostream>
#include "icc/ThreadManager.h"
#include "icm/Communicator.h"
#include "Hello.h"

using namespace std;

// implement the concrete call back class, reponse() will be invoked after recved server's reply
class AMI_MyHello_sayHelloI: public ::demo::AMI_MyHello_sayHello {
public:

  virtual void response(const string& ret, Long v) {
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }
};

// implement the concrete call back class, reponse() will be invoked after recved server's reply
class AMI_MyHello_sayByeI: public ::demo::AMI_MyHello_sayBye {
public:

  virtual void response(const string& ret, Long v) {
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }
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

  // Identity's arg must correspond to the class name in Hello.idl
  // Endpoind's arg: protocal, ip, port of the server end
  // in case of async call, timeout option is not needed
  Reference ref (comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference (&ref);

  unsigned long u =  1024ul * 1024 * 1024 * 12;

  // invoke async calls
  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async", u);
  myHello.sayBye_async(new AMI_MyHello_sayByeI, "Bye, async");

  // run() will enter a cycle to wait and process server reply, callback classes's response() will be invoked automatically
  // so usually this can be put into a new created thread 
  comm->run();

  return 0;
}
