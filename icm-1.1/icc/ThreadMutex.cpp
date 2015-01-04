
#include "icc/ThreadMutex.h"


ThreadMutex::ThreadMutex (const char *name, ACE_mutexattr_t *arg)
  : removed (false)
{
  if (OS::thread_mutex_init (&this->lock,
                                 0,
                                 name,
                                 arg) != 0)
  {
    //Log(); 
  }
}

ThreadMutex::~ThreadMutex (void)
{
  this->remove ();
}

const ACE_thread_mutex_t &
ThreadMutex::getLock (void) const
{
  return this->lock;
}

int
ThreadMutex::acquireRead (void)
{
  return OS::thread_mutex_lock (&this->lock);
}

int
ThreadMutex::acquireWrite (void)
{
  return OS::thread_mutex_lock (&this->lock);
}

int
ThreadMutex::tryAcquireRead (void)
{
  return OS::thread_mutex_trylock (&this->lock);
}

int
ThreadMutex::tryAcquireWrite (void)
{
  return OS::thread_mutex_trylock (&this->lock);
}

int
ThreadMutex::acquire (void)
{
  return OS::thread_mutex_lock (&this->lock);
}
/*
int
ThreadMutex::acquire (TimeValue &tv)
{
  return OS::threadMutexLock (&this->lock, tv);
}

int
ThreadMutex::acquire (TimeValue *tv)
{
  return OS::threadMutexLock (&this->lock, tv);
}
*/
int
ThreadMutex::tryAcquire (void)
{
  return OS::thread_mutex_trylock (&this->lock);
}

int
ThreadMutex::release (void)
{
  return OS::thread_mutex_unlock (&this->lock);
}

int
ThreadMutex::remove (void)
{
  int result = 0;
  if (this->removed == false) {
    this->removed = true;
    result = OS::thread_mutex_destroy (&this->lock);
  }
  return result;
}

