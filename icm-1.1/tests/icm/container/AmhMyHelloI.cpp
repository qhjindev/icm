#include <iostream>
#include "AmhMyHelloI.h"

using namespace std;

void demo::AmhMyHelloI::sayHello1_async(demo::AMH_MyHello_sayHello1* cb, const ::demo::Req1& req) {
  Hello1Context* context = new Hello1Context;
  context->req = req;
  context->cb = cb;

  ICC_DEBUG("req1: %s", demo::__toString(req).c_str());

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::sayHello1I_async(demo::AMH_MyHello_sayHello1I* cb, const ::demo::Req1I& req) {
  Hello1IContext* context = new Hello1IContext;
  context->req = req;
  context->cb = cb;

  ICC_DEBUG("req1I: %s", demo::__toString(req).c_str());

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::sayHello1R_async(demo::AMH_MyHello_sayHello1R* cb, const ::demo::Req1R& req) {
  Hello1RContext* context = new Hello1RContext;
  context->req = req;
  context->cb = cb;

  ICC_DEBUG("req1R: %s", demo::__toString(req).c_str());

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::sayHello2_async(demo::AMH_MyHello_sayHello2* cb, const ::demo::Req2& req) {
  Hello2Context* context = new Hello2Context;
  context->req = req;
  context->cb = cb;

  ICC_DEBUG("req2: %s", demo::__toString(req).c_str());

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::sayHello3_async(demo::AMH_MyHello_sayHello3* cb, const ::demo::Req3& req) {
  Hello3Context* context = new Hello3Context;
  context->req = req;
  context->cb = cb;

  ICC_DEBUG("req3: %s", demo::__toString(req).c_str());

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::sayHello4_async(demo::AMH_MyHello_sayHello4* cb, const ::demo::Req4& req) {
  Hello4Context* context = new Hello4Context;
  context->req = req;
  context->cb = cb;

  ICC_DEBUG("req4: %s", demo::__toString(req).c_str());

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

int demo::DelayResponse::svc() {

  while (1) {
    MessageBlock* mb;
    if (this->getq(mb) == -1)
      return -1;

//    OS::sleep(3);

    Context* context = (Context*) mb->base();
    Hello1Context* hello1Context = dynamic_cast<Hello1Context*>(context);
    Hello1IContext* hello1IContext = dynamic_cast<Hello1IContext*>(context);
    Hello1RContext* hello1RContext = dynamic_cast<Hello1RContext*>(context);
    Hello2Context* hello2Context = dynamic_cast<Hello2Context*>(context);
    Hello3Context* hello3Context = dynamic_cast<Hello3Context*>(context);
    Hello4Context* hello4Context = dynamic_cast<Hello4Context*>(context);
    if(hello1Context != 0) {
      hello1Context->cb->response(demo::__toString(hello1Context->req));
    } else if(hello1IContext != 0) {
      hello1IContext->cb->response(demo::__toString(hello1IContext->req));
    } else if(hello1RContext != 0) {
      hello1RContext->cb->response(demo::__toString(hello1RContext->req));
    } else if(hello2Context != 0) {
      hello2Context->cb->response(demo::__toString(hello2Context->req));
    } else if(hello3Context != 0) {
      hello3Context->cb->response(demo::__toString(hello3Context->req));
    } else if(hello4Context != 0) {
      hello4Context->cb->response(demo::__toString(hello4Context->req));
    } else {
      assert(false);
    }

    delete context;
    delete mb;
  }

  return 0;
}

