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
  virtual void testSequence_async(demo::AMH_MyHello_testSequence* , const ::demo::TestReq& req);
  virtual void testDictionary_async(::demo::AMH_MyHello_testDictionary*, const ::demo::TestReqDict& req);
  virtual void testCompound_async(::demo::AMH_MyHello_testCompound*, const ::demo::TestReqCom& req);

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

class HelloContext : public Context {
public:
  std::string msg;
  Short u;
  demo::AMH_MyHello_sayHello* cb;
};

class TestSequenceContext : public Context {
public:
  demo::TestReq req;
  demo::AMH_MyHello_testSequence* cb;
};

class TestDictContext : public Context {
public:
  demo::TestReqDict req;
  demo::AMH_MyHello_testDictionary* cb;
};

class TestComContext : public Context {
public:
  demo::TestReqCom req;
  demo::AMH_MyHello_testCompound* cb;
};

class DelayResponse : public Task<MT_SYNCH> {
public:
  virtual int svc(void);
};

}


#endif //AMH_MY_HELLO_I_H
