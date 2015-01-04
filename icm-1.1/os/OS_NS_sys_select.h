// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_select.h
 *
 *  $Id: OS_NS_sys_select.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_SYS_SELECT_H
# define OS_NS_SYS_SELECT_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

// The following is needed for Mac OSX 10.2 (Jaguar).  Mac OSX 10.3 (Panther)
// doesn't seem to have this issue.

#if defined (ACE_SYS_SELECT_NEEDS_UNISTD_H)
 #include "os/OS_NS_unistd.h"
#endif

#include "os/os_include/sys/os_select.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



class TimeValue;

namespace OS
{
  // Should be moved to cpp or inl.
  ACE_NAMESPACE_INLINE_FUNCTION
  int select (int width,
              fd_set *rfds,
              fd_set *wfds = 0,
              fd_set *efds = 0,
              const TimeValue *tv = 0);

  ACE_NAMESPACE_INLINE_FUNCTION
  int select (int width,
              fd_set *rfds,
              fd_set *wfds,
              fd_set *efds,
              const TimeValue &tv);

} /* namespace OS */



#endif /* OS_NS_SYS_SELECT_H */
