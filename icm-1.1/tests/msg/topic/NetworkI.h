#ifndef NETWORK_I_H
#define NETWORK_I_H

#include "Network.h"

class NetworkI : public demo::Network {
public:
  virtual void reportEvent(const ::demo::NetEvent&);
};











#endif //NETWORK_I_H
