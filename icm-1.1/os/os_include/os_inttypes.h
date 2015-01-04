// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_inttypes.h
 *
 *  fixed size integer types
 *
 *  $Id: os_inttypes.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_INTTYPES_H
#define OS_INCLUDE_OS_INTTYPES_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_stdint.h"

#if !defined (ACE_LACKS_INTTYPES_H)
# include /**/ <inttypes.h>
#endif /* !ACE_LACKS_INTTYPES_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

// @todo if needbe, we can define the macros if they aren't available.

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_INTTYPES_H */
