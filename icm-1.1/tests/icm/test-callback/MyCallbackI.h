#ifndef MY_CALLBACK_I_H
#define MY_CALLBACK_I_H

#include "Progress.h"

namespace My
{

class MyCallbackI : public My::MyCallback
{
public:
  virtual void reportProgress(const ::My::OpProgress&);

};

}


#endif //MY_CALLBACK_I_H
