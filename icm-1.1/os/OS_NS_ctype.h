// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_ctype.h
 *
 *  $Id: OS_NS_ctype.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_CTYPE_H
# define OS_NS_CTYPE_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS {

  // these are non-standard names...

  /** @name Functions from <cctype>
   *
   *  Included are the functions defined in <cctype> and their <cwctype>
   *  equivalents.
   *
   *  Since they are often implemented as macros, we don't use the same name
   *  here.  Instead, we change by prepending "ace_".
   */
  //@{

  /// Returns true if the character is an alphanumeric character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isalnum (ACE_TCHAR c);

  /// Returns true if the character is an alphabetic character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isalpha (ACE_TCHAR c);

  /// Returns true if the character is a control character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_iscntrl (ACE_TCHAR c);

  /// Returns true if the character is a decimal-digit character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isdigit (ACE_TCHAR c);

  /// Returns true if the character is a printable character other than a space.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isgraph (ACE_TCHAR c);

  /// Returns true if the character is a lowercase character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_islower (ACE_TCHAR c);

  /// Returns true if the character is a printable character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isprint (ACE_TCHAR c);

  /// Returns true if the character is a punctuation character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_ispunct (ACE_TCHAR c);

  /// Returns true if the character is a space character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isspace (ACE_TCHAR c);

  /// Returns true if the character is an uppercase character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isupper (ACE_TCHAR c);

  /// Returns true if the character is a hexadecimal-digit character.
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_isxdigit (ACE_TCHAR c);

  /// Converts a character to lower case (char version).
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_tolower (int c);

#if defined (ACE_HAS_WCHAR) && !defined (ACE_LACKS_TOWLOWER)
  /// Converts a character to lower case (wchar_t version).
  ACE_NAMESPACE_INLINE_FUNCTION
  wint_t ace_towlower (wint_t c);
#endif /* ACE_HAS_WCHAR && !ACE_LACKS_TOWLOWER */

  /// Converts a character to upper case (char version).
  ACE_NAMESPACE_INLINE_FUNCTION
  int ace_toupper (int c);

#if defined (ACE_HAS_WCHAR) && !defined (ACE_LACKS_TOWUPPER)
  /// Converts a character to upper case (wchar_t version).
  ACE_NAMESPACE_INLINE_FUNCTION
  wint_t ace_towupper (wint_t c);
#endif /* ACE_HAS_WCHAR && !ACE_LACKS_TOWUPPER */

  //@}

} /* namespace OS */



# if defined (ACE_HAS_INLINED_OSCALLS)
#   if defined (ACE_INLINE)
#     undef ACE_INLINE
#   endif /* ACE_INLINE */
#   define ACE_INLINE inline
#   include "OS_NS_ctype.inl"
# endif /* ACE_HAS_INLINED_OSCALLS */


#endif /* OS_NS_CTYPE_H */
