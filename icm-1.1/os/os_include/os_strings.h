// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_strings.h
 *
 *  string operations
 *
 *  $Id: os_strings.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_STRINGS_H
#define OS_INCLUDE_OS_STRINGS_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_stddef.h"

#if !defined (ACE_LACKS_STRINGS_H)
#  include /**/ <strings.h>
#endif /* !ACE_LACKS_STRINGS_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined (ACE_LACKS_STRCASECMP_PROTOTYPE)
  int strcasecmp(const char *, const char *);
#endif /* ACE_LACKS_STRCASECMP_PROTOTYPE */

#if defined (ACE_LACKS_STRNCASECMP_PROTOTYPE)
  int strncasecmp(const char *, const char *, size_t);
#endif /* ACE_LACKS_STRNCASECMP_PROTOTYPE */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_STRINGS_H */
