// -*- C++ -*-

//==========================================================================
/**
 *  @file   config-all.h
 *
 *  $Id: config-all.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author (Originally in OS.h)Doug Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 */
//==========================================================================

#ifndef ACE_CONFIG_ALL_H
#define ACE_CONFIG_ALL_H



#include "os/config-lite.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// This is used to indicate that a platform doesn't support a
// particular feature.
#if defined ACE_HAS_VERBOSE_NOTSUP
  // Print a console message with the file and line number of the
  // unsupported function.
# include "os/OS_NS_stdio.h"
# define ACE_NOTSUP_RETURN(FAILVALUE) do { errno = ENOTSUP; OS::fprintf (stderr, ACE_TEXT ("ACE_NOTSUP: %s, line %d\n"), __FILE__, __LINE__); return FAILVALUE; } while (0)
# define ACE_NOTSUP do { errno = ENOTSUP; OS::fprintf (stderr, ACE_TEXT ("ACE_NOTSUP: %s, line %d\n"), __FILE__, __LINE__); return; } while (0)
#else /* ! ACE_HAS_VERBOSE_NOTSUP */
# define ACE_NOTSUP_RETURN(FAILVALUE) do { errno = ENOTSUP ; return FAILVALUE; } while (0)
# define ACE_NOTSUP do { errno = ENOTSUP; return; } while (0)
#endif /* ! ACE_HAS_VERBOSE_NOTSUP */

// ----------------------------------------------------------------

# define ACE_TRACE_IMPL(X) ACE_Trace ____ (ACE_TEXT (X), __LINE__, ACE_TEXT (__FILE__))

// By default tracing is turned off.
#if !defined (ACE_NTRACE)
#  define ACE_NTRACE 1
#endif /* ACE_NTRACE */

#if (ACE_NTRACE == 1)
#  define ACE_TRACE(X)
#else
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define ACE_TRACE(X) ACE_TRACE_IMPL(X)
#  include "Trace.h"
#endif /* ACE_NTRACE */

// By default we perform no tracing on the OS layer, otherwise the
// coupling between the OS layer and Log_Msg is too tight.  But the
// application can override the default if they wish to.
#if !defined (OS_NTRACE)
#  define OS_NTRACE 1
#endif /* OS_NTRACE */

#if (OS_NTRACE == 1)
#  define OS_TRACE(X)
#else
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define OS_TRACE(X) ACE_TRACE_IMPL(X)
#  include "Trace.h"
#endif /* OS_NTRACE */

// These includes are here to avoid circular dependencies.
// Keep this at the bottom of the file.  It contains the main macros.
//#include "os/OS_main.h"



#endif /* ACE_CONFIG_ALL_H */
