// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_time.h
 *
 *  $Id: OS_NS_sys_time.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_SYS_TIME_H
# define OS_NS_SYS_TIME_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "icc/TimeValue.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS
{
#if defined (ACE_WIN32) && defined (_WIN32_WCE)
// Something is a bit brain-damaged here and I'm not sure what... this code
// compiled before the OS reorg for ACE 5.4. Since then it hasn't - eVC
// complains that the operators that return TimeValue are C-linkage
// functions that can't return a C++ class. The only way I've found to
// defeat this is to wrap the whole class in extern "C++".
//    - Steve Huston, 23-Aug-2004
extern "C++" {
#endif

  ACE_NAMESPACE_INLINE_FUNCTION
  TimeValue gettimeofday (void);

  int gettimeofday(struct timeval* tp, void* tzp);

#if defined (ACE_WIN32) && defined (_WIN32_WCE)
}
#endif
} /* namespace OS */




#endif /* OS_NS_SYS_TIME_H */
