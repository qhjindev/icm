#ifndef ICC_TIMER_QUEUE_CPP
#define ICC_TIMER_QUEUE_CPP

#include "os/config-all.h"

#include "icc/TimerQueue.h"
#include "icc/Guard.h"
//#include "icc/ReactorTimerInterface.h"
#include "icc/NullMutex.h"
#include "os/OS_NS_sys_time.h"

template <class TYPE, class FUNCTOR, class LOCK> TimeValue *
TimerQueueT <TYPE, FUNCTOR, LOCK>::calculateTimeout (TimeValue *maxWaitTime)
{
  ACE_MT(ACE_GUARD_RETURN (LOCK, mon, this->mMutex, maxWaitTime));

  if (this->isEmpty ())
    return maxWaitTime;
  else {
    TimeValue const curTime = OS::gettimeofday ();

    if (this->earliestTime () > curTime) {

      this->mTimeout = this->earliestTime () - curTime;
      if (maxWaitTime == 0 || *maxWaitTime > mTimeout)
        return &this->mTimeout;
      else
        return maxWaitTime;
    } else {
      this->mTimeout = TimeValue::zero;
      return &this->mTimeout;
    }
  }
}

template <class TYPE, class FUNCTOR, class LOCK> TimeValue *
TimerQueueT<TYPE, FUNCTOR, LOCK>::calculateTimeout (TimeValue *maxWaitTime, TimeValue *theTimeout)
{
  if (theTimeout == 0)
    return 0;

  if (this->isEmpty ())
    {
      // Nothing on the Timer_Queue, so use whatever the caller gave us.
      if (maxWaitTime)
        *theTimeout = *maxWaitTime;
      else
        return 0;
    }
  else
    {
      TimeValue curTime = OS::gettimeofday ();

      if (this->earliestTime () > curTime)
        {
          // The earliest item on the Timer_Queue is still in the
          // future.  Therefore, use the smaller of (1) caller's wait
          // time or (2) the delta time between now and the earliest
          // time on the Timer_Queue.

          *theTimeout = this->earliestTime () - curTime;
          if (!(maxWaitTime == 0 || *maxWaitTime > *theTimeout))
            *theTimeout = *maxWaitTime;
        }
      else
        {
          // The earliest item on the Timer_Queue is now in the past.
          // Therefore, we've got to "poll" the Reactor, i.e., it must
          // just check the descriptors and then dispatch timers, etc.
          *theTimeout = TimeValue::zero;
        }
    }
  return theTimeout;
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerQueueT<TYPE, FUNCTOR, LOCK>::TimerQueueT ()
{
  mUpcallFunctor = new FUNCTOR;
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerQueueT<TYPE, FUNCTOR, LOCK>::~TimerQueueT (void)
{
  delete this->mUpcallFunctor;
}
/*
template <class TYPE, class FUNCTOR, class LOCK> TimerNodeT<TYPE> *
TimerQueueT<TYPE, FUNCTOR, LOCK>::alloc_node (void)
{
  return this->free_list_->remove ();
}

template <class TYPE, class FUNCTOR, class LOCK> void
TimerQueueT<TYPE, FUNCTOR, LOCK>::free_node (TimerNodeT<TYPE> *node)
{
  this->free_list_->add (node);
}
*/
template <class TYPE, class FUNCTOR, class LOCK> LOCK &
TimerQueueT<TYPE, FUNCTOR, LOCK>::mutex (void)
{
  return this->mMutex;
}

template <class TYPE, class FUNCTOR, class LOCK> long
TimerQueueT<TYPE, FUNCTOR, LOCK>::schedule (const TYPE &type,
                                                      const void *act,
                                                      const TimeValue &future_time,
                                                      const TimeValue &interval)
{
  ACE_MT (ACE_GUARD_RETURN (LOCK, mon, this->mMutex, -1));

  // Schedule the timer.
  long const result =
    this->scheduleI (type,
                      act,
                      future_time,
                      interval);

  // Return on failure.
  if (result == -1)
    return result;

  // Return result;
  return result;
}

// Run the <handle_timeout> method for all Timers whose values are <=
// <cur_time>.
template <class TYPE, class FUNCTOR, class LOCK> int
TimerQueueT<TYPE, FUNCTOR, LOCK>::expire (const TimeValue &cur_time)
{
  ACE_MT (ACE_GUARD_RETURN (LOCK, mon, this->mMutex, -1));

  // Keep looping while there are timers remaining and the earliest
  // timer is <= the <cur_time> passed in to the method.

  if (this->isEmpty ())
    return 0;

  int number_of_timers_expired = 0;
  int result = 0;

  TimerNodeDispatchInfoT<TYPE> info;

  while ((result = this->dispatchInfoI (cur_time, info)) != 0)
    {
      this->upcall (info, cur_time);

      ++number_of_timers_expired;
    }

  return number_of_timers_expired;
}

template <class TYPE, class FUNCTOR, class LOCK> int
TimerQueueT<TYPE, FUNCTOR, LOCK>::dispatchInfoI (const TimeValue &cur_time, TimerNodeDispatchInfoT<TYPE> &info)
{

  if (this->isEmpty ())
    return 0;

  TimerNodeT<TYPE> *expired = 0;

  if (this->earliestTime () <= cur_time)
    {
      expired = this->removeFirst ();

      // Get the dispatch info
      expired->getDispatchInfo (info);

      // Check if this is an interval timer.
      if (expired->interval > TimeValue::zero)
        {
          // Make sure that we skip past values that have already "expired".
          do
            expired->timerValue += expired->interval;
          while (expired->timerValue <= cur_time);

          // Since this is an interval timer, we need to reschedule it.
          this->reschedule (expired);
        }
      else
        {
          // Call the factory method to free up the node.
          this->freeNode (expired);
        }

      return 1;
    }

  return 0;
}

template <class TYPE, class FUNCTOR, class LOCK>  int
TimerQueueT<TYPE, FUNCTOR, LOCK>::expire (void)
{
  if (!this->isEmpty ())
    return this->expire (OS::gettimeofday ());
  else
    return 0;
}

template <class TYPE, class FUNCTOR, class LOCK> int
TimerQueueT<TYPE, FUNCTOR, LOCK>::dispatchInfo (const TimeValue &cur_time, TimerNodeDispatchInfoT<TYPE> &info)
{

  ACE_MT (ACE_GUARD_RETURN (LOCK, mon, this->mMutex, 0));

  return this->dispatchInfoI (cur_time, info);
}

template <class TYPE, class FUNCTOR, class LOCK>  void
TimerQueueT<TYPE, FUNCTOR, LOCK>::upcall (TimerNodeDispatchInfoT<TYPE> &info, const TimeValue &cur_time)
{
  this->mUpcallFunctor->timeout (*this,
                                   info.type,
                                   info.act,
                                   info.recurringTimer,
                                   cur_time);
}

template <class TYPE, class FUNCTOR, class LOCK>  FUNCTOR &
TimerQueueT<TYPE, FUNCTOR, LOCK>::upcallFunctor (void)
{
  return *this->mUpcallFunctor;
}

template <class TYPE, class FUNCTOR, class LOCK> void
TimerQueueT<TYPE, FUNCTOR, LOCK>::returnNode (TimerNodeT<TYPE> *node)
{
  ACE_MT (ACE_GUARD (LOCK, mon, this->mMutex));
  this->freeNode (node);
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerQueueIteratorT<TYPE, FUNCTOR, LOCK>::TimerQueueIteratorT (void)
{
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerQueueIteratorT<TYPE, FUNCTOR, LOCK>::~TimerQueueIteratorT (void)
{
}

template <class LOCK> int
EventHandlerHandleTimeoutUpcall<LOCK>::timeout (TIMER_QUEUE &timer_queue,
                                                EventHandler *event_handler,
                                                const void *act,
                                                int recurring_timer,
                                                const TimeValue &cur_time)
{
  // Upcall to the <handler>s handle_timeout method.
  if (event_handler->handleTimeout (cur_time, act) == -1)
    {
      //if (event_handler->reactor_timer_interface ())
        //event_handler->reactor_timer_interface ()->cancelTimer (event_handler, 0);
      //else
        timer_queue.cancel (event_handler, 0); // 0 means "call handle_close()".
    }

  return 0;
}

template <class LOCK> int
EventHandlerHandleTimeoutUpcall<LOCK>::cancellation (TIMER_QUEUE &,
                                                     EventHandler *event_handler,
                                                     int dont_call)
{
  // Upcall to the <handler>s handle_close method
  if (dont_call == 0)
    event_handler->handleClose (ACE_INVALID_HANDLE,
                                 EventHandler::TIMER_MASK);

  return 0;
}

template <class LOCK> int
EventHandlerHandleTimeoutUpcall<LOCK>::deletion (TIMER_QUEUE &timer_queue,
                                                             EventHandler *event_handler,
                                                             const void *)
{

  return 0;
}

#endif //ICC_TIMER_QUEUE_CPP
