#include <iostream>
#include "AmhMyHelloI.h"

using namespace std;

void demo::AmhMyHelloI::sayBye_async(demo::AMH_MyHello_sayBye* cb, const ::std::string& msg, Short u) {
  Context* context = new Context;
  context->msg = msg;
  context->u = u;
  context->cb = cb;

  MessageBlock* mb = new MessageBlock;
  mb->base((char*) context, sizeof(Context*));
  delayResponse->putq(mb);
}

int demo::DelayResponse::svc() {

  while (1) {
    MessageBlock* mb;
    if (this->getq(mb) == -1)
      return -1;

    Context* context = (Context*) mb->base();
    OS::sleep(15);

    string msg = context->msg;
    Short u = context->u;
    demo::AMH_MyHello_sayBye* cb = context->cb;

    std::cout << "recv msg:" << msg << std::endl;
    std::cout << "u:" << u << std::endl;

    std::string ret = "return from ";
    ret += msg;
    u++;
    cb->response(ret, u);

    delete context;
  }

  return 0;
}

