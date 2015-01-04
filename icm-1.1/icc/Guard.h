#ifndef ICC_GUARD_H
#define ICC_GUARD_H

#include "icc/Lock.h"

template <class LOCK>
class Guard
{
public:
	Guard (LOCK &l);
	Guard (LOCK &l, int block);
	Guard (LOCK &l, int block, int becomeOwner);
	~Guard (void);

	int acquire (void);
	int tryAcquire (void);
	int release (void);
	void disown (void);

	int locked (void) const;
	int remove (void);

	void dump (void) const;
protected:
	Guard (LOCK *lock) : m_lock (lock) {}

	LOCK *m_lock;

	int m_owner;
};

template <class LOCK>
class WriteGuard : public Guard<LOCK>
{
public:
  // = Initialization method.

  /// Implicitly and automatically acquire a write lock.
  WriteGuard (LOCK &m);

  /// Implicitly and automatically acquire (or try to acquire) a write
  /// lock.
  WriteGuard (LOCK &m, int block);

  // = Lock accessors.

  /// Explicitly acquire the write lock.
  int acquireWrite (void);

  /// Explicitly acquire the write lock.
  int acquire (void);

  /// Conditionally acquire the write lock (i.e., won't block).
  int tryAcquireWrite (void);

  /// Conditionally acquire the write lock (i.e., won't block).
  int tryAcquire (void);

  // = Utility methods.

  /// Dump the state of an object.
  void dump (void) const;

};

template <class LOCK>
class ReadGuard : public Guard<LOCK>
{
public:
  // = Initialization methods.

  /// Implicitly and automatically acquire a read lock.
  ReadGuard (LOCK& m);

  /// Implicitly and automatically acquire (or try to acquire) a read
  /// lock.
  ReadGuard (LOCK &m, int block);

  // = Lock accessors.

  /// Explicitly acquire the read lock.
  int acquireRead (void);

  /// Explicitly acquire the read lock.
  int acquire (void);

  /// Conditionally acquire the read lock (i.e., won't block).
  int tryAcquireRead (void);

  /// Conditionally acquire the read lock (i.e., won't block).
  int tryAcquire (void);

  // = Utility methods.

  /// Dump the state of an object.
  void dump (void) const;

  // ACE_ALLOC_HOOK_DECLARE;
  // Declare the dynamic allocation hooks.
};


#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "Guard.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#endif //ICC_GUARD_H
