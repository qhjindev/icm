
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

  for (int i = 0; i < 10; i++) {
    Short u = 10 + i;
    Long v = 1000 + i;
    std::ostringstream ss;
    ss << "hello, world from " << i;
    string ret = myHello.sayHello (ss.str(), u, v);
	if ( IcmProxy::IsCallSuccess() ) { 
		std::cout<<"call success. errno:"<<errno<<std::endl;
	} else {
		std::cout<<"call failed. errno:"<<errno<<std::endl;
	}
    if (ret != "") {
      std::cout << "ret:" << ret << std::endl;
    } else {
      //err process
    }
  }

  return 0;
}
