#include <iostream>
#include <string>
#include "AmhMyHelloI.h"
#include <icc/Log.h>

using namespace std;

demo::Context::~Context() {
}
static int reqs(0);
void demo::AmhMyHelloI::sayHello_async(demo::AMH_MyHello_sayHello* cb, const ::std::string& msg, long u) {
  HelloContext* context = new HelloContext;
  context->msg = msg;
  context->u = u;
  context->cb = cb;
//  ICC_DEBUG("uuuu: %lu", u);
//  cout << "######  uuuu:" << u << endl;
//  printf("***********  uuuu: %lu\n", u);
  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(char*));
  if(++reqs % 100000 == 0)
    ICC_DEBUG("total recved events: %d", reqs);

  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::doSayHello(Context* context) {
//  OS::sleep(3);
  HelloContext* ctx = dynamic_cast<HelloContext*>(context);
  assert(ctx != 0);

  string msg = ctx->msg;
  static long v = 0;
  demo::AMH_MyHello_sayHello* cb = ctx->cb;

//  ICC_DEBUG("recv msg: %s, u: %lu", msg.c_str(), u);

  string ret = "return from ";
  ret += msg;
  cb->response(ret, v++ + ctx->u);
}

void demo::AmhMyHelloI::sayBye_async(demo::AMH_MyHello_sayBye* cb, const ::std::string& msg) {
  ByeContext* context = new ByeContext;
  context->msg = msg;
  context->cb = cb;

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(char*));

  if(++reqs % 100000 == 0)
    ICC_DEBUG("total recved events: %d", reqs);
  delayResponse->putq(mb);
}

void demo::AmhMyHelloI::doSayBye(Context* context) {
//  OS::sleep(1);
  static int v = 0;
  ByeContext* ctx = dynamic_cast<ByeContext*>(context);
  assert(ctx != 0);

  string msg = ctx->msg;
  demo::AMH_MyHello_sayBye* cb = ctx->cb;

//  ICC_DEBUG("recv msg: %s", msg.c_str());

  string ret = "return from ";
  ret += msg;
  cb->response(ret,++v);
}

int demo::DelayResponse::svc() {

  while (1) {
    MessageBlock* mb;
    if (this->getq(mb) == -1)
      return -1;

    Context* context = (Context*) mb->base();
    context->doJob(context);

    delete context;
    delete mb;
  }

  return 0;
}

