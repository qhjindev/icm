/*
 * example usage of Thread creation and synchronization
 *
 * This example demonstrates:
 *
 * 1) How to create and activate new thread 
 *      
 * 2) How to synchronize resource access among mulitple threads
 * 
 * 3) Use ThreadManager to wait all other threads in a process
 *
 */

#include "icc/Thread.h"
#include "icc/ThreadMutex.h"
#include "icc/ThreadManager.h"
#include "icc/Guard.h"
#include "os/OS_NS_string.h"
#include <string>
#include <iostream>

using namespace std;

class MyThread : public Thread {
public:
  MyThread(int& count) 
  : count_(count)
  { }
 
  virtual int svc(void) {
    for(int i = 0; i < 10; ++i) {
      // count_ reference to outer data which maybe shared by other threads, so any access need synchronization
      // ACE_GUARD macro uses the RAII idiom, the constructor acquire mutex lock, and destructor release it
      ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);

      cout << "in MyThread, now count is: " << count_++ << endl;
    }
    return 0;
  }

  static ThreadMutex& getMutex() { return mutex; }
private:
  static ThreadMutex mutex;

  int& count_;
};

ThreadMutex MyThread::mutex;

int main() {
  int count = 0;

  MyThread myThread(count);
  // really create a new thread, and run MyThread::svc in that thread
  myThread.activate();
  
  for(int i = 0; i < 10; ++i) {
    // use MyThread's static member mutex for synchronization
    ACE_GUARD_RETURN(ThreadMutex, guard, MyThread::getMutex(), -1);

    cout << "in main(), now count is: " << count++ << endl;
  }
 
  // exit of main thread will terminate the process, so must wait all other threads terminated first
  ThreadManager::instance()->wait();

  return 0;
}
