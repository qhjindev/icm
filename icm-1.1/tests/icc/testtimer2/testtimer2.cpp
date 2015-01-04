#include "icc/TimerHeap.h"
#include "icc/TimerQueueAdapter.h"
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

  virtual int handleTimeout(const TimeValue &currentTime, const void *act) {
    LogDebug << "timer " << id << " fired" << endl;
    return 0;
  }

  long id;
};

int main (int argc,char* argv[])
{
  ThreadTimerQueueAdapter<TimerHeap> ttq;
  ttq.activate();

  TimeValue futureTime = OS::gettimeofday() + TimeValue(5);
  TimeValue interval1(1);
  TimeValue interval2(2);
  TimeValue interval3(3);

  LogDebug << "start schedule timer after 5 seconds" << endl;
  TimeoutHandler* timeOutHandler1 = new TimeoutHandler;
  long timer1 = ttq.schedule(timeOutHandler1, 0, futureTime, interval1);
  timeOutHandler1->id = timer1;

  TimeoutHandler* timeOutHandler2 = new TimeoutHandler;
  long timer2 = ttq.schedule(timeOutHandler2, 0, futureTime, interval2);
  timeOutHandler2->id = timer2;

  TimeoutHandler* timeOutHandler3 = new TimeoutHandler;
  long timer3 = ttq.schedule(timeOutHandler3, 0, futureTime, interval3);
  timeOutHandler3->id = timer3;

  LogDebug << "sleep 15 second" << endl;
  sleep(15);

  LogDebug << "start cancel timer, shouldn't has timer after cancel" << endl;
  ttq.cancel(timer1);
  ttq.cancel(timer2);
  ttq.cancel(timer3);

  sleep(5);
  delete timeOutHandler1;
  delete timeOutHandler2;
  delete timeOutHandler3;

  ThreadManager::instance ()->wait ();

  return 0;
}

