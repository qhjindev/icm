#ifndef AMH_MY_HELLO_I_H
#define AMH_MY_HELLO_I_H

#include "icc/Task.h"
#include "icc/SynchTraits.h"

#include "my-amd.h"

namespace demo
{

class DelayResponse;

class AmhMyHelloI : public demo::MyHello
{
public:

  virtual void sayBye_async(demo::AMH_MyHello_sayBye* , const ::std::string& msg, Short u);

  void set(DelayResponse* delayResponse) {
    this->delayResponse = delayResponse;
  }

private:
  DelayResponse* delayResponse;
};

class Context {
public:
  std::string msg;
  Short u;
  demo::AMH_MyHello_sayBye* cb;
};

class DelayResponse : public Task<MT_SYNCH> {
public:
  virtual int svc(void);
};

}


#endif //AMH_MY_HELLO_I_H
