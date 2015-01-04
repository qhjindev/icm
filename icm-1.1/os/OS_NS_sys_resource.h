// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_resource.h
 *
 *  $Id: OS_NS_sys_resource.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_SYS_RESOURCE_H
# define OS_NS_SYS_RESOURCE_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/sys/os_resource.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS
{

  ACE_NAMESPACE_INLINE_FUNCTION
  int getrlimit (int resource,
                 struct rlimit *rl);

  ACE_NAMESPACE_INLINE_FUNCTION
  int getrusage (int who,
                 struct rusage *rusage);


  ACE_NAMESPACE_INLINE_FUNCTION
  int setrlimit (int resource,
                 const struct rlimit *rl);

} /* namespace OS */




#endif /* OS_NS_SYS_RESOURCE_H */
