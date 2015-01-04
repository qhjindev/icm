
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"

#include "AmhMyHelloI.h"

int
main (int argc, char* argv[])
{
  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;

  demo::DelayResponse* task = new demo::DelayResponse;

  Endpoint endpoint ("TCP", "", 3000);
  ObjectAdapter* oa = comm->createObjectAdapterWithEndpoint ("MyHello", &endpoint);

  demo::AmhMyHelloI*  amh = new demo::AmhMyHelloI;
  amh->set(task);
  oa->add (amh, "MyHello");

  task->activate();

  comm->run ();

  delete task;

  return 0;
}
