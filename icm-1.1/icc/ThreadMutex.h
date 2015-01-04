#ifndef _ICC_THREAD_MUTEX_H_
#define _ICC_THREAD_MUTEX_H_

//#include "os/config-all.h"
#include "os/OS_NS_Thread.h"

class ThreadMutex
{
public:
  ThreadMutex (const char *name = 0,
               ACE_mutexattr_t *attributes = 0);

  ~ThreadMutex (void);

  int remove (void);

  int acquire (void);

  //int acquire (TimeValue &tv);

  //int acquire (TimeValue *tv);

  int tryAcquire (void);

  int release (void);

  int acquireRead (void);

  int acquireWrite (void);

  int tryAcquireRead (void);

  int tryAcquireWrite (void);

  //int tryacqurieWriteUpgrade (void);

  const ACE_thread_mutex_t &getLock (void) const;

  //void dump (void) const;

  ACE_thread_mutex_t lock;

  bool removed;

private:

  void operator= (const ThreadMutex &);

  ThreadMutex (const ThreadMutex &);
};





#endif //_ICC_THREAD_MUTEX_H_
