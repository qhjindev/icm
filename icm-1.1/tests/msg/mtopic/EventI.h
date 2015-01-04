#ifndef NETWORK_I_H
#define NETWORK_I_H

#include "Event.h"

class NetworkI : public demo::Network {
public:
  virtual void reportEvent(const ::demo::NetEvent&);
};

class AlarmI : public demo::Alarm {
public:
  AlarmI() {}
  virtual void reportAlarm(const ::demo::AlarmEvent&);

};









#endif //NETWORK_I_H
