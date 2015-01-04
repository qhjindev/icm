// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_un.h
 *
 *  definitions for UNIX domain sockets
 *
 *  $Id: os_un.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_UN_H
#define OS_INCLUDE_SYS_OS_UN_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_socket.h"

#if !defined (ACE_LACKS_SYS_UN_H)
#  include /**/ <sys/un.h>
#endif /* !ACE_LACKS_SYS_UN_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined (ACE_VXWORKS) && (ACE_VXWORKS <= 0x600)
struct sockaddr_un {
  short sun_family;    // AF_UNIX.
  char  sun_path[108]; // path name.
};
#endif /* ACE_VXWORKS */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_UN_H */
