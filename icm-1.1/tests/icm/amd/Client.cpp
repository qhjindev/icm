
#include <iostream>
using namespace std;

#include "icm/Communicator.h"
#include "my-amd.h"

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;

  Reference ref (comm, Identity("Hello"), Endpoint("TCP", "127.0.0.1", 3000));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference (&ref);

  Short u = 12;
  Long v;
  string ret = myHello.sayBye("Bye,world", u, v);
  cout << "u:" << u << " v:" << v << " ret:" << ret << endl;

/*
  std::string msg = "first message";
  Long v;
  myHello.sayBye (msg, u, v);
*/

  //comm->run ();

  return 0;
}
