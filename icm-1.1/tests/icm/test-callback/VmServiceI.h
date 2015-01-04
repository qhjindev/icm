#ifndef VM_SERVICE_I_H
#define VM_SERVICE_I_H

#include "Progress.h"

namespace My
{

class VmServiceI : public My::VmService
{
public:
  virtual Int setCallback(IcmProxy::My::MyCallback *);

};

}


#endif //VM_SERVICE_I_H
