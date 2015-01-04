
#include "os/ACE.h"

#include "os/config-all.h"
#include "os/os_include/os_errno.h"
#include "os/OS_NS_sys_resource.h"
#include "os/OS_NS_unistd.h"
#include "os/OS_NS_string.h"

int
ACE::max_handles (void)
{
  //ACE_TRACE ("ACE::max_handles");
#if defined (RLIMIT_NOFILE) && !defined (ACE_LACKS_RLIMIT)
  rlimit rl;
  int r = OS::getrlimit (RLIMIT_NOFILE, &rl);
# if !defined (RLIM_INFINITY)
  if (r == 0)
    return rl.rlim_cur;
#else
  if (r == 0 && rl.rlim_cur != RLIM_INFINITY)
    return rl.rlim_cur;
  // If == RLIM_INFINITY, fall through to the ACE_LACKS_RLIMIT sections
# endif /* RLIM_INFINITY */
#endif /* RLIMIT_NOFILE && !ACE_LACKS_RLIMIT */

#if defined (_SC_OPEN_MAX)
  return OS::sysconf (_SC_OPEN_MAX);
#elif defined (ACE_VXWORKS) && (ACE_VXWORKS < 0x620)
  return maxFiles;
#elif defined (FD_SETSIZE)
  return FD_SETSIZE;
#else
  ACE_NOTSUP_RETURN (-1);
#endif /* _SC_OPEN_MAX */
}

// Set the number of currently open handles in the process.
//
// If NEW_LIMIT == -1 set the limit to the maximum allowable.
// Otherwise, set it to be the value of NEW_LIMIT.

int
ACE::set_handle_limit (int new_limit,
                       int increase_limit_only)
{
  //ACE_TRACE ("ACE::set_handle_limit");
  int cur_limit = ACE::max_handles ();
  int max_limit = cur_limit;

  if (cur_limit == -1)
    return -1;

#if !defined (ACE_LACKS_RLIMIT) && defined (RLIMIT_NOFILE)
  struct rlimit rl;

  OS::memset ((void *) &rl, 0, sizeof rl);
  int r = OS::getrlimit (RLIMIT_NOFILE, &rl);
  if (r == 0)
    max_limit = rl.rlim_max;
#endif /* ACE_LACKS_RLIMIT */

  if (new_limit == -1)
    new_limit = max_limit;

  if (new_limit < 0)
    {
      errno = EINVAL;
      return -1;
    }
  else if (new_limit > cur_limit)
    {
      // Increase the limit.
#if !defined (ACE_LACKS_RLIMIT) && defined (RLIMIT_NOFILE)
      rl.rlim_cur = new_limit;
      return OS::setrlimit (RLIMIT_NOFILE, &rl);
#else
      // Must return EINVAL errno.
      ACE_NOTSUP_RETURN (-1);
#endif /* ACE_LACKS_RLIMIT */
    }
  else if (increase_limit_only == 0)
    {
      // Decrease the limit.
#if !defined (ACE_LACKS_RLIMIT) && defined (RLIMIT_NOFILE)
      rl.rlim_cur = new_limit;
      return OS::setrlimit (RLIMIT_NOFILE, &rl);
#else
      // We give a chance to platforms without RLIMIT to work.
      // Instead of ACE_NOTSUP_RETURN (0), just return 0 because
      // new_limit is <= cur_limit, so it's a no-op.
      return 0;
#endif /* ACE_LACKS_RLIMIT */
    }

  return 0;
}
