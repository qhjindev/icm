// $Id: OS_NS_sys_select.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_sys_select.h"

#include "os/OS_NS_errno.h"
#include "os/OS_NS_macros.h"
#include "icc/TimeValue.h"



// It would be really cool to add another version of select that would
// function like the one we're defending against below!
ACE_INLINE int
OS::select (int width,
                fd_set *rfds, fd_set *wfds, fd_set *efds,
                const TimeValue *timeout)
{
  OS_TRACE ("OS::select");
#if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
  // We must defend against non-conformity!
  timeval copy;
  timeval *timep = 0;

  if (timeout != 0)
    {
      copy = *timeout;
      timep = &copy;
    }
  else
    timep = 0;
#else
  const timeval *timep = (timeout == 0 ? (const timeval *)0 : *timeout);
#endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
#if defined (ACE_LACKS_SELECT)
  ACE_UNUSED_ARG (width);
  ACE_UNUSED_ARG (rfds);
  ACE_UNUSED_ARG (wfds);
  ACE_UNUSED_ARG (efds);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#elif defined(ACE_TANDEM_T1248_PTHREADS)
  ACE_SOCKCALL_RETURN (::spt_select (width, rfds, wfds, efds, timep),
                       int, -1);
#else
  ACE_SOCKCALL_RETURN (::select (width, rfds, wfds, efds, timep),
                       int, -1);
#endif
}

ACE_INLINE int
OS::select (int width,
                fd_set *rfds, fd_set *wfds, fd_set *efds,
                const TimeValue &timeout)
{
  OS_TRACE ("OS::select");
#if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
# define ___ACE_TIMEOUT &copy
  timeval copy = timeout;
#else
# define ___ACE_TIMEOUT timep
  const timeval *timep = timeout;
#endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
#if defined (ACE_LACKS_SELECT)
  ACE_UNUSED_ARG (width);
  ACE_UNUSED_ARG (rfds);
  ACE_UNUSED_ARG (wfds);
  ACE_UNUSED_ARG (efds);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#elif defined(ACE_TANDEM_T1248_PTHREADS)
  ACE_SOCKCALL_RETURN (::spt_select (width, rfds, wfds, efds, ___ACE_TIMEOUT),
                       int, -1);
#else
  ACE_SOCKCALL_RETURN (::select (width, rfds, wfds, efds, ___ACE_TIMEOUT),
                       int, -1);
#endif
#undef ___ACE_TIMEOUT
}



