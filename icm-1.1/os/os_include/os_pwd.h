// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_pwd.h
 *
 *  password structure
 *
 *  $Id: os_pwd.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_PWD_H
#define OS_INCLUDE_OS_PWD_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_types.h"

#if !defined (ACE_LACKS_PWD_H)
# include /**/ <pwd.h>
#endif /* !ACE_LACKS_PWD_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if !defined (ACE_WIN32)
// VAC++ doesn't correctly grok the ::getpwnam_r - the function is redefined
// in pwd.h, and that redefinition is used here
#  if defined (_AIX) && defined (__IBMCPP__) && (__IBMCPP__ >= 400)
  extern int _posix_getpwnam_r(const char *, struct passwd *, char *,
                               int, struct passwd **);
#  endif /* AIX and VAC++ 4 */
#endif /* !ACE_WIN32 */

#if defined (DIGITAL_UNIX)
  extern int _Pgetpwnam_r (const char *, struct passwd *,
                           char *, size_t, struct passwd **);
#endif /* DIGITAL_UNIX */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_PWD_H */
