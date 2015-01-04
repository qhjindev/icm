// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_ipc.h
 *
 *  XSI interprocess communication access structure
 *
 *  $Id: os_ipc.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_IPC_H
#define OS_INCLUDE_SYS_OS_IPC_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_types.h"

#if !defined (ACE_LACKS_SYS_IPC_H)
#  include /**/ <sys/ipc.h>
#endif /* !ACE_LACKS_SYS_IPC_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined (ACE_WIN32)
#  define ACE_INVALID_SEM_KEY 0
#else /* !ACE_WIN32 */
#  define ACE_INVALID_SEM_KEY -1
#endif /* ACE_WIN32 */

#if !defined (IPC_PRIVATE)
#  define IPC_PRIVATE ACE_INVALID_SEM_KEY
#endif /* IPC_PRIVATE */

#if !defined (IPC_STAT)
#  define IPC_STAT 0
#endif /* IPC_STAT */

#if !defined (IPC_CREAT)
#  define IPC_CREAT 0
#endif /* IPC_CREAT */

#if !defined (IPC_NOWAIT)
#  define IPC_NOWAIT 0
#endif /* IPC_NOWAIT */

#if !defined (IPC_RMID)
#  define IPC_RMID 0
#endif /* IPC_RMID */

#if !defined (IPC_EXCL)
#  define IPC_EXCL 0
#endif /* IPC_EXCL */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_IPC_H */
