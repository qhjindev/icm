// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_utsname.h
 *
 *  $Id: OS_NS_sys_utsname.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_SYS_UTSNAME_H
# define OS_NS_SYS_UTSNAME_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export

#if defined (ACE_LACKS_UTSNAME_T)
#   if !defined (SYS_NMLN)
#     define SYS_NMLN 257
#   endif /* SYS_NMLN */
#   if !defined (_SYS_NMLN)
#     define _SYS_NMLN SYS_NMLN
#   endif /* _SYS_NMLN */

struct ACE_utsname
{
  char sysname[_SYS_NMLN];
  char nodename[_SYS_NMLN];
  char release[_SYS_NMLN];
  char version[_SYS_NMLN];
  char machine[_SYS_NMLN];
};

# else
#   include "os_include/sys/os_utsname.h"

typedef struct utsname ACE_utsname;

# endif /* ACE_LACKS_UTSNAME_T */



namespace OS {

  extern ACE_Export
  int uname (ACE_utsname *name);

} /* namespace OS */




#endif /* OS_NS_SYS_UTSNAME_H */
