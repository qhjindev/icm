
#include "os/OS.h"
#include "icc/SynchTraits.h"
#include "icc/Thread.h"
#include "icc/ThreadManager.h"

class MyThread1 : public Thread
{
public:
  virtual int svc (void)
  {
    printf("I'm happy!");
    //OS::sleep (4);

    return 0;
  }
};

int main (int argc,char* argv[])
{
  MyThread1* myThread = new MyThread1;
  myThread->activate();

  TimeValue tv(5);
  ThreadManager::instance ()->wait (&tv);

  MyThread1* myThread2 = new MyThread1;
  myThread2->activate();

  ThreadManager::instance ()->wait ();

  return 0;
}

