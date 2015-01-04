#ifndef ICC_LOCK_H
#define ICC_LOCK_H


#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/**
 * Lock
 *
 * This is the abstract base class that contains the uniform
 * locking API that is supported by all the ACE synchronization
 * mechanisms.
 *
 * This class is typically used in conjunction with the
 * LockAdapter in order to provide a polymorphic
 * interface to the ACE synchronization mechanisms (e.g.,
 * ACE_Mutex, ACE_Semaphore, ACE_RW_Mutex, etc).  Note that
 * the reason that all of ACE doesn't use polymorphic locks is
 * that (1) they add ~20% extra overhead for virtual function
 * calls and (2) objects with virtual functions can't be placed
 * into shared memory.
 */
class Lock
{
public:
  /// CE needs a default ctor here.
  Lock (void);

  /// Noop virtual destructor
  virtual ~Lock (void);

  /**
   * Explicitly destroy the lock.  Note that only one thread should
   * call this method since it doesn't protect against race
   * conditions.
   */
  virtual int remove (void) = 0;

  /// Block the thread until the lock is acquired.  Returns -1 on
  /// failure.
  virtual int acquire (void) = 0;

  /**
   * Conditionally acquire the lock (i.e., won't block).  Returns -1
   * on failure.  If we "failed" because someone else already had the
   * lock, @c errno is set to @c EBUSY.
   */
  virtual int tryAcquire (void) = 0;

  /// Release the lock.  Returns -1 on failure.
  virtual int release (void) = 0;

  /**
   * Block until the thread acquires a read lock.  If the locking
   * mechanism doesn't support read locks then this just calls
   * <acquire>.  Returns -1 on failure.
   */
  virtual int acquireRead (void) = 0;

  /**
   * Block until the thread acquires a write lock.  If the locking
   * mechanism doesn't support read locks then this just calls
   * <acquire>.  Returns -1 on failure.
   */
  virtual int acquireWrite (void) = 0;

  /**
   * Conditionally acquire a read lock.  If the locking mechanism
   * doesn't support read locks then this just calls <acquire>.
   * Returns -1 on failure.  If we "failed" because someone else
   * already had the lock, @c errno is set to @c EBUSY.
   */
  virtual int tryAcquireRead (void) = 0;

  /**
   * Conditionally acquire a write lock.  If the locking mechanism
   * doesn't support read locks then this just calls <acquire>.
   * Returns -1 on failure.  If we "failed" because someone else
   * already had the lock, @c errno is set to @c EBUSY.
   */
  virtual int tryAcquireWrite (void) = 0;

  /**
   * Conditionally try to upgrade a lock held for read to a write lock.
   * If the locking mechanism doesn't support read locks then this just
   * calls <acquire>. Returns 0 on success, -1 on failure.
   */
  virtual int tryAcquireWriteUpgrade (void) = 0;
};

#endif //ICC_LOCK_H
