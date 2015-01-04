// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_sched.h
 *
 *  execution scheduling (REALTIME)
 *
 *  $Id: os_sched.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_SCHED_H
#define OS_INCLUDE_OS_SCHED_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_time.h"

#if !defined (ACE_LACKS_SCHED_H)
# include /**/ <sched.h>
#endif /* !ACE_LACKS_SCHED_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if !defined (__cpu_set_t_defined) || !defined (ACE_HAS_CPU_SET_T)
#  define ACE_CPU_SETSIZE 1024
   typedef struct
   {
     ACE_UINT32 bit_array_[ACE_CPU_SETSIZE / (8 * sizeof (ACE_UINT32))];
   } cpu_set_t;
#endif /* !ACE_HAS_CPU_SET_T || !__cpu_set_t_defined */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_SCHED_H */
