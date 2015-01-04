// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_local.h
 *
 *  category macros
 *
 *  $Id: os_local.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_LOCAL_H
#define OS_INCLUDE_OS_LOCAL_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_stddef.h"

#if !defined (ACE_LACKS_LOCAL_H)
# include /**/ <local.h>
#endif /* !ACE_LACKS_LOCAL_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_LOCAL_H */
