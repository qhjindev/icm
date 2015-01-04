// -*- C++ -*-
// $Id: OS_NS_errno.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_errno.h"

ACE_RCSID(ace, OS_NS_errno, "$Id: OS_NS_errno.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

#include "os/OS_Memory.h"

#if defined (ACE_LACKS_ALPHASORT)
# include "os/OS_NS_string.h"
#endif /* ACE_LACKS_ALPHASORT */


#include "os/config-all.h"    /* Need ACE_TRACE */



ACE_INLINE int
OS::last_error (void)
{
  // OS_TRACE ("OS::last_error");

#if defined (ACE_WIN32)
  // OS::last_error() prefers errnor since started out as a way to
  // avoid directly accessing errno in ACE code - particularly the ACE
  // C++ socket wrapper facades.  On Windows, some things that would
  // use errno on UNIX require ::GetLastError(), so this method tries
  // to shield the rest of ACE from having to know about this.
  int lerror = ::GetLastError ();
  int lerrno = errno;
  return lerrno == 0 ? lerror : lerrno;
#else
  return errno;
#endif /* ACE_WIN32 */
}

ACE_INLINE void
OS::last_error (int error)
{
  OS_TRACE ("OS::last_error");
#if defined (ACE_WIN32)
  ::SetLastError (error);
#endif /* ACE_WIN32 */
  errno = error;
}

ACE_INLINE int
OS::map_errno (int error)
{
#if defined (ACE_WIN32)
  switch (error)
    {
    case WSAEWOULDBLOCK:
      return EAGAIN; // Same as UNIX errno EWOULDBLOCK.
    }
#endif /* ACE_WIN32 */

  return error;
}

ACE_INLINE int
OS::set_errno_to_last_error (void)
{
# if defined (ACE_WIN32)
  return errno = ::GetLastError ();
#else
  return errno;
# endif /* defined(ACE_WIN32) */
}

ACE_INLINE int
OS::set_errno_to_wsa_last_error (void)
{
# if defined (ACE_WIN32)
  return errno = ::WSAGetLastError ();
#else
  return errno;
# endif /* defined(ACE_WIN32) */
}

#if defined (ACE_HAS_WINCE_BROKEN_ERRNO)

ACE_INLINE ACE_CE_Errno *
ACE_CE_Errno::instance ()
{
  // This should be inlined.
  return ACE_CE_Errno::instance_;
}

ACE_INLINE
ACE_CE_Errno::operator int (void) const
{
  return (int) TlsGetValue (ACE_CE_Errno::errno_key_);
}

ACE_INLINE int
ACE_CE_Errno::operator= (int x)
{
  // error checking?
  TlsSetValue (ACE_CE_Errno::errno_key_, (void *) x);
  return x;
}

#endif /* ACE_HAS_WINCE_BROKEN_ERRNO */


