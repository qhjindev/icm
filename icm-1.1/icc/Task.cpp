
#include "icc/Task.h"

template<ACE_SYNCH_DECL>
Task<ACE_SYNCH_USE>::Task (ThreadManager *thrMan, MessageQueue<ACE_SYNCH_USE> *mq)
  : Thread (thrMan), msgQueue (0)
{
  if (mq == 0)
    mq = new MessageQueue<ACE_SYNCH_USE>;

  this->msgQueue = mq;
}

template<ACE_SYNCH_DECL>
Task<ACE_SYNCH_USE>::~Task (void)
{
}

template<ACE_SYNCH_DECL> int
Task<ACE_SYNCH_USE>::getq (MessageBlock *&mb)
{
  return this->msgQueue->dequeueHead (mb);
}

template<ACE_SYNCH_DECL> int
Task<ACE_SYNCH_USE>::putq (MessageBlock *mb)
{
  return this->msgQueue->enqueueTail (mb);
}
