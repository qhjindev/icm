// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_ucontext.h
 *
 *  user context
 *
 *  $Id: os_ucontext.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_UCONTEXT_H
#define OS_INCLUDE_OS_UCONTEXT_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_signal.h"

#if !defined (ACE_LACKS_UCONTEXT_H)
#  include /**/ <ucontext.h>
#endif /* !ACE_LACKS_ucontext_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

# if !defined (ACE_HAS_UCONTEXT_T)
typedef int ucontext_t;
# endif /* ACE_HAS_UCONTEXT_T */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_UCONTEXT_H */
