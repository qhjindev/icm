
#include "icc/MessageQueue.h"
#include "icc/Log.h"

template<ACE_SYNCH_DECL>
MessageQueue<ACE_SYNCH_USE>::MessageQueue (size_t hwm, size_t lwm)
: m_notEmptyCond (m_lock),
  m_notFullCond (m_lock)
{
  if (this->open (hwm, lwm) == -1) {
    LogMsg << "err open message queue!" << endl;
  }
}

template<ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::open (size_t hwm, size_t lwm)
{
  this->m_highWaterMark = hwm;
  this->m_lowWaterMark = lwm;
  this->m_curCount = 0;
  this->m_tail = 0;
  this->m_head = 0;

  return 0;
}

template<ACE_SYNCH_DECL>
MessageQueue<ACE_SYNCH_USE>::~MessageQueue (void)
{
}

template<ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::enqueueTail (MessageBlock *newItem, TimeValue *timeout)
{
  int queueCount = 0;
  {
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, icc_mon, this->m_lock, -1);

    if (this->waitNotFullCond (icc_mon, timeout) == -1)
      return -1;

    queueCount = this->enqueueTailInternal (newItem);

    if (queueCount == -1)
      return -1;
  }
  return queueCount;
}

template<ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::dequeueHead (MessageBlock *&firstItem, TimeValue *timeout)
{
  ACE_GUARD_RETURN (SYNCH_MUTEX, icc_mon, this->m_lock, -1);

  if (this->waitNotEmptyCond (icc_mon, timeout) == -1)
    return -1;

  return this->dequeueHeadInternal (firstItem);
}

template<ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::enqueueTailInternal (MessageBlock *newItem)
{
  if (newItem == 0)
    return -1;

  ++this->m_curCount;

  // List is empty, so build a new one.
  if (this->m_tail == 0) {
    this->m_head = newItem;
    this->m_tail = newItem;
  } else { //Link at the end.
    this->m_tail->next (newItem);
    newItem->prev (this->m_tail);
    this->m_tail = newItem;
  }

  if (this->signalDequeueWaiters () == -1)
    return -1;
  else
    return (int)this->m_curCount;
}

template<ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::dequeueHeadInternal (MessageBlock *&firstItem)
{
  if (this->m_head == 0)
    return -1;

  firstItem = this->m_head;
  this->m_head = this->m_head->next ();

  if (this->m_head == 0)
    this->m_tail = 0;
  else
    this->m_head->prev (0);

  --this->m_curCount;

  if (this->m_curCount == 0 && this->m_head == this->m_tail)
    this->m_head = this->m_tail = 0;

  firstItem->prev (0);
  firstItem->next (0);

  if (this->m_curCount <= this->m_lowWaterMark
      && this->signalEnqueueWaiters () == -1)
    return -1;
  else
    return (int)this->m_curCount;
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::waitNotFullCond(Guard<ACE_SYNCH_MUTEX_T> &mon,
                                             TimeValue *timeout)
{
  int result = 0;

  while (this->isFullInternal ()) {
    if (this->m_notFullCond.wait (timeout) == -1) {
      if (errno == ETIME)
        errno = EWOULDBLOCK;
      result = -1;
      break;
    }
  }

  return result;
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::waitNotEmptyCond(Guard<ACE_SYNCH_MUTEX_T> &mon,
                                              TimeValue *timeout)
{
  int result = 0;

  while (this->isEmptyInternal ()) {
    if (this->m_notEmptyCond.wait (timeout) == -1) {
      if (errno == ETIME)
        errno = EWOULDBLOCK;
      result = -1;
      break;
    }
  }
  return result;
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::isEmptyInternal (void)
{
  return this->m_tail == 0;
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::isFullInternal (void)
{
  return this->m_curCount >= this->m_highWaterMark;
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::isEmpty (void)
{
  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, icc_mon, this->m_lock, -1);

  return this->isEmptyInternal ();
}

// Check if queue is full (holds locks).

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::isFull (void)
{
  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX_T, icc_mon, this->m_lock, -1);

  return this->isFullInternal ();
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::signalEnqueueWaiters (void)
{
  if (this->m_notFullCond.signal () != 0)
    return -1;
  return 0;
}

template <ACE_SYNCH_DECL> int
MessageQueue<ACE_SYNCH_USE>::signalDequeueWaiters (void)
{
  if (this->m_notEmptyCond.signal () != 0)
    return -1;
  return 0;
}
