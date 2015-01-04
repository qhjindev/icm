// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_sysctl.h
 *
 *  declarations for sysctl
 *
 *  $Id: os_sysctl.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Johnny Willemsen  <jwillemsen@remedy.nl>
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_SYSCTL_H
#define OS_INCLUDE_SYS_OS_SYSCTL_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (ACE_LACKS_SYS_SYSCTL_H)
#  include /**/ <sys/sysctl.h>
#endif /* !ACE_LACKS_SYS_SYSCTL_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_SYSCTL_H */
