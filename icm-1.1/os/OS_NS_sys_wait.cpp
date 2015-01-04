// $Id: OS_NS_sys_wait.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_sys_wait.h"

#include "os/OS_NS_errno.h"
#include "os/Global_Macros.h"



ACE_INLINE pid_t
OS::wait (int *status)
{
  OS_TRACE ("OS::wait");
#if defined (ACE_LACKS_WAIT)
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (0);
#else
  OSCALL_RETURN (::wait (status), pid_t, -1);
#endif /* ACE_LACKS_WAIT */
}

ACE_INLINE pid_t
OS::waitpid (pid_t pid,
                 ACE_exitcode *status,
                 int wait_options,
                 ACE_HANDLE handle)
{
  OS_TRACE ("OS::waitpid");
#if defined (ACE_LACKS_WAITPID)
  ACE_UNUSED_ARG (pid);
  ACE_UNUSED_ARG (status);
  ACE_UNUSED_ARG (wait_options);
  ACE_UNUSED_ARG (handle);

  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_WIN32)
  int blocking_period = ACE_BIT_ENABLED (wait_options, WNOHANG)
    ? 0 /* don't hang */
    : INFINITE;

  ACE_HANDLE phandle = handle;

  if (phandle == 0)
    {
      phandle = ::OpenProcess (SYNCHRONIZE,
                               FALSE,
                               pid);

      if (phandle == 0)
        {
          OS::set_errno_to_last_error ();
          return -1;
        }
    }

  pid_t result = pid;

  // Don't try to get the process exit status if wait failed so we can
  // keep the original error code intact.
  switch (::WaitForSingleObject (phandle,
                                 blocking_period))
    {
    case WAIT_OBJECT_0:
      if (status != 0)
        // The error status of <GetExitCodeProcess> is nonetheless
        // not tested because we don't know how to return the value.
        ::GetExitCodeProcess (phandle,
                              status);
      break;
    case WAIT_TIMEOUT:
      errno = ETIME;
      result = 0;
      break;
    default:
      OS::set_errno_to_last_error ();
      result = -1;
    }
  if (handle == 0)
    ::CloseHandle (phandle);
  return result;
#elif defined(ACE_TANDEM_T1248_PTHREADS)
  ACE_UNUSED_ARG (handle);
  OSCALL_RETURN (::spt_waitpid (pid, status, wait_options),
                     pid_t, -1);
#else
  ACE_UNUSED_ARG (handle);
  OSCALL_RETURN (::waitpid (pid, status, wait_options),
                     pid_t, -1);
#endif /* ACE_LACKS_WAITPID */
}

ACE_INLINE pid_t
OS::wait (pid_t pid,
              ACE_exitcode *status,
              int wait_options,
              ACE_HANDLE handle)
{
  OS_TRACE ("OS::wait");
  return OS::waitpid (pid,
                          status,
                          wait_options,
                          handle);
}


