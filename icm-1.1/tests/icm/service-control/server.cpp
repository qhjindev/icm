
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"

#include <iostream>

#include "MyHelloI.h"

using namespace std;

class ControllThread : public Thread {
public:
  explicit ControllThread(Endpoint* endpoint_, Object* object_ = 0)
  : endpoint(endpoint_), object(object_)
  { }

  virtual int svc(void) {
    OS::sleep(10);
    while(true) {
      cout << "stoping...." << endl;
      //Communicator::instance()->stopEndpoint(endpoint); 
      Communicator::instance()->objectAdapter()->remove("MyHello");
      OS::sleep(10);

      cout << "resuming...." << endl;
      //Communicator::instance()->resumeEndpoint(endpoint); 
      Communicator::instance()->objectAdapter()->add(object, "MyHello");
      OS::sleep(10);
    }
    return 0;
  }

private:
  Endpoint* endpoint;
  Object* object;
};

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;
  
  Endpoint endpoint ("TCP", "", 3000);
  ObjectAdapter* oa = comm->createObjectAdapterWithEndpoint ("MyHello", &endpoint);
  Object* object = new demo::MyHelloI;
  oa->add (object, "MyHello");

  ControllThread controllThread(&endpoint, object);
  controllThread.activate();

  comm->run ();

  return 0;
}
