/*
 * example usage of sync rpc call
 *
 * This example demonstrates:
 *
 * 1) How to configure server ip, port, and sync call timeout threshold
 *      
 * 2) How to invoke sync call and check result;
 *
 */

#include <iostream>
#include "icc/ThreadManager.h"
#include "icm/Communicator.h"
#include "Hello.h"

using namespace std;

int main(int argc, char* argv[]) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  // init the communicator
  Communicator* comm = Communicator::instance();
  if (comm->init() == -1)
    return -1;

  // Identity's arg must correspond to the class name in Hello.idl
  // Endpoind's arg: protocal, ip, port of the server end
  // the last arg, new TimeValue(3), means the timeout threshold of sync call is 3 seconds
  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000), new TimeValue(3));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference(&ref);

  unsigned long u =  1024ul * 1024 * 1024 * 12;
  Long v = 1;

  // sync call
  string helloRet = myHello.sayHello("Hello, sync", u, v);
  if(helloRet == "") {
    cout << "sayHello call error!!" << endl;
  } else {
    cout << "sayHello call success!!" << endl;
    cout << "u: " << u << ", v: " << v << ", return: " << helloRet << endl;
  }

  string byeRet = myHello.sayBye("Bye, sync", v);
  if(byeRet == "") {
    cout << "sayBye call error!!" << endl;
  } else {
    cout << "sayBye call success!!" << endl;
    cout << "v: " << v << ", return: " << byeRet << endl;
  }

  return 0;
}
