// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_wchar.h
 *
 *  wide-character handling
 *
 *  $Id: os_wchar.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_WCHAR_H
#define OS_INCLUDE_OS_WCHAR_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// ctype.h, string.h, stdarg.h, stdio.h, stdlib.h, time.h
#include "os/os_include/os_stdio.h"
#include "os/os_include/os_stdlib.h"
#include "os/os_include/os_time.h"
#include "os/os_include/os_string.h"
#include "os/os_include/os_ctype.h"

#if !defined (ACE_LACKS_WCHAR_H)
#  include /**/ <wchar.h>
#endif /* !ACE_LACKS_WCHAR_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_WCHAR_H */
