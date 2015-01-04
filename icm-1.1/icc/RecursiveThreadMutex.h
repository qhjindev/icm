#ifndef ICC_RECURSIVE_THREAD_MUTEX_H
#define ICC_RECURSIVE_THREAD_MUTEX_H

#include "os/OS_NS_Thread.h"
#include "icc/TimeValue.h"

class RecursiveThreadMutex
{
public:
  RecursiveThreadMutex (const char *name = 0, ACE_mutexattr_t *arg = 0);

  ~RecursiveThreadMutex (void);

  int remove (void);

  int acquire (void);

  int acquire (TimeValue &tv);

  int acquire (TimeValue *tv);

  int tryAcquire (void);

  int acquireRead (void);

  int acquireWrite (void);

  int tryAcquireRead (void);

  int tryAcquireWrite (void);

  int release (void);

  ACE_thread_t getThreadId (void);

  int getNestingLevel (void);

  ACE_recursive_thread_mutex_t &mutex (void);

  ACE_thread_mutex_t &getNestingMutex (void);

protected:
  void setThreadId (ACE_thread_t t);

  ACE_recursive_thread_mutex_t m_lock;

  bool m_removed;

private:
  void operator= (const RecursiveThreadMutex &);
  RecursiveThreadMutex (const RecursiveThreadMutex &);
};

#endif //ICC_RECURSIVE_THREAD_MUTEX_H
