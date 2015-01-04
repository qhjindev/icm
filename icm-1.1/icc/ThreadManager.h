#ifndef ICC_THREAD_MANAGER_H
#define ICC_THREAD_MANAGER_H

#include "os/OS.h"
//#include "os/OS_NS_Thread.h"
#include "icc/ThreadMutex.h"
#include "icc/ConditionThreadMutex.h"

#include <list>

class ThreadManager;

class ThreadDescriptor
{
  friend class ThreadManager;

public:
  ThreadDescriptor (void);

  ~ThreadDescriptor (void);

  void terminate ();

protected:
  ACE_thread_t m_thrId;

  ACE_hthread_t m_thrHandle;

  int m_flags;

  bool m_terminated;

  ThreadManager *m_tm;
};

class ThreadManager
{
public:
  friend class ThreadDescriptor;

  ThreadManager ();

  ~ThreadManager ();

  static ThreadManager *instance (void);

  static ThreadManager *instance (ThreadManager *);

  int spawn (ACE_THR_FUNC func, 
             void *arg = 0,
             long flags = THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED);

  int wait (const TimeValue *timeout = 0);

  int wait (const int seconds);

  int join (ACE_thread_t tid, ACE_THR_FUNC_RETURN *status = 0);

  int exit ();

  ThreadDescriptor * findThread (ACE_thread_t tid);

  ThreadDescriptor *findHThread (ACE_hthread_t hid);

protected:

  int registerAsTerminated (ThreadDescriptor *td);

  int appendThr (ACE_thread_t tid, ACE_hthread_t thandle, long flags);

  void removeThr (ThreadDescriptor *td);

  std::list<ThreadDescriptor*> m_thrList;

  std::list<ThreadDescriptor*> m_terminatedThrList;

  ThreadMutex m_lock;

  ConditionThreadMutex m_zeroCond;

private:

  static ThreadManager *thrMgr;
};

#endif //ICC_THREAD_MANAGER_H
