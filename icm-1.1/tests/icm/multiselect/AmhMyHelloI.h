#ifndef AMH_MY_HELLO_I_H
#define AMH_MY_HELLO_I_H

#include "icc/Task.h"
#include "icc/SynchTraits.h"

#include "Hello.h"

namespace demo
{

class DelayResponse;

class AmhMyHelloI : public demo::MyHello
{
public:

  virtual void sayHello_async(demo::AMH_MyHello_sayHello* , const ::std::string& msg, Short u);

  void set(DelayResponse* delayResponse) {
    this->delayResponse = delayResponse;
  }

private:
  DelayResponse* delayResponse;
};

class HelloContext {
public:
  std::string msg;
  Short u;
  demo::AMH_MyHello_sayHello* cb;
};

class DelayResponse : public Task<MT_SYNCH> {
public:
  virtual int svc(void);
};

}


#endif //AMH_MY_HELLO_I_H
