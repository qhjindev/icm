
#include <iostream>
#include "icm/Communicator.h"
#include "Hello.h"

int
main (int argc, char* argv[])
{
  Communicator comm;
  if (comm.init () == -1)
    return -1;

  Reference ref (&comm, "MyHello", "h", "TCP", "127.0.0.1", 3000);

  for (int k = 0; k < 5; k++) {

    IcmProxy::Demo::MyHello myHello;
    myHello.setup (&ref);

    Int i = 50;
    std::string ret = myHello.sayHello(i);
    std::cout << "ret:" << ret << std::endl;
  }

  return 0;
}
