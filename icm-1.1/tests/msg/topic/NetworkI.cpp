#include <iostream>
#include "NetworkI.h"

using namespace std;

void
NetworkI::reportEvent(const ::demo::NetEvent& netEvent)
{
  cout << "receive network event:" << endl;
  cout << "ip:" << netEvent.ip.length() << " port:" << netEvent.port << " event:" << netEvent.event << endl;
}
