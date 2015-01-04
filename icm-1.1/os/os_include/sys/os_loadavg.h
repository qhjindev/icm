// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_loadavg.h
 *
 *  loadavg functions
 *
 *  $Id: os_loadavg.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Johnny Willemsen  <jwillemsen@remedy.nl>
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_LOADAVG_H
#define OS_INCLUDE_SYS_OS_LOADAVG_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_SYS_LOADAVG_H)
# include /**/ <sys/loadavg.h>
#endif /* ACE_HAS_SYS_LOADAVG_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_LOADAVG_H */
