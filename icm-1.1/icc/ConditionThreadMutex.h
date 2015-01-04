#ifndef ICC_CONDITION_THREAD_MUTEXT_H
#define ICC_CONDITION_THREAD_MUTEXT_H

#include "icc/ThreadMutex.h"
#include "os/OS_NS_Thread.h"

class TimeValue;
class ThreadMutex;

class ConditionAttributes
{
public:
  ConditionAttributes (int type = ACE_DEFAULT_SYNCH_TYPE);
  ~ConditionAttributes (void);

private:
  friend class ConditionThreadMutex;

  ACE_condattr_t m_attributes;
};

class ConditionThreadMutex
{
public:
  ConditionThreadMutex (ThreadMutex &m,
                        const char *name = 0,
                        void *arg = 0);

  ConditionThreadMutex (ThreadMutex &m,
                        ConditionAttributes &attributes,
                        const char *name = 0,
                        void *arg = 0);

  ~ConditionThreadMutex (void);

  int remove (void);

  int wait (const TimeValue *abstime);

  //Wait from now
  int waitNow (const TimeValue *relativetime);

  int waitNow (int sec);

  int wait (void);

  int wait (ThreadMutex &mutex, const TimeValue *abstime = 0);

  int signal (void);

  int broadcast (void);

  ThreadMutex &getMutex (void);

protected:
  ACE_cond_t m_cond;

  ThreadMutex &m_mutex;

  bool m_removed;
};

#endif //ICC_CONDITION_THREAD_MUTEXT_H
