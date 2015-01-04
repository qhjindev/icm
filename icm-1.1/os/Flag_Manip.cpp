// $Id: Flag_Manip.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/Flag_Manip.h"

#if defined (ACE_LACKS_FCNTL)
#  include "os/OS_NS_stropts.h"
#  include "os/OS_NS_errno.h"
#endif /* ACE_LACKS_FCNTL */

#include "os/OS_NS_fcntl.h"



// Return flags currently associated with handle.
ACE_INLINE int
ACE::get_flags (ACE_HANDLE handle)
{
  ACE_TRACE ("ACE::get_flags");

#if defined (ACE_LACKS_FCNTL)
  // OS::fcntl is not supported.  It
  // would be better to store ACE's notion of the flags
  // associated with the handle, but this works for now.
  ACE_UNUSED_ARG (handle);
  return 0;
#else
  return OS::fcntl (handle, F_GETFL, 0);
#endif /* ACE_LACKS_FCNTL */
}


#if defined (CYGWIN32)
#  include "ace/os_include/os_termios.h"
#endif /* CYGWIN32 */

ACE_RCSID (ace,
           Flag_Manip,
           "$Id: Flag_Manip.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")



// Flags are file status flags to turn on.

int
ACE::set_flags (ACE_HANDLE handle, int flags)
{
  ACE_TRACE ("ACE::set_flags");
#if defined (ACE_LACKS_FCNTL)
  switch (flags)
    {
    case ACE_NONBLOCK:
      // nonblocking argument (1)
      // blocking:            (0)
      {
        int nonblock = 1;
        return OS::ioctl (handle, FIONBIO, &nonblock);
      }
    default:
      ACE_NOTSUP_RETURN (-1);
    }
#else
  int val = OS::fcntl (handle, F_GETFL, 0);

  if (val == -1)
    return -1;

  // Turn on flags.
  ACE_SET_BITS (val, flags);

  if (OS::fcntl (handle, F_SETFL, val) == -1)
    return -1;
  else
    return 0;
#endif /* ACE_LACKS_FCNTL */
}

// Flags are the file status flags to turn off.

int
ACE::clr_flags (ACE_HANDLE handle, int flags)
{
  ACE_TRACE ("ACE::clr_flags");

#if defined (ACE_LACKS_FCNTL)
  switch (flags)
    {
    case ACE_NONBLOCK:
      // nonblocking argument (1)
      // blocking:            (0)
      {
        int nonblock = 0;
        return OS::ioctl (handle, FIONBIO, &nonblock);
      }
    default:
      ACE_NOTSUP_RETURN (-1);
    }
#else
  int val = OS::fcntl (handle, F_GETFL, 0);

  if (val == -1)
    return -1;

  // Turn flags off.
  ACE_CLR_BITS (val, flags);

  if (OS::fcntl (handle, F_SETFL, val) == -1)
    return -1;
  else
    return 0;
#endif /* ACE_LACKS_FCNTL */
}


