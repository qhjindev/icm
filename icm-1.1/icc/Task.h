#ifndef ICC_TASK_H
#define ICC_TASK_H

//#include "Synch_Traits.h"
#include "icc/Thread.h"
#include "icc/MessageQueue.h"

template <ACE_SYNCH_DECL>
class Task : public Thread
{
public:
  Task (ThreadManager *thrMgr = 0,
        MessageQueue<ACE_SYNCH_USE> *mq = 0);

  virtual ~Task (void);

public:

  int putq (MessageBlock *);

  int getq (MessageBlock *&mb);

  MessageQueue<ACE_SYNCH_USE> *msgQueue;

};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "Task.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#endif //ICC_TASK_H
