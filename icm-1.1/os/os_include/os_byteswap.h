// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_byteswap.h
 *
 *  Byteswap methods
 *
 *  $Id: os_byteswap.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Johnny Willemsen  <jwillemsen@remedy.nl>
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_BYTESWAP_H
#define OS_INCLUDE_OS_BYTESWAP_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined (ACE_HAS_BYTESWAP_H)
# include /**/ <byteswap.h>
#endif /* !ACE_HAS_INTRIN_H */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_BYTESWAP_H */
