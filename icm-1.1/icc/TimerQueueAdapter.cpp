#ifndef ICC_TIMER_QUEUE_ADAPTER_CPP
#define ICC_TIMER_QUEUE_ADAPTER_CPP

#include "icc/Guard.h"
#include "icc/TimerQueueAdapter.h"

template<class TQ>
ThreadTimerQueueAdapter<TQ>::ThreadTimerQueueAdapter()
 : mCondition (mMutex)
{
  this->mTimerQueue = new TQ;
}

template<class TQ>
ThreadTimerQueueAdapter<TQ>::~ThreadTimerQueueAdapter()
{
  delete this->mTimerQueue;
  this->mTimerQueue = 0;
}

template<class TQ> long
ThreadTimerQueueAdapter<TQ>::schedule(EventHandler *handler, 
                                      const void *act, 
                                      const TimeValue &futureTime, 
                                      const TimeValue &interval)
{
  ACE_GUARD_RETURN (SYNCH_RECURSIVE_MUTEX, guard, this->mMutex, -1);

  long const result = this->mTimerQueue->schedule (handler, act, futureTime, interval);
  this->mCondition.signal ();
  return result;
}

template<class TQ> int
ThreadTimerQueueAdapter<TQ>::cancel (long timerId, const void **act)
{
  ACE_GUARD_RETURN (SYNCH_RECURSIVE_MUTEX, guard, this->mMutex, -1);

  int const result = this->mTimerQueue->cancel (timerId, act);
  mCondition.signal ();
  return result;
}

template<class TQ> int
ThreadTimerQueueAdapter<TQ>::svc (void)
{
  ACE_GUARD_RETURN (SYNCH_RECURSIVE_MUTEX, guard, this->mMutex, -1);

  while (1)
    {
      // If the queue is empty, sleep until there is a change on it.
      if (this->mTimerQueue->isEmpty ())
        this->mCondition.wait ();
      else
        {
          // Compute the remaining time, being careful not to sleep
          // for "negative" amounts of time.
          TimeValue const tvCurr = OS::gettimeofday ();
          TimeValue const tvEarl = this->mTimerQueue->earliestTime ();

          if (tvEarl > tvCurr)
            {
              // The earliest time on the Timer_Queue is in future, so
              // use OS::gettimeofday() to convert the tv to the
              // absolute time.
              TimeValue const tv = OS::gettimeofday () + (tvEarl - tvCurr);
              // ACE_DEBUG ((LM_DEBUG,  ACE_TEXT ("waiting until %u.%3.3u secs\n"),
              // tv.sec(), tv.msec()));
              this->mCondition.wait (&tv);
            }
        }

      // Expire timers anyway, at worst this is a no-op.
      this->mTimerQueue->expire ();
    }

  return 0;
}

#endif /* ICC_TIMER_QUEUE_ADAPTER_CPP */
