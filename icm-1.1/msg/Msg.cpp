#include "Msg.h"
#include "icm/IcmStream.h"

Msg::Msg() {
  messageType = Msg_Unknow;
  messageSize = 0;
  currentOffset = 0;
  creatTime = time(NULL);
}

Msg::~Msg() {
}

//Msg::Msg()
//: is(0)
//{
//}
//
//Msg::~Msg() {
//  if(is != NULL)
//    delete is;
//}
