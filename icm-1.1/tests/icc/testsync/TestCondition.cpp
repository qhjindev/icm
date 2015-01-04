
#include <iostream>
#include "icc/Thread.h"
#include "icc/ThreadMutex.h"
#include "icc/ConditionThreadMutex.h"
#include "icc/ThreadManager.h"

using namespace std;

class ThreadA: public Thread {
public:
  ThreadA() :  cond(lock) {

  }

  void signal() {
    this->cond.signal();
  }

protected:
  virtual int svc(void) {
    //cond.waitNow(5);
    cond.wait();
    cout << "wait exit" << endl;
    return 0;
  }

  ThreadMutex lock;
  ConditionThreadMutex cond;
};

class ThreadB: public Thread {
public:
  ThreadB(ThreadA* a) {
    this->a = a;
  }

  virtual int svc(void) {
    ::sleep(10);
    cout << "start signal thread a" << endl;
    a->signal();
    return 0;
  }

  ThreadA* a;
};


int main(int argc, char* argv[]) {
  ThreadA* a = new ThreadA;
  ThreadB* b = new ThreadB(a);

  a->activate();
  b->activate();

  ThreadManager::instance()->wait();
  return 0;
}

