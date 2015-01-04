#ifndef ICC_TIMER_QUEUE_H
#define ICC_TIMER_QUEUE_H

#include "icc/SynchTraits.h"

#include "icc/EventHandler.h"
#include "icc/TimeValue.h"

#if defined (ACE_HAS_THREADS)
#  include "icc/RecursiveThreadMutex.h"
#else
#  include "icc/NullMutex.h"
#endif /* ACE_HAS_THREADS */

//Maintains generated dispatch information for Timer nodes.
template <class TYPE>
class TimerNodeDispatchInfoT
{
public:
  // The type of object held in the queue
  TYPE type;

  // Asynchronous completion token associated with the timer.
  const void *act;

  // Flag to check if the timer is recurring.
  int recurringTimer;
};

template <class TYPE>
class TimerNodeT
{
public:
  //TimerNodeT (void);

  //~TimerNodeT (void);

  typedef TimerNodeDispatchInfoT <TYPE> DISPATCH_INFO;

  void set (const TYPE &type,
            const void *a,
            const TimeValue &t,
            const TimeValue &i,
            TimerNodeT<TYPE> *n,
            long timer_id)
  {
    this->type = type;
    this->act = a;
    this->timerValue = t;
    this->interval = i;
    this->next = n;
    this->timerId = timer_id;
  }

  void getDispatchInfo (TimerNodeDispatchInfoT <TYPE>& info)
  {
    info.type = this->type;
    info.act = this->act;
    info.recurringTimer = this->interval > TimeValue::zero;
  }

  // Type of object stored in the Queue
  TYPE type;

  // Asynchronous completion token associated with the timer.
  const void* act;

  TimeValue timerValue;

  TimeValue interval;

  TimerNodeT<TYPE>* prev;

  TimerNodeT<TYPE>* next;

  long timerId;
};

template <class TYPE, class FUNCTOR, class LOCK>
class TimerQueueIteratorT
{
public:
  TimerQueueIteratorT(void);

  virtual ~TimerQueueIteratorT(void);

  virtual void first (void) = 0;

  virtual void next (void) = 0;

  virtual bool isdone (void) const = 0;

  virtual TimerNodeT<TYPE>* item (void) = 0;
};

template <class TYPE, class FUNCTOR, class LOCK>
class TimerQueueT
{
public:
  typedef TimerQueueIteratorT<TYPE, FUNCTOR, LOCK> ITERATOR;

  TimerQueueT ();

  ~TimerQueueT ();

  virtual bool isEmpty (void) const = 0;

  virtual const TimeValue& earliestTime (void) const = 0;

  virtual long schedule (const TYPE& type,
                         const void* act,
                         const TimeValue& futureTime,
                         const TimeValue& interval = TimeValue::zero);

  virtual int resetInterval (long timerId, const TimeValue& interval) = 0;

  virtual int cancel (const TYPE& type, int dontCallHandleClose = 1) = 0;

  virtual int cancel (long timerId, const void** act = 0, int dontCallHandleClose = 1) = 0;

  int expire (void);

  virtual int expire (const TimeValue& currentTime);

  virtual int dispatchInfo (const TimeValue& currrentTime, TimerNodeDispatchInfoT<TYPE>& info);

  virtual TimeValue* calculateTimeout (TimeValue* max);

  virtual TimeValue* calculateTimeout (TimeValue* max, TimeValue* theTimeout);

  LOCK& mutex (void);

  FUNCTOR& upcallFunctor (void);

  virtual ITERATOR& iter (void) = 0;

  virtual TimerNodeT<TYPE>* removeFirst (void) = 0;

  virtual TimerNodeT<TYPE>* getFirst (void) = 0;

  virtual void returnNode (TimerNodeT<TYPE>*);

  void upcall (TimerNodeDispatchInfoT<TYPE>& info, const TimeValue& curTime);

protected:

  virtual long scheduleI (const TYPE& type, 
                          const void* act,
                          const TimeValue& futureTime,
                          const TimeValue& interval) = 0;

  virtual void reschedule (TimerNodeT<TYPE> *) = 0;

  virtual TimerNodeT<TYPE> *allocNode (void) = 0;

  virtual void freeNode (TimerNodeT<TYPE>*) = 0;

  virtual int dispatchInfoI (const TimeValue& curTime, TimerNodeDispatchInfoT<TYPE>& info);

  LOCK mMutex;

  FUNCTOR* mUpcallFunctor;

  //bool const mDeleteUpcallFunctor;

private:

  TimeValue mTimeout;
};

template <class LOCK>
class EventHandlerHandleTimeoutUpcall
{
public:
  typedef TimerQueueT<EventHandler*,
                      EventHandlerHandleTimeoutUpcall<LOCK>,
                      LOCK>
          TIMER_QUEUE;

  int timeout (TIMER_QUEUE& timerQueue,
               EventHandler* handler,
               const void* arg,
               int recurringTimer,
               const TimeValue& curTime);

  int cancellation (TIMER_QUEUE& timerQueue, EventHandler* handler, int dont_call);

  int deletion (TIMER_QUEUE& timerQueue, EventHandler* handler, const void* arg);

};

typedef TimerQueueT<EventHandler*,
                    EventHandlerHandleTimeoutUpcall<SYNCH_RECURSIVE_MUTEX>,
                    SYNCH_RECURSIVE_MUTEX>
        TimerQueue;

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "icc/TimerQueue.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("TimerQueueT.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

#endif //ICC_TIMER_QUEUE_H
