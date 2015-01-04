#include <iostream>
#include <sstream>
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"
#include "Progress.h"
#include "MyCallbackI.h"

using namespace std;

int main(int argc, char* argv[]) {
  Communicator* comm = Communicator::instance();
  if (comm->init(true) == -1)
    return -1;

  Endpoint endpoint ("TCP", "", 0);
  ObjectAdapter* oa = comm->createObjectAdapterWithEndpoint ("Callback", "localhost");
  Object* object = new My::MyCallbackI;
  oa->add (object, "Callback");

  Reference ref(comm, Identity("VmService"), Endpoint("TCP", "127.0.0.1", 3000));
  IcmProxy::My::VmService vmService;
  vmService.setReference(&ref);

  Reference callbackRef(comm, Identity("Callback"), *oa->getEndpoint());
  IcmProxy::My::MyCallback cbProxy;
  cbProxy.setReference(&callbackRef);
  vmService.setCallback(&cbProxy);

  comm->run();

  return 0;
}

