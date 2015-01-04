
#include "icc/Guard.h"

template <class LOCK> int
Guard<LOCK>::acquire (void)
{
  return this->m_owner = this->m_lock->acquire ();
}

template <class LOCK>  int
Guard<LOCK>::tryAcquire (void)
{
  return this->m_owner = this->m_lock->tryAquire ();
}

template <class LOCK>  int
Guard<LOCK>::release (void)
{
  if (this->m_owner == -1)
    return -1;
  else
    {
      this->m_owner = -1;
      return this->m_lock->release ();
    }
}

template <class LOCK>
Guard<LOCK>::Guard (LOCK &l)
  : m_lock (&l),
    m_owner (0)
{
  this->acquire ();
}

template <class LOCK>
Guard<LOCK>::Guard (LOCK &l, int block)
  : m_lock (&l),
    m_owner (0)
{
  if (block)
    this->acquire ();
  else
    this->tryAcquire ();
}

template <class LOCK>
Guard<LOCK>::Guard (LOCK &l, int block, int becomeOwner)
  : m_lock (&l),
    m_owner (becomeOwner == 0 ? -1 : 0)
{
  //ACE_UNUSED_ARG (block);
}

// Implicitly and automatically acquire (or try to acquire) the
// lock.

template <class LOCK>
Guard<LOCK>::~Guard (void)
{
  this->release ();
}

template <class LOCK>  int
Guard<LOCK>::locked (void) const
{
  return this->m_owner != -1;
}

template <class LOCK>  int
Guard<LOCK>::remove (void)
{
  return this->m_lock->remove ();
}

template <class LOCK>  void
Guard<LOCK>::disown (void)
{
  this->m_owner = -1;
}

template <class LOCK> void
Guard<LOCK>::dump (void) const
{

}

template <class LOCK>
WriteGuard<LOCK>::WriteGuard (LOCK &m)
  : Guard<LOCK> (&m)
{
  this->acquire_write ();
}

template <class LOCK>  int
WriteGuard<LOCK>::acquireWrite (void)
{
  return this->m_owner = this->m_lock->acquireWrite ();
}

template <class LOCK>  int
WriteGuard<LOCK>::acquire (void)
{
  return this->m_owner = this->m_lock->acquireWrite ();
}

template <class LOCK>  int
WriteGuard<LOCK>::tryAcquireWrite (void)
{
  return this->m_owner = this->m_lock->tryAcquire_write ();
}

template <class LOCK>  int
WriteGuard<LOCK>::tryAcquire (void)
{
  return this->m_owner = this->m_lock->tryAcquire_write ();
}

template <class LOCK>
WriteGuard<LOCK>::WriteGuard (LOCK &m,int block)
  : Guard<LOCK> (&m)
{
  if (block)
    this->acquireWrite ();
  else
    this->tryAcquireWrite ();
}

template <class LOCK> void
WriteGuard<LOCK>::dump (void) const
{

}

template <class LOCK>  int
ReadGuard<LOCK>::acquireRead (void)
{
  return this->m_owner = this->m_lock->acquireRead ();
}

template <class LOCK>  int
ReadGuard<LOCK>::acquire (void)
{
  return this->m_owner = this->m_lock->acquireRead ();
}

template <class LOCK>  int
ReadGuard<LOCK>::tryAcquireRead (void)
{
  return this->m_owner = this->m_lock->tryAcquireRead ();
}

template <class LOCK>  int
ReadGuard<LOCK>::tryAcquire (void)
{
  return this->m_owner = this->m_lock->tryAcquireRead ();
}

template <class LOCK>
ReadGuard<LOCK>::ReadGuard (LOCK &m)
  : Guard<LOCK> (&m)
{
  this->acquireRead ();
}

template <class LOCK>
ReadGuard<LOCK>::ReadGuard (LOCK &m,int block)
  : Guard<LOCK> (&m)
{
  if (block)
    this->acquireRead ();
  else
    this->tryAcquireRead ();
}

template <class LOCK> void
ReadGuard<LOCK>::dump (void) const
{

}
