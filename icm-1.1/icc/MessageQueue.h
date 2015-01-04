#ifndef _ICC_MESSAGE_QUEUE_H_
#define _ICC_MESSAGE_QUEUE_H_

#include "os/Global_Macros.h"
#include "icc/SynchTraits.h"
#include "icc/MessageBlock.h"
#include "icc/TimeValue.h"
#include "icc/Guard.h"

template <ACE_SYNCH_DECL>
class MessageQueue
{
public:
  enum
  {
    DEFAULT_HWM = 16 * 1024,
    DEFAULT_LWM = 16 * 1024
  };

  MessageQueue (size_t hwm = MessageQueue<ACE_SYNCH_USE>::DEFAULT_HWM,
                size_t lwm = MessageQueue<ACE_SYNCH_USE>::DEFAULT_LWM);

  int open (size_t hwm = MessageQueue<ACE_SYNCH_USE>::DEFAULT_HWM,
            size_t lwm = MessageQueue<ACE_SYNCH_USE>::DEFAULT_LWM);

  virtual ~MessageQueue (void);

  int enqueueTail (MessageBlock *newItem, TimeValue *timeout = 0);

  int dequeueHead (MessageBlock *&firstItem, TimeValue *timeout = 0);

  int isFull (void);
  int isEmpty (void);

protected:
  int enqueueTailInternal (MessageBlock *newItem);
  int dequeueHeadInternal (MessageBlock *&firstItem);

  int isFullInternal (void);

  int isEmptyInternal (void);

  int waitNotFullCond (Guard<ACE_SYNCH_MUTEX_T> &mon, TimeValue *timeout);

  int waitNotEmptyCond (Guard<ACE_SYNCH_MUTEX_T> &mon, TimeValue *timeout);

  int signalEnqueueWaiters (void);

  int signalDequeueWaiters (void);

  MessageBlock *m_head;
  MessageBlock *m_tail;

  size_t m_curCount;

  size_t m_lowWaterMark;
  size_t m_highWaterMark;

  ACE_SYNCH_MUTEX_T m_lock;

  ACE_SYNCH_CONDITION_T m_notEmptyCond;

  ACE_SYNCH_CONDITION_T m_notFullCond;
};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "MessageQueue.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#endif //_ICC_MESSAGE_QUEUE_H_
