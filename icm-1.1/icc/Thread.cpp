
#include "icc/Thread.h"
#include "icc/ThreadManager.h"
#include "icc/Log.h"

Thread::Thread (ThreadManager *thrMan)
  : m_thrMgr (thrMan),
    m_logMsgStream (0)
{
}

Thread::~Thread (void)
{
}

int
Thread::svc (void)
{
  return 0;
}

int
Thread::open (void *)
{
  return 0;
}

int
Thread::close (u_long)
{
  return 0;
}

int
Thread::activate (long flags)
{
  if (this->m_thrMgr == 0)
    this->m_thrMgr = ThreadManager::instance ();

  this->m_logMsgStream = Log::instance ()->os;

  int result = this->m_thrMgr->spawn (&Thread::svcRun, (void*)this, flags);
  if (result == -1)
    return -1;
  else
    return 0;
}

ThreadManager*
Thread::thrMgr (void) const
{
  return this->m_thrMgr;
}

void
Thread::thrMgr (ThreadManager* tm)
{
  this->m_thrMgr = tm;
}

ACE_THR_FUNC_RETURN
Thread::svcRun (void *args)
{
  Thread *t = (Thread*)args;
  Log::instance ()->os = t->m_logMsgStream;

  int svcStatus = t->svc ();

#if defined (ACE_WIN32)
  ACE_THR_FUNC_RETURN status = static_cast<ACE_THR_FUNC_RETURN> (svcStatus);
#else
  ACE_THR_FUNC_RETURN status = reinterpret_cast<ACE_THR_FUNC_RETURN> (svcStatus);
#endif //ACE_WIN32

  t->m_thrMgr->exit ();

  t->close ();

  return status;
}

void
Thread::self (ACE_hthread_t &t_id)
{
  OS::thr_self (t_id);
}

ACE_thread_t
Thread::self (void)
{
  return OS::thr_self ();
}
