#ifndef ICC_CONDITION_RECURSIVE_THREAD_MUTEXT_H
#define ICC_CONDITION_RECURSIVE_THREAD_MUTEXT_H

#include "icc/RecursiveThreadMutex.h"
#include "icc/ConditionThreadMutex.h"
#include "os/OS_NS_Thread.h"

class TimeValue;
class ThreadMutex;

/*
class ConditionAttributes
{
public:
  ConditionAttributes (int type = ACE_DEFAULT_SYNCH_TYPE);
  ~ConditionAttributes (void);

private:
  friend class ConditionThreadMutex;

  ACE_condattr_t m_attributes;
};
*/

class ConditionRecursiveThreadMutex
{
public:
  ConditionRecursiveThreadMutex (RecursiveThreadMutex &m,
                        const char *name = 0,
                        void *arg = 0);
/*
  ConditionRecursiveThreadMutex (RecursiveThreadMutex &m,
                        ConditionAttributes &attributes,
                        const char *name = 0,
                        void *arg = 0);
*/
  ~ConditionRecursiveThreadMutex (void);

  int remove (void);

  int wait (const TimeValue *abstime);

  //Wait from now
  int waitNow (const TimeValue *relativetime);

  int waitNow (int sec);

  int wait (void);

  int wait (RecursiveThreadMutex &mutex, const TimeValue *abstime = 0);

  int signal (void);

  int broadcast (void);

  RecursiveThreadMutex &getMutex (void);

protected:
  ACE_cond_t m_cond;

  RecursiveThreadMutex &m_mutex;

  bool m_removed;
};

#endif //ICC_CONDITION_RECURSIVE_THREAD_MUTEXT_H
