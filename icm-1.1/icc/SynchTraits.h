#ifndef ICC_SYNCH_TRAITS_H
#define ICC_SYNCH_TRAITS_H


#include /**/ "os/config-all.h"    /* Need to know threads, template settings */
#include "icc/Synch.h"
#include "icc/Lock.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// Forward decl
class NullMutex;
class NullCondition;
class NullSemaphore;

class ThreadMutex;
class ProcessMutex;
class RecursiveThreadMutex;
class RWThreadMutex;
class ConditionThreadMutex;
class ConditionRecursiveThreadMutex;
class ThreadSemaphore;

#if defined (ACE_HAS_TEMPLATE_TYPEDEFS)

/**
 * @class ACE_NULL_SYNCH
 *
 * @brief Implement a do nothing Synchronization wrapper that
 *        typedefs the @c ACE_Condition and @c ACE_Mutex to the
 *        @c Null* versions.
 */
class ACE_Export NULL_SYNCH
{
public:
  typedef NullMutex MUTEX;
  typedef NullMutex NULL_MUTEX;
  typedef NullMutex PROCESS_MUTEX;
  typedef NullMutex RECURSIVE_MUTEX;
  typedef NullMutex RW_MUTEX;
  typedef NullCondition CONDITION;
  typedef NullCondition RECURSIVE_CONDITION;
  typedef NullSemaphore SEMAPHORE;
  typedef NullMutex NULL_SEMAPHORE;
};

#if defined (ACE_HAS_THREADS)

class ProcessMutex;
class ConditionRecursiveThreadMutex;

/**
 * @class ACE_MT_SYNCH
 *
 * @brief Implement a default thread safe synchronization wrapper that
 *        typedefs the @c ACE_Condition and @c ACE_Mutex to the
 * @c ACE_Condition and @c ACE_Mutex versions.
 *
 * @todo This should be a template, but SunC++ 4.0.1 complains about
 *       this.
 */
class ACE_Export MT_SYNCH
{
public:
  typedef ThreadMutex MUTEX;
  typedef NullMutex NULL_MUTEX;
  typedef ProcessMutex PROCESS_MUTEX;
  typedef RecursiveThreadMutex RECURSIVE_MUTEX;
  typedef RWThreadMutex RW_MUTEX;
  typedef ConditionThreadMutex CONDITION;
  typedef ConditionRecursiveThreadMutex RECURSIVE_CONDITION;
  //typedef ConditionThreadMutex RECURSIVE_CONDITION;
  typedef ThreadSemaphore SEMAPHORE;
  typedef NullSemaphore NULL_SEMAPHORE;
};

#endif /* ACE_HAS_THREADS */

#define SYNCH_MUTEX SYNCH::MUTEX
#define SYNCH_NULL_MUTEX SYNCH::NULL_MUTEX
#define SYNCH_RECURSIVE_MUTEX SYNCH::RECURSIVE_MUTEX
#define SYNCH_RW_MUTEX SYNCH::RW_MUTEX
#define SYNCH_CONDITION SYNCH::CONDITION
#define SYNCH_RECURSIVE_CONDITION SYNCH::RECURSIVE_CONDITION
#define SYNCH_NULL_SEMAPHORE SYNCH::NULL_SEMAPHORE
#define SYNCH_SEMAPHORE SYNCH::SEMAPHORE

#else /* !ACE_HAS_TEMPLATE_TYPEDEFS */

#if defined (ACE_HAS_OPTIMIZED_MESSAGE_QUEUE)
#define NULL_SYNCH NullMutex, NullCondition, NullMutex
#define MT_SYNCH ACE_Thread_Mutex, ConditionThreadMutex, ThreadSemaphore
#else
#define NULL_SYNCH NullMutex, NullCondition
#define MT_SYNCH ThreadMutex, ConditionThreadMutex
#endif /* ACE_HAS_OPTIMIZED_MESSAGE_QUEUE */

#if defined (ACE_HAS_THREADS)

#define SYNCH_MUTEX ACE_Thread_Mutex
#define SYNCH_NULL_MUTEX  NullMutex
#define SYNCH_RECURSIVE_MUTEX RecursiveThreadMutex
#define SYNCH_RW_MUTEX RWThreadMutex
#define SYNCH_CONDITION ConditionThreadMutex
#define SYNCH_RECURSIVE_CONDITION ConditionRecursiveThreadMutex
#define SYNCH_SEMAPHORE ThreadSemaphore
#define SYNCH_NULL_SEMAPHORE  NullSemaphore

#else /* ACE_HAS_THREADS */

#define SYNCH_MUTEX NullMutex
#define SYNCH_NULL_MUTEX NullMutex
#define SYNCH_RECURSIVE_MUTEX NullMutex
#define SYNCH_RW_MUTEX NullMutex
#define SYNCH_CONDITION NullCondition
#define SYNCH_RECURSIVE_CONDITION NullCondition
#define SYNCH_SEMAPHORE NullSemaphore
#define SYNCH_NULL_SEMAPHORE NullMutex

#endif /* ACE_HAS_THREADS */
#endif /* ACE_HAS_TEMPLATE_TYPEDEFS */

// These are available on *all* platforms
#define SYNCH_PROCESS_SEMAPHORE ProcessSemaphore
#define SYNCH_PROCESS_MUTEX  ProcessMutex

#if defined (ACE_HAS_THREADS)
#define SYNCH MT_SYNCH
#else /* ACE_HAS_THREADS */
#define SYNCH NULL_SYNCH
#endif /* ACE_HAS_THREADS */

#endif //ICC_SYNCH_TRAITS_H

