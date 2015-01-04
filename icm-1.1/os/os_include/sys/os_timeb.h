// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_timeb.h
 *
 *  additional definitions for date and time
 *
 *  $Id: os_timeb.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_TIMEB_H
#define OS_INCLUDE_SYS_OS_TIMEB_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_types.h"

#if !defined (ACE_LACKS_SYS_TIMEB_H)
#  include /**/ <sys/timeb.h>
#endif /* !ACE_LACKS_SYS_TIMEB_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined (__BORLANDC__) && (__BORLANDC__ <= 0x560)
#  define _ftime ftime
#  define _timeb timeb
#endif /* __BORLANDC__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_TIMEB_H */
