#include <iostream>
#include "AmhMyHelloI.h"

using namespace std;

void demo::AmhMyHelloI::sayHello_async(demo::AMH_MyHello_sayHello* cb, const ::std::string& msg, Short u) {
  HelloContext* context = new HelloContext;
  context->msg = msg;
  context->u = u;
  context->cb = cb;

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(HelloContext*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::testSequence_async(demo::AMH_MyHello_testSequence* cb, const ::demo::TestReq& req) {
  TestSequenceContext* context = new TestSequenceContext;
  context->req = req;
  context->cb = cb;

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(HelloContext*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::testDictionary_async(demo::AMH_MyHello_testDictionary* cb, const ::demo::TestReqDict& req) {
  TestDictContext* context = new TestDictContext;
  context->req = req;
  context->cb = cb;

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(HelloContext*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::testCompound_async(demo::AMH_MyHello_testCompound* cb, const ::demo::TestReqCom& req) {
  TestComContext* context = new TestComContext;
  context->req = req;
  context->cb = cb;

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(HelloContext*));
  delayResponse->putq(mb);
}

int demo::DelayResponse::svc() {

  while (1) {
    MessageBlock* mb;
    if (this->getq(mb) == -1)
      return -1;

//    OS::sleep(3);

    Context* context = (Context*) mb->base();
    HelloContext* helloContext = dynamic_cast<HelloContext*>(context);
    TestSequenceContext* testSequenceContext = dynamic_cast<TestSequenceContext*>(context);
    TestDictContext* testDictContext = dynamic_cast<TestDictContext*>(context);
    TestComContext* testComContext = dynamic_cast<TestComContext*>(context);
    if(helloContext != 0) {
      string msg = helloContext->msg;
      Short u = helloContext->u;
      demo::AMH_MyHello_sayHello* cb = helloContext->cb;

      std::cout << "recv msg:" << msg << std::endl;
      std::cout << "u:" << u << std::endl;

      std::string ret = "return from ";
      ret += msg;
      u++;
      cb->response(ret, u);
    } else if(testSequenceContext != 0) {
      testSequenceContext->cb->response(testSequenceContext->req.toString());
    } else if(testDictContext != 0) {
      testDictContext->cb->response(testDictContext->req.toString());
    } else if(testComContext != 0) {
      testComContext->cb->response(testComContext->req.toString());
    } else {
      assert(false);
    }

    delete context;
    delete mb;
  }

  return 0;
}

