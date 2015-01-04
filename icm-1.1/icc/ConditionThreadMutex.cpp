
#include "os/OS.h"
#include "icc/ConditionThreadMutex.h"
#include "icc/ThreadMutex.h"
#include "icc/TimeValue.h"
#include "icc/Log.h"

ConditionAttributes::ConditionAttributes (int type)
{
  (void) OS::condattr_init (this->m_attributes, type);
}

ConditionAttributes::~ConditionAttributes (void)
{
  OS::condattr_destroy (this->m_attributes);
}

int
ConditionThreadMutex::remove (void)
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

ThreadMutex &
ConditionThreadMutex::getMutex (void)
{
  return this->m_mutex;
}

ConditionThreadMutex::ConditionThreadMutex (ThreadMutex &m,
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

ConditionThreadMutex::ConditionThreadMutex (ThreadMutex &m,
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

ConditionThreadMutex::~ConditionThreadMutex (void)
{
  this->remove ();
}

// Peform an "alertable" timed wait.  If the argument <abstime> == 0
// then we do a regular <cond_wait>, else we do a timed wait for up to
// <abstime> using the <cond_timedwait> function.

int
ConditionThreadMutex::wait (void)
{
  return OS::cond_wait (&this->m_cond, &this->m_mutex.lock);
}

int
ConditionThreadMutex::wait (ThreadMutex &mutex,
                            const TimeValue *abstime)
{
  return OS::cond_timedwait (&this->m_cond,
                                 &mutex.lock,
                                 const_cast <TimeValue *> (abstime));
}

int
ConditionThreadMutex::wait (const TimeValue *abstime)
{
  return this->wait (this->m_mutex, abstime);
}

int ConditionThreadMutex::waitNow (const TimeValue *relativetime) {
  TimeValue abstime = *relativetime + OS::gettimeofday();
  return this->wait(this->m_mutex, &abstime);
}

int ConditionThreadMutex::waitNow (int sec) {
  TimeValue abstime(sec);
  abstime += OS::gettimeofday();
  return this->wait(this->m_mutex, &abstime);
}

int
ConditionThreadMutex::signal (void)
{
  return OS::cond_signal (&this->m_cond);
}

int
ConditionThreadMutex::broadcast (void)
{
  return OS::cond_broadcast (&this->m_cond);
}
