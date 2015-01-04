
#include "os/OS.h"
#include "icc/SynchTraits.h"
#include "icc/Task.h"
#include "icc/ThreadManager.h"

class MyTask1 : public Task<MT_SYNCH>
{
public:
  virtual int svc (void)
  {
    OS::sleep (4);

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

