/*
 * example implementation of an amd(async message dispatch) server
 *
 * This example demonstrates:
 *
 * 1) How to implement the sync request process class, which just recv and put requet into somewhere,
 *    for the async processor to retrieve and process asynclly
 *
 * 2) How to implement the async processor
 *      
 * 3) How to enable/disable all services or a specific service
 *
 */

#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"
#include "icc/Task.h"
#include <sstream>
#include <iostream>

#include "Hello.h"

using namespace std;

namespace demo
{

// define an abstract class Context, after recved client request, a corresponding concrete Contex 
// will be created, and it's pointer is put into a message queue for another thread to process asyncly.
// the doJob() will be called when that processer thread retrieved the pointer from the queue
class Context {
public:
  virtual void doJob() = 0;
};

class HelloContext : public Context {
public:
  void doJob() {
    cout << "recv msg:" << msg << std::endl;
    cout << "u:" << u << std::endl;

    string ret = "return from ";
    ret += msg;

    // send back response message to client
    cb->response(ret, ++u);
  }

  string msg;
  Long u;
  demo::AMH_MyHello_sayHello* cb;
};

class ByeContext : public Context {
public:
  void doJob() {
    static Long v = 0;
    cout << "recv msg:" << msg << std::endl;

    string ret = "return from ";
    ret += msg;

    // send back response message to client
    cb->response(ret, v++);
  }

  string msg;
  demo::AMH_MyHello_sayBye* cb;
};

// the class Task is derived from Thread, added a thread-safe message queue
class AsyncProcessor : public Task<MT_SYNCH> {
public:
  virtual int svc(void) {
    // infinitely get message from the queue, and callback the doJob() function
    while (true) {
      MessageBlock* mb;
      if (this->getq(mb) == -1)
        return -1;

      Context* context = (Context*) mb->base();

      context->doJob();
      delete context;
    }

    return 0;
  }
};

// implement the concrete class derived from demo::MyHello, the sayHello_async() and sayBye_async() 
// create a Context which holds all request info, and put it into the AsyncProcessor task's message queue
class MyHelloI : public demo::MyHello
{
public:
  explicit MyHelloI(AsyncProcessor* asyncProcessor)
  :asyncProcessor_(asyncProcessor)
  { }

  virtual void sayHello_async(demo::AMH_MyHello_sayHello* cb, const ::std::string& msg, Long u) {
    HelloContext* context = new HelloContext;
    context->msg = msg;
    context->u = u;
    context->cb = cb;

    MessageBlock* mb = new MessageBlock;
    mb->base((char*) context, sizeof(Context*));
    asyncProcessor_->putq(mb);
  }
 
  virtual void sayBye_async(demo::AMH_MyHello_sayBye* cb, const ::std::string& msg) {
    ByeContext* context = new ByeContext;
    context->msg = msg;
    context->cb = cb;

    MessageBlock* mb = new MessageBlock;
    mb->base((char*) context, sizeof(Context*));
    asyncProcessor_->putq(mb);
  } 

private:
  AsyncProcessor* asyncProcessor_;
};

} // namespace demo

// use an extra thread to control service
// stop/resume endpoint is used at lower network level, to disable/enable the listen port, all services are affected 
// remove/add is used in at a higher and finer level, to disable/enable one specific service class
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

  // configure the listen port
  Endpoint endpoint ("TCP", "", 3000);
  ObjectAdapter* oa = comm->createObjectAdapterWithEndpoint ("MyHello", &endpoint);

  demo::AsyncProcessor* task = new demo::AsyncProcessor;
  Object*  object = new demo::MyHelloI(task);
  oa->add (object, "MyHello");

  task->activate();

  ControllThread controllThread(&endpoint, object);
  controllThread.activate();

  // wait and process client request infinitely
  comm->run ();

  delete task;
 
  return 0;
}
