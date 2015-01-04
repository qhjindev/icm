#ifndef MY_HELLO_I_H
#define MY_HELLO_I_H

#include "my.h"

namespace demo
{

class MyHelloI : public demo::MyHello
{
public:
  virtual std::string sayHello(const ::std::string&, Short, Long&);

};

}


#endif //MY_HELLO_I_H
