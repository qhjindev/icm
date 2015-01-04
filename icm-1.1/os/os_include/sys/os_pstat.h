// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_pstat.h
 *
 *  pstat functions
 *
 *  $Id: os_pstat.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Johnny Willemsen  <jwillemsen@remedy.nl>
 */
//=============================================================================

#ifndef OS_INCLUDE_SYS_OS_PSTAT_H
#define OS_INCLUDE_SYS_OS_PSTAT_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_SYS_PSTAT_H)
#  include /**/ <sys/param.h>
#  include /**/ <sys/pstat.h>
#endif /* ACE_HAS_SYS_PSTAT_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_SYS_OS_PSTAT_H */
