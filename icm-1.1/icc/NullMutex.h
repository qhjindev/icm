#ifndef ICC_NULL_MUTEX_H
#define ICC_NULL_MUTEX_H


#include "os/os_include/os_errno.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/Global_Macros.h"
#include "os/OS_Memory.h"


class TimeValue;

/**
 * class NullMutex
 *
 * Implement a do nothing ACE_Mutex, i.e., all the methods are
 * no ops.
 */
class NullMutex
{
public:
  NullMutex (const ACE_TCHAR * = 0)
    : lock (0) {}
  ~NullMutex (void) {}
  /// Return 0.
  int remove (void) {return 0;}

  /// Return 0.
  int acquire (void) {return 0;}

  /// Return -1 with @c errno == @c ETIME.
  int acquire (TimeValue &) {errno = ETIME; return -1;}

  /// Return -1 with @c errno == @c ETIME.
  int acquire (TimeValue *) {errno = ETIME; return -1;}

  /// Return 0.
  int tryAcquire (void) {return 0;}

  /// Return 0.
  int release (void) {return 0;}

  /// Return 0.
  int acquireWrite (void) {return 0;}

  /// Return 0.
  int tryAcquireWrite (void) {return 0;}

  /// Return 0.
  int tryaAquireWriteUpgrade (void) {return 0;}

  /// Return 0.
  int acquireRead (void) {return 0;}

  /// Return 0.
  int tryAcquireRead (void) {return 0;}

  /// Dump the state of an object.
  void dump (void) const {}

  int lock; // A dummy lock.
};


template <class LOCK>
class Guard;

/**
 * @class Guard<NullMutex>
 *
 * @brief Template specialization of Guard for the
 * NullMutex.
 *
 * This specialization is useful since it helps to speedup
 * performance of the "Null_Mutex" considerably.
 */
template<>
class Guard<NullMutex>
{
public:
  // = Initialization and termination methods.
  Guard (NullMutex &) {}
  Guard (NullMutex &, int) {}
  Guard (NullMutex &, int, int) {}
#if defined (ACE_WIN32)
  ~Guard (void) {}
#endif /* ACE_WIN32 */

  int acquire (void) { return 0; }
  int tryAcquire (void) { return 0; }
  int release (void) { return 0; }
  void disown (void) {}
  int locked (void) { return 1; }
  int remove (void) { return 0; }
  void dump (void) const {}

private:

  // Disallow copying and assignment.
  Guard (const Guard<NullMutex> &);
  void operator= (const Guard<NullMutex> &);

};

template <class ACE_LOCK>
class WriteGuard;

/**
 * @class WriteGuard<NullMutex>
 *
 */
template<>
class ACE_Export WriteGuard<NullMutex>
  : public Guard<NullMutex>
{
public:
  WriteGuard (NullMutex &m)
    : Guard<NullMutex> (m) {}
  WriteGuard (NullMutex &m, int blocked)
    : Guard<NullMutex> (m, blocked) {}

  int acquireWrite (void) { return 0; }
  int acquire (void) { return 0; }
  int tryAcquireWrite (void) { return 0; }
  int tryAcquire (void) { return 0; }
  void dump (void) const {}
};

template <class ACE_LOCK>
class ReadGuard;

/**
 * @class ReadGuard<NullMutex>
 *
 */
template<>
class ReadGuard<NullMutex>
  : public Guard<NullMutex>
{
public:
  ReadGuard (NullMutex &m)
    : Guard<NullMutex> (m) {}
  ReadGuard (NullMutex &m, int blocked)
    : Guard<NullMutex> (m, blocked) {}

  int acquireRead (void) { return 0; }
  int acquire (void) { return 0; }
  int tryAcquireRead (void) { return 0; }
  int tryAcquire (void) { return 0; }
  void dump (void) const {}
};


#endif  //ICC_NULL_MUTEX_H
