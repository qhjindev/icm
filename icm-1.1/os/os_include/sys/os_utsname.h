// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_utsname.h
 *
 *  system name structure
 *
 *  $Id: os_utsname.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_UTSNAME_H
#define OS_INCLUDE_SYS_OS_UTSNAME_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (ACE_LACKS_SYS_UTSNAME_H)
#  include /**/ <sys/utsname.h>
#endif /* !ACE_LACKS_SYS_UTSNAME_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_UTSNAME_H */
