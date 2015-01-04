#ifndef HELLO_I_H
#define HELLO_I_H

#include "Hello.h"


namespace demo
{

class HelloI : virtual public Hello
{
public:

  HelloI() {}

  virtual ::std::string sayHello(const ::std::string&, Short, Long&);
  virtual ::std::string sayBye(const ::std::string&, Short, Long&);

};

}



#endif //HELLO_I_H
