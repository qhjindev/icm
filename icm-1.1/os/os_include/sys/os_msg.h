// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_msg.h
 *
 *  XSI message queue structures
 *
 *  $Id: os_msg.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_MSG_H
#define OS_INCLUDE_SYS_OS_MSG_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_ipc.h"

#if !defined (ACE_LACKS_SYS_MSG_H)
// OSF1 has problems with sys/msg.h and C++...
#  if defined (ACE_HAS_BROKEN_MSG_H)
#    define _KERNEL
#  endif /* ACE_HAS_BROKEN_MSG_H */
#  include /**/ <sys/msg.h>
#  if defined (ACE_HAS_BROKEN_MSG_H)
#    undef _KERNEL
#  endif /* ACE_HAS_BROKEN_MSG_H */
#endif /* !ACE_LACKS_SYS_MSG_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

   // Declare opaque type.  Needed for OS wrappers on systems
   // without SysV IPC.
   struct msqid_ds;

#if defined (ACE_LACKS_SYSV_MSQ_PROTOS)
   int msgget (key_t, int);
   int msgrcv (int, void *, size_t, long, int);
   int msgsnd (int, const void *, size_t, int);
   int msgctl (int, int, struct msqid_ds *);
#endif /* ACE_LACKS_SYSV_MSQ_PROTOS */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_MSG_H */
