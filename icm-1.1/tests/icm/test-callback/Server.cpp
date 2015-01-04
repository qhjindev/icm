


#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"

#include "VmServiceI.h"

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;

  Endpoint endpoint ("TCP", "", 3000);
  ObjectAdapter* oa = comm->createObjectAdapterWithEndpoint ("VmService", &endpoint);
  Object* object = new My::VmServiceI;
  oa->add (object, "VmService");

  comm->run ();

  return 0;
}

