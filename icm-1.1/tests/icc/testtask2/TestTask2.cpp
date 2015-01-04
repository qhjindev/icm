
#include "os/OS.h"
#include "icc/SynchTraits.h"
#include "icc/Task.h"
#include "icc/ThreadManager.h"
#include "icc/MessageBlock.h"
#include "icc/Log.h"
#include <iostream>

using namespace std;

class MyTask1 : public Task<MT_SYNCH>
{
public:

  virtual int svc (void)
  {
    while (1) {
      MessageBlock* mb;
      if(this->getq (mb) == -1)
        break;

      const char *db = mb->base ();
      int m = *((int *)db);
      //cout << "recv data: " << m << endl;
      LogMsg << "recv data: " << m << endl;

      delete db;
      delete mb;
    }

    return 0;
  }

};

class MyTask2 : public Task <MT_SYNCH>
{
public:
  MyTask2 (Task <MT_SYNCH> &task)
  {
    this->peerTask = task;
  }

  virtual int svc (void)
  {
/*
    char *buffer = new char[64];
    memset (buffer, 0, 64);
    strcpy (buffer, "hello,world");

    MessageBlock *mb = new MessageBlock;
    mb->setDataBlock (buffer);
    this->peerTask.putq (mb);
*/
    for (int i = 1000; i < 1020; i++) {
      int* m = new int;
      *m = i;
      MessageBlock *mb = new MessageBlock;
      mb->base ((char *)m, sizeof(int));
      this->peerTask.putq (mb);
      LogMsg << "send data: " << i << endl;
      OS::sleep (1);
    }

    return 0;
  }

  Task<MT_SYNCH> peerTask;
};

int main (int argc,char* argv[])
{
  Log::instance() ->open("log.txt");

  LogMsg << "start main " << endl;

  MyTask1 task1;
  MyTask2 task2 (task1);
  task1.activate ();
  task2.activate ();

  ThreadManager::instance ()->wait ();

  return 0;
}

