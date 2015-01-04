#ifndef ICC_TIMER_QUEUE_ADAPTER_H
#define ICC_TIMER_QUEUE_ADAPTER_H

#include "icc/Thread.h"
#include "icc/EventHandler.h"
#include "icc/TimeValue.h"
#include "icc/SynchTraits.h"

template <class TQ>
class ThreadTimerQueueAdapter : public Thread
{
public:
  typedef TQ TIMER_QUEUE;

  ThreadTimerQueueAdapter ();

  virtual ~ThreadTimerQueueAdapter (void);

  long schedule (EventHandler* handler,
                 const void* act,
                 const TimeValue &futureTime,
                 const TimeValue &interval = TimeValue::zero);

  int cancel (long timerId, const void **act = 0);

  virtual int svc (void);

  SYNCH_RECURSIVE_MUTEX &mutex (void);

private:

  TQ* mTimerQueue;

  SYNCH_RECURSIVE_MUTEX mMutex;
  //ThreadMutex mMutex;

  SYNCH_RECURSIVE_CONDITION mCondition;
  //ConditionThreadMutex mCondition;
};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
# include "icc/TimerQueueAdapter.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
# pragma implementation ("TimerQueueAdapter.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

#endif //ICC_TIMER_QUEUE_ADAPTER_H
