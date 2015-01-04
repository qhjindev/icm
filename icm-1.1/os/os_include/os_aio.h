// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_aio.h
 *
 *  asynchronous input and output (REALTIME)
 *
 *  $Id: os_aio.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_AIO_H
#define OS_INCLUDE_OS_AIO_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// Inclusion of the <aio.h> header may make visible symbols defined in
// the headers <fcntl.h>, <signal.h>, <sys/types.h>, and <time.h>.

#include "os/os_include/os_signal.h" // for sigevent

#if !defined (ACE_LACKS_AIO_H)
# include /**/ <aio.h>
#endif /* !ACE_LACKS_AIO_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_AIO_H */
