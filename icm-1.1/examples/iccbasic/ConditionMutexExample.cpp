/*
 * example of Thread synchronization using ConditionMutex
 *
 */

#include "icc/Thread.h"
#include "icc/ThreadMutex.h"
#include "icc/ConditionThreadMutex.h"
#include "icc/ThreadManager.h"
#include "icc/Guard.h"
#include "os/OS_NS_string.h"
#include <string>
#include <iostream>

using namespace std;

ThreadMutex mutex;
ConditionThreadMutex condition(mutex);

class MyThread1 : public Thread {
public:
  virtual int svc(void) {
    cout << "thread1 begin working" << endl;

    // do some job
    OS::sleep(5);

    cout << "thread1 job done, sending signal" << endl;
    condition.signal();
    return 0;
  }
};

class MyThread2 : public Thread {
public:
  virtual int svc(void) {
    cout << "thread2 begin, waiting signal" << endl;
    condition.wait();

    cout << "thread2 received signal, now begin working" << endl;
    return 0;
  }
};


int main() {
  MyThread1 thread1;
  MyThread2 thread2;
  thread1.activate();
  thread2.activate();

  ThreadManager::instance()->wait();

  return 0;
}
