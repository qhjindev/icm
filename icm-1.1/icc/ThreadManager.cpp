
#include "os/OS.h"
#include "os/config-macros.h"
#include "os/Global_Macros.h"
#include "icc/ThreadManager.h"
#include "icc/Guard.h"
#include "icc/Log.h"

#include <list>

using namespace std;

ThreadDescriptor::ThreadDescriptor (void)
{
  this->m_thrId = 0;
  this->m_thrHandle = 0;
  this->m_tm = 0;
  this->m_flags = 0;
  this->m_terminated = false;
}

ThreadDescriptor::~ThreadDescriptor (void)
{

}

void
ThreadDescriptor::terminate ()
{
  if (!m_terminated) {
    m_terminated = true;

    if (this->m_tm != 0) {
      if (ACE_BIT_DISABLED (this->m_flags, THR_DETACHED | THR_DAEMON)
          || ACE_BIT_ENABLED (this->m_flags, THR_JOINABLE)) {
          this->m_tm->registerAsTerminated (this);
      }

      if (this->m_tm != 0)
        this->m_tm->removeThr (this);
    }
  }
}

ThreadManager::ThreadManager (void)
  : m_zeroCond (m_lock)
{

}

ThreadManager::~ThreadManager (void)
{
}

ThreadManager *ThreadManager::thrMgr = 0;

ThreadManager *
ThreadManager::instance (void)
{
  if (ThreadManager::thrMgr == 0) {
    if (ThreadManager::thrMgr == 0) {
      ThreadManager::thrMgr = new ThreadManager;
    }
  }

  return ThreadManager::thrMgr;
}

ThreadManager *
ThreadManager::instance (ThreadManager *tm)
{
  ThreadManager *t = ThreadManager::thrMgr;
  ThreadManager::thrMgr = tm;
  return t;
}

int
ThreadManager::spawn (ACE_THR_FUNC func,
                      void *args,
                      long flags)
{
  ACE_BIT_DISABLED (flags, THR_DAEMON);

  ACE_thread_t *tid = 0;
  ACE_hthread_t *thandle = 0;

  int const result = OS::thr_create (func,
                                        args,
                                        flags,
                                        tid,
                                        thandle);
  if (result != 0) {
    ICC_ERROR("ThreadManager::spawn creat thread error: %s", OS::strerror(OS::map_errno(OS::last_error())));
    return -1;
  }

  return this->appendThr (*tid, *thandle, flags);
}

int
ThreadManager::wait (const int seconds) {
  TimeValue tv(seconds);
  return wait(&tv);
}

int
ThreadManager::wait (const TimeValue *timeout)
{
  std::list<ThreadDescriptor*> termThrListCopy;
  {
    ACE_GUARD_RETURN (ThreadMutex, mon, this->m_lock, -1);
    TimeValue localTimeout;
    if(timeout != 0) {
      localTimeout = *timeout;
      localTimeout += OS::gettimeofday();
      timeout = &localTimeout;
    }

    while (this->m_thrList.size () > 0)
      if (this->m_zeroCond.wait (timeout) == -1)
        return -1;

    termThrListCopy.assign (this->m_terminatedThrList.begin (), this->m_terminatedThrList.end ());
    /*
    ThreadDescriptor* item = 0;
    item = this->m_terminatedThrList.front ();
    while ((item = this->m_terminatedThrList.front ()) != this->m_terminatedThrList.end ()) {
      termThrListCopy.push_back (item);
      this->m_terminatedThrList.pop_front ();
    }*/
  }

  list<ThreadDescriptor*>::iterator iter = termThrListCopy.begin ();
  ThreadDescriptor* item = 0;
  while (iter != termThrListCopy.end ()) {
    item = *iter;
    if (ACE_BIT_DISABLED (item->m_flags, THR_DETACHED | THR_DAEMON)
        || ACE_BIT_ENABLED (item->m_flags, THR_JOINABLE))
      OS::thr_join (item->m_thrHandle, 0);

    delete item;

    iter++;
  }

  return 0;
}

int
ThreadManager::exit ()
{
  {
    ACE_GUARD_RETURN (ThreadMutex, mon, this->m_lock, 0);
    ACE_thread_t id = OS::thr_self ();
    ThreadDescriptor *td = this->findThread (id);
    if (td != 0) {
      td->terminate ();
    }
  }

  return 0;
}

ThreadDescriptor *
ThreadManager::findThread (ACE_thread_t tid)
{
  for (list<ThreadDescriptor*>::iterator iter = this->m_thrList.begin ();
       iter != this->m_thrList.end ();
       iter++)
  {
    ThreadDescriptor *td = *iter;
    if (OS::thr_equal (td->m_thrId, tid))
      return td;
  }

  return 0;
}

ThreadDescriptor *
ThreadManager::findHThread (ACE_hthread_t hid)
{
  for (list<ThreadDescriptor*>::iterator iter = this->m_thrList.begin ();
       iter != this->m_thrList.end ();
       iter++)
  {
    ThreadDescriptor *td = *iter;
    if (OS::thr_cmp (td->m_thrHandle, hid))
      return td;
  }

  return 0;
}

int
ThreadManager::registerAsTerminated (ThreadDescriptor *td)
{
  this->m_terminatedThrList.push_back (td);

  return 0;
}

int
ThreadManager::appendThr (ACE_thread_t tid, ACE_hthread_t thandle, long flags)
{
  ThreadDescriptor *thrDesc = new ThreadDescriptor;
  thrDesc->m_tm = this;
  thrDesc->m_thrId = tid;
  thrDesc->m_thrHandle = thandle;
  thrDesc->m_flags = flags;

  this->m_thrList.push_back (thrDesc);

  return 0;
}

void
ThreadManager::removeThr (ThreadDescriptor *td)
{
  td->m_tm = 0;
  this->m_thrList.remove (td);

  if (this->m_thrList.size () == 0)
    this->m_zeroCond.broadcast ();
}
