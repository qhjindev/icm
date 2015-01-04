// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_tcp.h
 *
 *  definitions for the Internet Transmission Control Protocol (TCP)
 *
 *  $Id: os_tcp.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_NETINET_OS_TCP_H
#define OS_INCLUDE_NETINET_OS_TCP_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (ACE_LACKS_NETINET_TCP_H)
# include /**/ <netinet/tcp.h>
#endif /* !ACE_LACKS_NETIINET_TCP_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

# if !defined (TCP_NODELAY)
#   define TCP_NODELAY 0x01
# endif /* TCP_NODELAY */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_NETINET_OS_TCP_H */
