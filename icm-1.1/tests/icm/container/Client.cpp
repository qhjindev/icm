#include <iostream>
using namespace std;

#include "icm/Communicator.h"
#include "test.h"

class AMI_MyHello_sayHello1I: public ::demo::AMI_MyHello_sayHello1 {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_sayHello1II: public ::demo::AMI_MyHello_sayHello1I {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_sayHello1RI: public ::demo::AMI_MyHello_sayHello1R {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_sayHello2I: public ::demo::AMI_MyHello_sayHello2 {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_sayHello3I: public ::demo::AMI_MyHello_sayHello3 {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_sayHello4I: public ::demo::AMI_MyHello_sayHello4 {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AsyncCaller: public Thread {
public:
  AsyncCaller(IcmProxy::demo::MyHello& hello,
      const demo::Req1& req1_, const demo::Req1I& req1I_, const demo::Req1R& req1R_, const ::demo::Req2& req2_, const ::demo::Req3& req3_, const ::demo::Req4& req4_)
  : myHello(hello), req1(req1_), req1I(req1I_), req1R(req1R_), req2(req2_), req3(req3_), req4(req4_)
  {}

  virtual int svc(void) {
    //sleep(2);
//    ICC_DEBUG("calling comm->run().......");
//    return communicator.run();
    while(true) {
      ICC_DEBUG("async calling sayHello1.......");
      myHello.sayHello1_async(new AMI_MyHello_sayHello1I, req1);
      ICC_DEBUG("async calling sayHello1I.......");
      myHello.sayHello1I_async(new AMI_MyHello_sayHello1II, req1I);
      ICC_DEBUG("async calling sayHello1R.......");
      myHello.sayHello1R_async(new AMI_MyHello_sayHello1RI, req1R);
      ICC_DEBUG("async calling sayHello2.......");
      myHello.sayHello2_async(new AMI_MyHello_sayHello2I, req2);
      ICC_DEBUG("async calling sayHello3.......");
      myHello.sayHello3_async(new AMI_MyHello_sayHello3I, req3);
      ICC_DEBUG("async calling sayHello4.......");
      myHello.sayHello4_async(new AMI_MyHello_sayHello4I, req4);
      sleep(30);
    } // while
    return 0;
  }
private:
  IcmProxy::demo::MyHello& myHello;
  const ::demo::Req1& req1;
  const ::demo::Req1I& req1I;
  const ::demo::Req1R& req1R;
  const ::demo::Req2& req2;
  const ::demo::Req3& req3;
  const ::demo::Req4& req4;
};

int main(int argc, char* argv[]) {
  Communicator* comm = Communicator::instance();
  if (comm->init() == -1)
    return -1;

  //Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.30.2.70", 3000));
  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.16.10.32", 3000));
  //Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.16.11.148", 3000));
//  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.16.10.23", 3000));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference(&ref);

  demo::Req1 req1;
  req1.push_back("hello1");
  req1.push_back("hello11");
  req1.push_back("hello111");

  demo::Req1I req1I;
  req1I.push_back(1);
  req1I.push_back(2);
  req1I.push_back(3);

//  ICC_DEBUG("req1I: %s", demo::__toString(req1I).c_str());
//  return 0;

  demo::Req3 req3;
  req3.syn = 100;
  req3.request1 = req1;

  demo::Req1 req11;
  req11.push_back("hello2");
  req11.push_back("hello22");
  req11.push_back("hello222");

  demo::Req3 req31;
  req31.syn = 101;
  req31.request1 = req11;

  demo::Req1R req1R;
  req1R.push_back(req3);
  req1R.push_back(req31);

  demo::Req2 req2;
  req2.insert(make_pair(1, "bye1"));
  req2.insert(make_pair(2, "bye11"));
  req2.insert(make_pair(3, "bye111"));
  req2.insert(make_pair(4, "bye1111"));

  demo::Req4 req41;
  req41.syn = 100;
  req41.request1 = req1;
  req41.syn2 = 202;
  demo::Req4 req42;
  req42.syn = 101;
  req42.request1 = req11;
  req42.syn2 = 303;
//  ICC_DEBUG("req:\n%s", req.toString().c_str());

//  ICC_DEBUG("sync calling.......");
//  string ret = myHello.testSequence(req);
//  ICC_DEBUG("return:\n%s", ret.c_str());

  AsyncCaller caller(myHello, req1, req1I, req1R, req2, req3, req41);
  caller.activate();
//  ICC_DEBUG("async calling.......");
//  myHello.testSequence_async(new AMI_MyHello_testSequenceI, req);

  comm->run();

  return 0;
}
