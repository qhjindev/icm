// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_errno.h
 *
 *  $Id: OS_NS_errno.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_ERRNO_H
# define OS_NS_ERRNO_H



# include "os/config-lite.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_errno.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS {

  ACE_NAMESPACE_INLINE_FUNCTION
  int last_error (void);

  ACE_NAMESPACE_INLINE_FUNCTION
  void last_error (int);

  ACE_NAMESPACE_INLINE_FUNCTION
  int map_errno (int error);

  ACE_NAMESPACE_INLINE_FUNCTION
  int set_errno_to_last_error (void);

  ACE_NAMESPACE_INLINE_FUNCTION
  int set_errno_to_wsa_last_error (void);

} /* namespace OS */

#if defined (ACE_HAS_WINCE_BROKEN_ERRNO)
/**
 * @class ACE_CE_Errno
 *
 * Some versions of CE don't support @c errno and some versions'
 * implementations are busted.  So we implement our own.
 * Our implementation takes up one Tls key, however, it does not
 * allocate memory fromt the heap so there's no problem with cleanin
 * up the errno when a thread exit.
 */
class ACE_Export ACE_CE_Errno
{
public:
  ACE_CE_Errno () {}
  static void init ();
  static void fini ();
  static ACE_CE_Errno *instance ();

  operator int (void) const;
  int operator= (int);

private:
  static ACE_CE_Errno *instance_;
  static DWORD errno_key_;
};

# define errno (* (ACE_CE_Errno::instance ()))
#endif /* ACE_HAS_WINCE_BROKEN_ERRNO */

#if defined (ACE_HAS_WINCE_BROKEN_ERRNO)
#  define ACE_ERRNO_TYPE ACE_CE_Errno
#else
#  define ACE_ERRNO_TYPE int
#endif /* ACE_HAS_WINCE_BROKEN_ERRNO */



#endif /* OS_NS_ERRNO_H */
