
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"
#include "icm/ServantLocator.h"
#include "Hello.h"

using namespace Demo;

class MyHelloI : public MyHello
{
public:
  std::string sayHello (Int i) 
  {
    return "hello,world";
  }
};

class ServantLocatorI : public ServantLocator
{
public:
  virtual Object* locate(const ServerRequest& serverRequest, void* cookie)
  {
    return new MyHelloI;
  }

  virtual void finished(const ServerRequest& serverRequest, const Object*& servant, void* cookie)
  {

  }

  virtual void deactivate(const ::std::string&)
  {

  }
};


int main()
{
  Communicator comm;
  if (comm.init () == -1)
    return -1;
  
  Endpoint endpoint ("TCP", "", 3000);
  ObjectAdapter* oa = comm.createObjectAdapterWithEndpoint ("MyHello", &endpoint);

  ServantLocatorI* locator = new ServantLocatorI;
  oa->addServantLocator (locator, "h");

  comm.run ();

  return 0;
}
