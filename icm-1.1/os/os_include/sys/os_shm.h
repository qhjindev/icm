// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_shm.h
 *
 *  XSI shared memory facility
 *
 *  $Id: os_shm.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_SHM_H
#define OS_INCLUDE_SYS_OS_SHM_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_ipc.h"

#if !defined (ACE_LACKS_SYS_SHM_H)
#  include /**/ <sys/shm.h>
#endif /* !ACE_LACKS_SYS_SHM_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

  // Declare opaque type.  Needed for OS wrappers on systems
  // without SysV IPC.
  struct shmid_ds;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_SHM_H */
