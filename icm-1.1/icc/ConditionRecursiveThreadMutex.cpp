
#include "os/OS.h"
#include "icc/ConditionRecursiveThreadMutex.h"
#include "icc/ThreadMutex.h"
#include "icc/TimeValue.h"
#include "icc/Log.h"

int
ConditionRecursiveThreadMutex::remove (void)
{
  int result = 0;

  if (!this->m_removed)
    {
      this->m_removed = true;

      while ((result = OS::cond_destroy (&this->m_cond)) == -1
             && errno == EBUSY)
      {
        OS::cond_broadcast (&this->m_cond);
        OS::thr_yield ();
      }
    }
  return result;
}

RecursiveThreadMutex &
ConditionRecursiveThreadMutex::getMutex (void)
{
  return this->m_mutex;
}

ConditionRecursiveThreadMutex::ConditionRecursiveThreadMutex (RecursiveThreadMutex &m,
                                            const char *name,
                                            void *arg)
  : m_mutex (m),
    m_removed (false)
{
  if (OS::cond_init (&this->m_cond,
                         (short) USYNC_THREAD,
                         name,
                         arg) != 0)
  {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p\n"),
                ACE_TEXT ("ConditionThreadMutex::ConditionThreadMutex")));
  }
}
/*
ConditionRecursiveThreadMutex::ConditionRecursiveThreadMutex (RecursiveThreadMutex &m,
                                            ConditionAttributes &attributes,
                                            const char *name,
                                            void *arg)
  : m_mutex (m),
    m_removed (false)
{
  if (OS::cond_init (&this->m_cond, attributes.m_attributes, name, arg) != 0) {
    ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p\n"),
                ACE_TEXT ("ConditionThreadMutex::ConditionThreadMutex")));
  }
}
*/
ConditionRecursiveThreadMutex::~ConditionRecursiveThreadMutex (void)
{
  this->remove ();
}

// Peform an "alertable" timed wait.  If the argument <abstime> == 0
// then we do a regular <cond_wait>, else we do a timed wait for up to
// <abstime> using the <cond_timedwait> function.

int
ConditionRecursiveThreadMutex::wait (void)
{
  return OS::cond_wait (&this->m_cond, &this->m_mutex.mutex());
}

int
ConditionRecursiveThreadMutex::wait (RecursiveThreadMutex &mutex,
                            const TimeValue *abstime)
{
  return OS::cond_timedwait (&this->m_cond,
                                 &mutex.mutex(),
                                 const_cast <TimeValue *> (abstime));
}

int
ConditionRecursiveThreadMutex::wait (const TimeValue *abstime)
{
  return this->wait (this->m_mutex, abstime);
}

int ConditionRecursiveThreadMutex::waitNow (const TimeValue *relativetime) {
  TimeValue abstime = *relativetime + OS::gettimeofday();
  return this->wait(this->m_mutex, &abstime);
}

int ConditionRecursiveThreadMutex::waitNow (int sec) {
  TimeValue abstime(sec);
  abstime += OS::gettimeofday();
  return this->wait(this->m_mutex, &abstime);
}

int
ConditionRecursiveThreadMutex::signal (void)
{
  return OS::cond_signal (&this->m_cond);
}

int
ConditionRecursiveThreadMutex::broadcast (void)
{
  return OS::cond_broadcast (&this->m_cond);
}
