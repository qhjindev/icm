// -*- C++ -*-

//=============================================================================
/**
 *  @file os_time.h
 *
 *  time types
 *
 *  $Id: os_time.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_TIME_H
#define OS_INCLUDE_SYS_OS_TIME_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_types.h"

#if !defined (ACE_LACKS_SYS_TIME_H)
#  include /**/ <sys/time.h>
#endif /* !ACE_LACKS_SYS_TIME_H */

#if defined (ACE_VXWORKS) && (ACE_VXWORKS == 0x620)
#  include /**/ <time.h> // VxWorks 6.2 defined timeval in time.h
#endif

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined (ACE_HAS_SVR4_GETTIMEOFDAY)
# if !defined (SCO)
  int gettimeofday (struct timeval *tp, void * = 0);
# else
  int gettimeofday (struct timeval *tp);
# endif  /* !SCO */
#elif defined (ACE_HAS_OSF1_GETTIMEOFDAY)
  int gettimeofday (struct timeval *tp, struct timezone * = 0);
#elif defined (ACE_HAS_VOIDPTR_GETTIMEOFDAY)
# define ACE_HAS_SVR4_GETTIMEOFDAY
#endif /* ACE_HAS_SVR4_GETTIMEOFDAY */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_TIME_H */
