
#include "icc/Log.h"
#include "icc/SelectReactor.h"
#include "icc/Reactor.h"
#include "icc/Task.h"
#include "os/OS_NS_string.h"
#include "icc/ThreadManager.h"
#include <string>
#include <iostream>

using namespace std;


class TimeoutHandler : public EventHandler {
public:
  // called by reactor thread when timer expired
  virtual int handleTimeout(const TimeValue &currentTime, const void *act) {
    LogMsg << "handleTimeout called" << endl;
    return 0;
  }
};

class MyTask1 : public Task<MT_SYNCH>
{
public:
  virtual int svc (void)
  {
    TimeValue tv1(0);
    TimeValue tv2(5);
    TimeValue tv3(10);

    TimeoutHandler* timeOutHandler = new TimeoutHandler;
    if(Reactor::instance()->scheduleTimer(timeOutHandler, this, tv1, tv2) == -1) {
      cerr << "scheduleTimer1() failed!! \n";
      return -1;
    }

    TimeoutHandler* timeOutHandler2 = new TimeoutHandler;
    if(Reactor::instance()->scheduleTimer(timeOutHandler2, this, tv1, tv3) == -1) {
      cerr << "scheduleTimer2() failed!! \n";
      return -1;
    }

    Reactor::instance()->runReactorEventLoop();
    return 0;
  }
};

int main (int argc,char* argv[])
{
  MyTask1 task;
  task.activate ();

  ThreadManager::instance ()->wait ();

  return 0;
}

