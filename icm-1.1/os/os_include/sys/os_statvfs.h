// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_statvfs.h
 *
 *  VFS File System information structure
 *
 *  $Id: os_statvfs.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_STATVFS_H
#define OS_INCLUDE_SYS_OS_STATVFS_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_types.h"

#if !defined (ACE_LACKS_SYS_STATVFS_H)
#  include /**/ <sys/statvfs.h>
#endif /* !ACE_LACKS_SYS_STATVFS_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_STATVFS_H */
