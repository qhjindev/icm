#include <iostream>
using namespace std;

#include "icm/Communicator.h"
#include "Hello.h"

class AMI_MyHello_sayHelloI: public ::demo::AMI_MyHello_sayHello {
public:
  virtual void response(const ::std::string& ret, Long v) {
    cout << "return:" << ret << endl;
    cout << "v:" << v << endl;
  }
};

class AMI_MyHello_testSequenceI: public ::demo::AMI_MyHello_testSequence {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_testDictionaryI: public ::demo::AMI_MyHello_testDictionary {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AMI_MyHello_testCompoundI: public ::demo::AMI_MyHello_testCompound {
public:
  virtual void response(const ::std::string& ret) {
    ICC_DEBUG("return:\n%s", ret.c_str());
  }
};

class AsyncCaller: public Thread {
public:
  AsyncCaller(IcmProxy::demo::MyHello& hello, const demo::TestReq& request, const demo::TestReqDict& requestD, const demo::TestReqCom& requestC)
  : myHello(hello), req(request), reqD(requestD), reqC(requestC)
  {}

  virtual int svc(void) {
    sleep(2);
//    ICC_DEBUG("calling comm->run().......");
//    return communicator.run();
    while(true) {
      ICC_DEBUG("async calling sayHello.......");
      myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, sync1", 12);
      ICC_DEBUG("async calling testSequence.......");
      myHello.testSequence_async(new AMI_MyHello_testSequenceI, req);
      ICC_DEBUG("async calling testDictionary.......");
      myHello.testDictionary_async(new AMI_MyHello_testDictionaryI, reqD);
      ICC_DEBUG("async calling testCompound.......");
      myHello.testCompound_async(new AMI_MyHello_testCompoundI, reqC);
      sleep(3);
    } // while
    return 0;
  }
private:
  IcmProxy::demo::MyHello& myHello;
  const demo::TestReq& req;
  const demo::TestReqDict& reqD;
  const demo::TestReqCom& reqC;
};

int main(int argc, char* argv[]) {
  Communicator* comm = Communicator::instance();
  if (comm->init() == -1)
    return -1;

  //Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "127.0.0.1", 3000));
  Reference ref(comm, Identity("MyHello"), Endpoint("TCP", "172.16.10.23", 3000));
  IcmProxy::demo::MyHello myHello;
  myHello.setReference(&ref);

//  Short u = 12;
//  Long v;
//
//  ICC_DEBUG("sync calling.......");
//  string ret = myHello.sayHello("Hello, sync1", u, v);
//  cout << "u:" << u << " v:" << v << " ret:" << ret << endl;
//
//  ICC_DEBUG("sync calling.......");
//  ret = myHello.sayHello("Hello, sync2", u, v);
//  cout << "u:" << u << " v:" << v << " ret:" << ret << endl;
//
//  ICC_DEBUG("async calling.......");
//  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async1", u);
//
//  ICC_DEBUG("async calling.......");
//  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async2", u);
//
//  ICC_DEBUG("async calling.......");
//  myHello.sayHello_async(new AMI_MyHello_sayHelloI, "Hello, async3", u);


  demo::TestReq req;
  demo::S1 ss1;
  ss1.id = 1;
  ss1.name = "name1";
  demo::S1 ss2;
  ss2.id = 2;
  ss2.name = "name2";
  req.syn = 1;
  req.reqs1.push_back(ss1);
  req.reqs1.push_back(ss2);

  demo::TestReqDict reqD;
  reqD.syn = 2;
  reqD.dict1.insert(make_pair(1, ss1));
  reqD.dict1.insert(make_pair(2, ss2));


  demo::TestReqCom reqC;
  demo::Reqs reqs1;
  reqs1.push_back(ss1);
  reqs1.push_back(ss2);
  demo::Dicta dicta1;
  dicta1.insert(make_pair("no1", reqs1));
  demo::S1 ss3;
  ss3.id = 3;
  ss3.name = "name3";
  demo::S1 ss4;
  ss4.id = 4;
  ss4.name = "name4";
  demo::Reqs reqs2;
  reqs2.push_back(ss3);
  reqs2.push_back(ss4);
  dicta1.insert(make_pair("no2", reqs2));

  demo::Reqs reqs3;
  reqs3.push_back(ss1);
  reqs3.push_back(ss2);
  demo::Dicta dicta2;
  dicta2.insert(make_pair("no3", reqs3));
  demo::Reqs reqs4;
  reqs4.push_back(ss3);
  reqs4.push_back(ss4);
  dicta2.insert(make_pair("no4", reqs4));

  reqC.syn = 3;
  reqC.com1.push_back(dicta1);
  reqC.com1.push_back(dicta2);

//  ICC_DEBUG("req:\n%s", req.toString().c_str());

//  ICC_DEBUG("sync calling.......");
//  string ret = myHello.testSequence(req);
//  ICC_DEBUG("return:\n%s", ret.c_str());

  AsyncCaller caller(myHello, req, reqD, reqC);
  caller.activate();
//  ICC_DEBUG("async calling.......");
//  myHello.testSequence_async(new AMI_MyHello_testSequenceI, req);

  comm->run();

  return 0;
}
