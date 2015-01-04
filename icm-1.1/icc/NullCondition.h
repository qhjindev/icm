#ifndef ICC_NULL_CONDITION_H
#define ICC_NULL_CONDITION_H


#include "icc/NullMutex.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_errno.h"

class Time_Value;

/**
 * class NullCondition
 *
 * Implement a do nothing Condition variable wrapper,
 * i.e., all methods are no ops.  This class is necessary since
 * some C++ compilers are *very* lame...
 */
class NullCondition
{
public:
  NullCondition (const NullMutex &m,
                      const char * = 0,
                      void * = 0)
    : m_mutex ((NullMutex &) m) {}

  ~NullCondition (void) {}

  /// Returns 0.
  int remove (void) {return 0;}

  /// Returns -1 with @c errno == @c ETIME.
  int wait (const TimeValue * = 0) {errno = ETIME; return -1;}

  /// Returns -1 with @c errno == @c ETIME.
  int wait (NullMutex &,
            const TimeValue * = 0) {errno = ETIME; return -1;}

  /// Returns 0.
  int signal (void) {return 0;}

  /// Returns 0.
  int broadcast (void) {return 0;}
  NullMutex &mutex (void) {return this->m_mutex;};

  /// Dump the state of an object.
  void dump (void) const {}

  // ACE_ALLOC_HOOK_DECLARE;
  // Declare the dynamic allocation hooks.

protected:
  NullMutex &m_mutex; // Reference to mutex lock.

private:
  // = Prevent assignment and initialization.
  void operator= (const NullCondition &);
  NullCondition (const NullCondition &);
};

#endif //ICC_NULL_CONDITION_H
