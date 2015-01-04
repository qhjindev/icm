
#include <iostream>
#include <sstream>
#include "icm/Communicator.h"
#include "icm/ResourceFactory.h"
#include "my.h"

using namespace std;

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init (true) == -1)
    return -1;

  Reference ref (comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference (&ref);

  for (int i = 0; i < 10000; i++) {
    Short u = 10 + i;
    Long v = 1000 + i;
    ostringstream oss;
    for(int j=0; j < 1000; j++) {
      oss << "abcdefghijklmnopqrstuvwxyz";
    }
    string ibigstr = oss.str();
    string ret = myHello.sayHello (ibigstr, u, v);
    if (ret != "") {
      std::cout << "ret:" << ret << std::endl;
    } else {
      //err process
    }
  }

  return 0;
}
