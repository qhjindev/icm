#ifndef AMH_MY_HELLO_I_H
#define AMH_MY_HELLO_I_H

#include "icc/Task.h"
#include "icc/SynchTraits.h"

#include "test.h"

namespace demo
{

class DelayResponse;

class AmhMyHelloI : public demo::MyHello
{
public:

  virtual void sayHello1_async(demo::AMH_MyHello_sayHello1* , const ::demo::Req1& req);
  virtual void sayHello1I_async(demo::AMH_MyHello_sayHello1I* , const ::demo::Req1I& req);
  virtual void sayHello1R_async(::demo::AMH_MyHello_sayHello1R*, const ::demo::Req1R& req);
  virtual void sayHello2_async(::demo::AMH_MyHello_sayHello2*, const ::demo::Req2& req);
  virtual void sayHello3_async(::demo::AMH_MyHello_sayHello3*, const ::demo::Req3& req);
  virtual void sayHello4_async(::demo::AMH_MyHello_sayHello4*, const ::demo::Req4& req);

  void set(DelayResponse* delayResponse) {
    this->delayResponse = delayResponse;
  }

private:
  DelayResponse* delayResponse;
};

class Context {
public:
  virtual ~Context() {}
};

class Hello1Context : public Context {
public:
  demo::Req1 req;
  demo::AMH_MyHello_sayHello1* cb;
};

class Hello1IContext : public Context {
public:
  demo::Req1I req;
  demo::AMH_MyHello_sayHello1I* cb;
};

class Hello1RContext : public Context {
public:
  demo::Req1R req;
  demo::AMH_MyHello_sayHello1R* cb;
};

class Hello2Context : public Context {
public:
  demo::Req2 req;
  demo::AMH_MyHello_sayHello2* cb;
};

class Hello3Context : public Context {
public:
  demo::Req3 req;
  demo::AMH_MyHello_sayHello3* cb;
};

class Hello4Context : public Context {
public:
  demo::Req4 req;
  demo::AMH_MyHello_sayHello4* cb;
};

class DelayResponse : public Task<MT_SYNCH> {
public:
  virtual int svc(void);
};

}


#endif //AMH_MY_HELLO_I_H
