// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_wait.h
 *
 *  $Id: OS_NS_sys_wait.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_SYS_WAIT_H
# define OS_NS_SYS_WAIT_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_wait.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS
{

  /// Calls OS @c ::wait function, so it's only portable to UNIX/POSIX
  /// platforms.
  ACE_NAMESPACE_INLINE_FUNCTION
  pid_t wait (int * = 0);

  /**
   * Calls @c ::WaitForSingleObject on Win32 and ACE::waitpid ()
   * otherwise.  Returns the passed in @a pid_t on success and -1 on
   * failure.
   * On Win32, @a pid is ignored if the @a handle is not equal to 0.
   * Passing the process @a handle is prefer on Win32 because using
   * @a pid to wait on the project doesn't always work correctly
   * if the waited process has already terminated.
   */
  ACE_NAMESPACE_INLINE_FUNCTION
  pid_t wait (pid_t pid,
              ACE_exitcode *status,
              int wait_options = 0,
              ACE_HANDLE handle = 0);

  /**
   * Calls @c ::waitpid on UNIX/POSIX platforms Does not work on Vxworks 5.5.x.
   * On Win32, @a pid is ignored if the @a handle is not equal to 0.
   * Passing the process @a handle is prefer on Win32 because using
   * @a pid to wait on the project doesn't always work correctly
   * if the waited process has already terminated.
   */
  ACE_NAMESPACE_INLINE_FUNCTION
  pid_t waitpid (pid_t pid,
                 ACE_exitcode *status = 0,
                 int wait_options = 0,
                 ACE_HANDLE handle = 0);

} /* namespace OS */




#endif /* OS_NS_SYS_WAIT_H */
