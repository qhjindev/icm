
#include "icc/RecursiveThreadMutex.h"

RecursiveThreadMutex::RecursiveThreadMutex (const char *name, ACE_mutexattr_t *arg)
: m_removed(false)
{
   if (OS::recursive_mutex_init (&this->m_lock,
                                     name,
                                     arg) == -1)
   {/*
     ACE_ERROR ((LM_ERROR,
                 ACE_TEXT ("%p\n"),
                 ACE_TEXT ("recursive_mutex_init")));*/
   }
}

RecursiveThreadMutex::~RecursiveThreadMutex (void)
{
  this->remove ();
}

int
RecursiveThreadMutex::remove (void)
{
  int result = 0;
  if (this->m_removed == false)
  {
    this->m_removed = true;
    result = OS::recursive_mutex_destroy (&this->m_lock);
  }
  return result;
}

// The counter part of the following two functions for Win32 are
// located in file Synch.i
ACE_thread_t
RecursiveThreadMutex::getThreadId (void)
{
  // ACE_TRACE ("ACE_Recursive_Thread_Mutex::get_thread_id");
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  // @@ The structure CriticalSection in Win32 doesn't hold the thread
  // handle of the thread that owns the lock.  However it is still not
  // clear at this point how to translate a thread handle to its
  // corresponding thread id.
  errno = ENOTSUP;
  return OS::NULL_thread;
#else
  ACE_thread_t owner_id;
  OS::mutex_lock (&this->m_lock.nesting_mutex_);
  owner_id = this->m_lock.owner_id_;
  OS::mutex_unlock (&this->m_lock.nesting_mutex_);
  return owner_id;
#endif /* ACE_WIN32 */
}

int
RecursiveThreadMutex::getNestingLevel (void)
{
  // ACE_TRACE ("ACE_Recursive_Thread_Mutex::get_nesting_level");
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
# if defined (ACE_WIN32)
  // This is really a Win32-ism...
  // Nothing inside of a CRITICAL_SECTION object should ever be
  // accessed directly.  It is documented to change at any time.
  //
  // It has been reported that this this works for all three
  // architectures.  However, this does not work on Win64 before SP1.
  return this->m_lock.RecursionCount;
# else
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_WIN32 */
#else
  int nesting_level = 0;
  OS::mutex_lock (&this->m_lock.nesting_mutex_);
  nesting_level = this->m_lock.nesting_level_;
  OS::mutex_unlock (&this->m_lock.nesting_mutex_);
  return nesting_level;
#endif /* !ACE_HAS_WINCE */
}

ACE_recursive_thread_mutex_t &
RecursiveThreadMutex::mutex (void)
{
  return m_lock;
}

ACE_thread_mutex_t &
RecursiveThreadMutex::getNestingMutex (void)
{
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return static_cast<ACE_thread_mutex_t &> (m_lock);
#else
  return m_lock.nesting_mutex_;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
}

void
RecursiveThreadMutex::setThreadId (ACE_thread_t t)
{
// ACE_TRACE ("ACE_Recursive_Thread_Mutex::set_thread_id");
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  ACE_UNUSED_ARG (t);
#else  /* ! ACE_HAS_RECURSIVE_MUTEXES */
  this->m_lock.owner_id_ = t;
#endif /* ! ACE_HAS_RECURSIVE_MUTEXES */
}

int
RecursiveThreadMutex::acquire (void)
{
  return OS::recursive_mutex_lock (&this->m_lock);
}

int
RecursiveThreadMutex::release (void)
{
  return OS::recursive_mutex_unlock (&this->m_lock);
}

int
RecursiveThreadMutex::tryAcquire (void)
{
  return OS::recursive_mutex_trylock (&this->m_lock);
}

int
RecursiveThreadMutex::acquire (TimeValue &tv)
{
  return OS::recursive_mutex_lock (&this->m_lock, tv);
}

int
RecursiveThreadMutex::acquire (TimeValue *tv)
{
  return OS::recursive_mutex_lock (&this->m_lock, tv);
}

int
RecursiveThreadMutex::acquireRead (void)
{
  return this->acquire ();
}

int
RecursiveThreadMutex::acquireWrite (void)
{
  return this->acquire ();
}

int
RecursiveThreadMutex::tryAcquireRead (void)
{
  return this->tryAcquire ();
}

int
RecursiveThreadMutex::tryAcquireWrite (void)
{
  return this->tryAcquire ();
}

RecursiveThreadMutex::RecursiveThreadMutex (const RecursiveThreadMutex &)
{
}

