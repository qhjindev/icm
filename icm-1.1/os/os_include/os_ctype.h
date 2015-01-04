// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_ctype.h
 *
 *  character types
 *
 *  $Id: os_ctype.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_CTYPE_H
#define OS_INCLUDE_OS_CTYPE_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (ACE_LACKS_CTYPE_H)
# include /**/ <ctype.h>
#endif /* !ACE_LACKS_CTYPE_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

// @todo move the is* and is* emulation methods in OS here
// and let OS just call them.

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_CTYPE_H */
