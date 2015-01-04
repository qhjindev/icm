#ifndef ICC_THREAD_H
#define ICC_THREAD_H

#include "os/OS_NS_Thread.h"
#include "os/config-macros.h"
#include <ostream>

class ThreadManager;

class Thread
{
public:
  Thread (ThreadManager *thrMan = 0);

  virtual ~Thread (void);

  virtual int open (void *args = 0);

  virtual int close (unsigned long flags = 0);

  virtual int svc (void);

  virtual int activate (long flags = THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED);

  ThreadManager* thrMgr (void) const;

  void thrMgr (ThreadManager* thrMgr);

  static ACE_THR_FUNC_RETURN svcRun (void *);

  static void self (ACE_hthread_t &t_handle);

  /// Return the unique ID of the thread.
  static ACE_thread_t self (void);

protected:

  ThreadManager *m_thrMgr;

  std::ostream *m_logMsgStream;

};

#endif //ICC_THREAD_H
