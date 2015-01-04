// -*- C++ -*-
// $Id: OS_Errno.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_Errno.h"

ACE_RCSID(ace, OS_Errno, "$Id: OS_Errno.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")


ACE_INLINE
ACE_Errno_Guard::ACE_Errno_Guard (ACE_ERRNO_TYPE &errno_ref,
                                  int error)
  :
#if defined (ACE_MT_SAFE)
    errno_ptr_ (&errno_ref),
#endif /* ACE_MT_SAFE */
    error_ (error)
{
#if !defined(ACE_MT_SAFE)
  ACE_UNUSED_ARG (errno_ref);
#endif /* ACE_MT_SAFE */
}

ACE_INLINE
ACE_Errno_Guard::ACE_Errno_Guard (ACE_ERRNO_TYPE &errno_ref)
  :
#if defined (ACE_MT_SAFE)
    errno_ptr_ (&errno_ref),
#endif /* ACE_MT_SAFE */
    error_ (errno_ref)
{
}

ACE_INLINE
ACE_Errno_Guard::~ACE_Errno_Guard (void)
{
#if defined (ACE_MT_SAFE)
  *errno_ptr_ = this->error_;
#else
  errno = this->error_;
#endif /* ACE_MT_SAFE */
}

#if defined (ACE_HAS_WINCE_BROKEN_ERRNO)
ACE_INLINE int
ACE_Errno_Guard::operator= (const ACE_ERRNO_TYPE &error)
{
  return this->error_ = error;
}
#endif /* ACE_HAS_WINCE_BROKEN_ERRNO */

ACE_INLINE int
ACE_Errno_Guard::operator= (int error)
{
  return this->error_ = error;
}

ACE_INLINE bool
ACE_Errno_Guard::operator== (int error)
{
  return this->error_ == error;
}

ACE_INLINE bool
ACE_Errno_Guard::operator!= (int error)
{
  return this->error_ != error;
}



#if defined (ACE_HAS_WINCE_BROKEN_ERRNO)

#include "os/OS_Memory.h"



ACE_CE_Errno *ACE_CE_Errno::instance_ = 0;
DWORD ACE_CE_Errno::errno_key_ = 0xffffffff;

void
ACE_CE_Errno::init ()
{
  ACE_NEW (ACE_CE_Errno::instance_,
           ACE_CE_Errno ());
  ACE_CE_Errno::errno_key_ = TlsAlloc ();
}

void
ACE_CE_Errno::fini ()
{
  TlsFree (ACE_CE_Errno::errno_key_);
  delete ACE_CE_Errno::instance_;
  ACE_CE_Errno::instance_ = 0;
}



#endif /* ACE_HAS_WINCE_BROKEN_ERRNO */
