
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"

#include "HelloI.h"

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;
  
  Endpoint endpoint ("TCP", "", 3000);
  ObjectAdapter* oa = comm->createObjectAdapterWithEndpoint ("Hello", &endpoint);
  //Object* object = new demo::AmhMyHelloI;
  Object* object = new demo::HelloI;
  oa->add (object, "Hello");

  comm->run ();

  return 0;
}
