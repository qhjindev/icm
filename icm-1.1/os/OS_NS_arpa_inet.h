// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_arpa_inet.h
 *
 *  $Id: OS_NS_arpa_inet.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_ARPA_INET_H
# define OS_NS_ARPA_INET_H

# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/arpa/os_inet.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS {

  ACE_NAMESPACE_INLINE_FUNCTION
  unsigned long inet_addr (const char *name);

  extern ACE_Export
  int inet_aton (const char *strptr,
                 struct in_addr *addr);

  ACE_NAMESPACE_INLINE_FUNCTION
  char *inet_ntoa (const struct in_addr addr);

  ACE_NAMESPACE_INLINE_FUNCTION
  const char *inet_ntop (int family,
                         const void *addrptr,
                         char *strptr,
                         size_t len);

  ACE_NAMESPACE_INLINE_FUNCTION
  int inet_pton (int family,
                 const char *strptr,
                 void *addrptr);

} /* namespace OS */


#endif /* OS_NS_ARPA_INET_H */
