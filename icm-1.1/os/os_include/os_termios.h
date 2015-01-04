// -*- C++ -*-

//=============================================================================
/**
 *  @file    os_termios.h
 *
 *  define values for termios
 *
 *  $Id: os_termios.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Don Hinton <dhinton@dresystems.com>
 *  @author This code was originally in various places including ace/OS.h.
 */
//=============================================================================

#ifndef OS_INCLUDE_OS_TERMIOS_H
#define OS_INCLUDE_OS_TERMIOS_H



#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if !defined (ACE_LACKS_TERMIOS_H)
#  include /**/ <termios.h>
#endif /* !ACE_LACKS_TERMIOS_H */

#if defined (HPUX)
#  include /**/ <sys/modem.h>
#endif /* HPUX */

// Place all additions (especially function declarations) within extern "C" {}
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* OS_INCLUDE_OS_TERMIOS_H */
