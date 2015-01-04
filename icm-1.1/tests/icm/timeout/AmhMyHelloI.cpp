#include <iostream>
#include "AmhMyHelloI.h"

using namespace std;

void demo::AmhMyHelloI::sayHello_async(demo::AMH_MyHello_sayHello* cb, const ::std::string& msg, Short u) {
  std::cout << "recv msg:" << msg << std::endl;
  std::cout << "u:" << u << std::endl;

  HelloContext* context = new HelloContext;
  context->msg = msg;
  context->u = u;
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

    HelloContext* context = (HelloContext*) mb->base();
    static int i = 1;
    OS::sleep(++i % 5);
    //OS::sleep(10);

    string msg = context->msg;
    Short u = context->u;
    demo::AMH_MyHello_sayHello* cb = context->cb;

    std::string ret = "return from ";
    ret += msg;
    u++;
    cout << "responsing, ret = " << ret << ", u = " << u << endl;
    cb->response(ret, u);

    delete context;
    delete mb;
  }

  return 0;
}

