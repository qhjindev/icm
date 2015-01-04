
#include <iostream>
using namespace std;

#include "icm/Communicator.h"
#include "Hello.h"

class AMI_Hello_sayByeI : public ::demo::AMI_Hello_sayBye
{
public:

  virtual void response (const ::std::string& ret, Long v)
  {
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }

};

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;

  Reference ref (comm, Identity("Hello"), Endpoint("TCP", "127.0.0.1", 3000));

  IcmProxy::demo::Hello myHello;
  myHello.setReference (&ref);

  Short u = 12;
  myHello.sayBye_async (new AMI_Hello_sayByeI, "Bye,world!", u);

/*
  std::string msg = "first message";
  Long v;
  myHello.sayBye (msg, u, v);
*/

  comm->run ();

  return 0;
}
