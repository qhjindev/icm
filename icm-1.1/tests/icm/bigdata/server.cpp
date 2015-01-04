
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"

//#include "my.h"
#include "MyHelloI.h"

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

  comm->run ();

  return 0;
}
