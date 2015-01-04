#include <iostream>
#include "EventI.h"

using namespace std;

void
NetworkI::reportEvent(const ::demo::NetEvent& netEvent)
{
  cout << "receive network event:" << endl;
  cout << "ip:" << netEvent.ip.length() << " port:" << netEvent.port << " event:" << netEvent.event << endl;
}

void
AlarmI::reportAlarm(const ::demo::AlarmEvent& alarmEvent)
{
  cout << "receive alarm event:" << endl;
  cout << "id:" << alarmEvent.id << " image:" << alarmEvent.image << " mds:" << alarmEvent.mds << endl;
}
