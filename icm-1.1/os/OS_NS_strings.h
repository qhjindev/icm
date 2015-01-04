// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_strings.h
 *
 *  $Id: OS_NS_strings.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_STRINGS_H
#define OS_NS_STRINGS_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_strings.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



class TimeValue;

namespace OS
{

  /// Compares two strings (case insensitive const char version).
  ACE_NAMESPACE_INLINE_FUNCTION
  int strcasecmp (const char *s, const char *t);

#if defined (ACE_HAS_WCHAR)
  /// Compares two strings (case insensitive const wchar_t version).
  ACE_NAMESPACE_INLINE_FUNCTION
  int strcasecmp (const wchar_t *s, const wchar_t *t);
#endif /* ACE_HAS_WCHAR */

  /// Compares two arrays (case insensitive const char version).
  ACE_NAMESPACE_INLINE_FUNCTION
  int strncasecmp (const char *s, const char *t, size_t len);

#if defined (ACE_HAS_WCHAR)
  /// Compares two arrays (case insensitive const wchar_t version).
  ACE_NAMESPACE_INLINE_FUNCTION
  int strncasecmp (const wchar_t *s, const wchar_t *t, size_t len);
#endif /* ACE_HAS_WCHAR */

#if defined (ACE_LACKS_STRCASECMP)
  /// Emulated strcasecmp - Performs a case insensitive comparison of strings.
  extern ACE_Export
  int strcasecmp_emulation (const char *s, const char *t);

  /// Emulated strncasecmp - Performs a case insensitvie comparison of arrays.
  extern ACE_Export
  int strncasecmp_emulation (const char *s, const char *t, size_t len);
#endif /* ACE_LACKS_STRCASECMP */

} /* namespace OS */



#endif /* OS_NS_STRINGS_H */
