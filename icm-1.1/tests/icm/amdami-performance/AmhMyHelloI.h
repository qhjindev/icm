#ifndef AMH_MY_HELLO_I_H
#define AMH_MY_HELLO_I_H

#include "icc/Task.h"
#include "icc/SynchTraits.h"

#include "Hello.h"

namespace demo
{

class DelayResponse;
class Context;

class AmhMyHelloI : public demo::MyHello
{
public:

  virtual void sayHello_async(demo::AMH_MyHello_sayHello* , const ::std::string& msg, long u);
  virtual void sayBye_async(demo::AMH_MyHello_sayBye*, const ::std::string&);

  void set(DelayResponse* delayResponse) {
    this->delayResponse = delayResponse;
  }

  static void doSayHello(Context* context);
  static void doSayBye(Context* context);
private:
  DelayResponse* delayResponse;
};
class Context {
public:
  typedef void (*DoJob)(Context*);
  DoJob doJob;
  virtual ~Context() = 0;
};

class HelloContext : public Context {
public:
  HelloContext()
  {
    Context::doJob = AmhMyHelloI::doSayHello;
  }
  ~HelloContext(){

  }

  std::string msg;
  long u;
  demo::AMH_MyHello_sayHello* cb;
};

class ByeContext : public Context {
public:
  ByeContext()
  {
    Context::doJob = AmhMyHelloI::doSayBye;
  }
  ~ByeContext(){

  }

  std::string msg;
  demo::AMH_MyHello_sayBye* cb;
};

class DelayResponse : public Task<MT_SYNCH> {
public:
  virtual int svc(void);
};

}


#endif //AMH_MY_HELLO_I_H
