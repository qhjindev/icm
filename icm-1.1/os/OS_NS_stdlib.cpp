// $Id: OS_NS_stdlib.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_stdlib.h"
#include "os/Default_Constants.h"

#include "os/OS_Memory.h"

#include "os/OS_NS_unistd.h"

#if defined (ACE_LACKS_MKTEMP) \
    || defined (ACE_LACKS_MKSTEMP) \
    || defined (ACE_LACKS_REALPATH)
#  include "os/OS_NS_stdio.h"
#  include "os/OS_NS_sys_stat.h"
#endif /* ACE_LACKS_MKTEMP || ACE_LACKS_MKSTEMP || ACE_LACKS_REALPATH */

#if defined (ACE_LACKS_MKSTEMP)
# include "os/OS_NS_fcntl.h"
# include "os/OS_NS_ctype.h"
# include "os/OS_NS_sys_time.h"
# include "os/OS_NS_Thread.h"
//# include "Numeric_Limits.h"
#include <limits>

#undef min
#undef max

#endif  /* ACE_LACKS_MKSTEMP */

ACE_EXIT_HOOK OS::exit_hook_ = 0;

// -*- C++ -*-
//
// $Id: OS_NS_stdlib.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/config-all.h"           /* Need ACE_TRACE */
//#include "Object_Manager_Base.h"
#include "os/OS_NS_string.h"
#include "os/Global_Macros.h"
#include "os/Basic_Types.h"  /* intptr_t */
#include "os/os_include/os_errno.h"
#include "os/os_include/os_search.h"

#if defined (ACE_WCHAR_IN_STD_NAMESPACE)
# define ACE_WCHAR_STD_NAMESPACE std
#else
# define ACE_WCHAR_STD_NAMESPACE ACE_STD_NAMESPACE
#endif /* ACE_WCHAR_IN_STD_NAMESPACE */



// Doesn't need a macro since it *never* returns!

ACE_INLINE void
OS::_exit (int status)
{
  OS_TRACE ("OS::_exit");
#if defined (ACE_VXWORKS)
  ::exit (status);
#elif !defined (ACE_HAS_WINCE)
  ::_exit (status);
#else
  ::TerminateProcess (::GetCurrentProcess (),
                      status);
#endif /* ACE_VXWORKS */
}

ACE_INLINE void
OS::abort (void)
{
#if !defined (ACE_HAS_WINCE)
  ::abort ();
#else
  // @@ CE doesn't support abort?
  exit (1);
#endif /* !ACE_HAS_WINCE */
}

ACE_INLINE int
OS::atexit (ACE_EXIT_HOOK func)
{
  return 0;
}

ACE_INLINE int
OS::atoi (const char *s)
{
  OSCALL_RETURN (::atoi (s), int, -1);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::atoi (const wchar_t *s)
{
#if defined (ACE_WIN32)
  OSCALL_RETURN (::_wtoi (s), int, -1);
#else /* ACE_WIN32 */
  return OS::atoi (ACE_Wide_To_Ascii (s).char_rep ());
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

#if defined (atop)
#  undef atop
#endif /* atop */

ACE_INLINE void *
OS::atop (const char *s)
{
  ACE_TRACE ("OS::atop");
#if defined (ACE_WIN64)
  intptr_t ip = ::_atoi64 (s);
#elif defined (ACE_OPENVMS)
#  if !defined (__INITIAL_POINTER_SIZE) || (__INITIAL_POINTER_SIZE < 64)
  int ip = ::atoi (s);
#  else
  intptr_t ip = ::atoi (s);
#  endif
#else
  intptr_t ip = ::atoi (s);
#endif /* ACE_WIN64 */
  void * p = reinterpret_cast<void *> (ip);
  return p;
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE void *
OS::atop (const wchar_t *s)
{
#  if defined (ACE_WIN64)
  intptr_t ip = ::_wtoi64 (s);
#  elif defined (ACE_OPENVMS)
#    if !defined (__INITIAL_POINTER_SIZE) || (__INITIAL_POINTER_SIZE < 64)
  int ip = OS::atoi (s);
#    else
  intptr_t ip = OS::atoi (s);
#    endif
#  else
  intptr_t ip = OS::atoi (s);
#  endif /* ACE_WIN64 */
  void * p = reinterpret_cast<void *> (ip);
  return p;
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE void *
OS::bsearch (const void *key,
                 const void *base,
                 size_t nel,
                 size_t size,
                 ACE_COMPARE_FUNC compar)
{
#if !defined (ACE_LACKS_BSEARCH)
  return ::bsearch (key, base, nel, size, compar);
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (base);
  ACE_UNUSED_ARG (nel);
  ACE_UNUSED_ARG (size);
  ACE_UNUSED_ARG (compar);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_LACKS_BSEARCH */
}

ACE_INLINE char *
OS::getenv (const char *symbol)
{
  OS_TRACE ("OS::getenv");
#if defined (ACE_LACKS_ENV)
  ACE_UNUSED_ARG (symbol);
  ACE_NOTSUP_RETURN (0);
#else /* ACE_LACKS_ENV */
  OSCALL_RETURN (::getenv (symbol), char *, 0);
#endif /* ACE_LACKS_ENV */
}

#if defined (ACE_HAS_WCHAR) && defined (ACE_WIN32)
ACE_INLINE wchar_t *
OS::getenv (const wchar_t *symbol)
{
#if defined (ACE_LACKS_ENV)
  ACE_UNUSED_ARG (symbol);
  ACE_NOTSUP_RETURN (0);
#else
  OSCALL_RETURN (::_wgetenv (symbol), wchar_t *, 0);
#endif /* ACE_LACKS_ENV */
}
#endif /* ACE_HAS_WCHAR && ACE_WIN32 */

ACE_INLINE char *
OS::itoa (int value, char *string, int radix)
{
#if !defined (ACE_HAS_ITOA)
  return OS::itoa_emulation (value, string, radix);
#elif defined (ACE_ITOA_EQUIVALENT)
  return ACE_ITOA_EQUIVALENT (value, string, radix);
#else /* !ACE_HAS_ITOA */
  return ::itoa (value, string, radix);
#endif /* !ACE_HAS_ITOA */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::itoa (int value, wchar_t *string, int radix)
{
#if defined (ACE_LACKS_ITOW)
  return OS::itow_emulation (value, string, radix);
#else /* ACE_LACKS_ITOW */
  return ::_itow (value, string, radix);
#endif /* ACE_LACKS_ITOW */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE ACE_HANDLE
OS::mkstemp (char *s)
{
#if !defined (ACE_LACKS_MKSTEMP)
  return ::mkstemp (s);
#else
  return OS::mkstemp_emulation (ACE_TEXT_CHAR_TO_TCHAR (s));
#endif  /* !ACE_LACKS_MKSTEMP */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE ACE_HANDLE
OS::mkstemp (wchar_t *s)
{
#  if !defined (ACE_LACKS_MKSTEMP)
  return ::mkstemp (ACE_TEXT_WCHAR_TO_TCHAR (ACE_TEXT_ALWAYS_CHAR (s)));
#  else
  return OS::mkstemp_emulation (ACE_TEXT_WCHAR_TO_TCHAR (s));
#  endif  /* !ACE_LACKS_MKSTEMP */
}
#endif /* ACE_HAS_WCHAR */

#if !defined (ACE_LACKS_MKTEMP)
ACE_INLINE char *
OS::mktemp (char *s)
{
# if defined (ACE_WIN32)
  return ::_mktemp (s);
# else /* ACE_WIN32 */
  //return ::mktemp (s); by qinghua
  return 0;
# endif /* ACE_WIN32 */
}

#  if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::mktemp (wchar_t *s)
{
#    if defined (ACE_WIN32)
  return ::_wmktemp (s);
#    else
  // For narrow-char filesystems, we must convert the wide-char input to
  // a narrow-char string for mktemp (), then convert the name back to
  // wide-char for the caller.
  ACE_Wide_To_Ascii narrow_s (s);
  //if (::mktemp (narrow_s.char_rep ()) == 0) by qinghua
  if (::mkstemp(narrow_s.char_rep ()) == 0)
    return 0;
  ACE_Ascii_To_Wide wide_s (narrow_s.char_rep ());
  OS::strcpy (s, wide_s.wchar_rep ());
  return s;
#    endif
}
#  endif /* ACE_HAS_WCHAR */

#endif /* !ACE_LACKS_MKTEMP */

#if defined (INTEGRITY)
extern "C" {
  int putenv (char *string);
}
#endif

ACE_INLINE int
OS::putenv (const char *string)
{
  OS_TRACE ("OS::putenv");
#if defined (ACE_HAS_WINCE)
  // WinCE don't have the concept of environment variables.
  ACE_UNUSED_ARG (string);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_LACKS_PUTENV) && defined (ACE_HAS_SETENV)
  int result = 0;
  char *sp = OS::strchr (const_cast <char *> (string), '=');
  if (sp)
    {
      char *stmp = OS::strdup (string);
      if (stmp)
        {
          stmp[sp - string] = '\0';
          OSCALL (::setenv (stmp, sp+sizeof (char), 1), int, -1, result);
          OS::free (stmp);
        }
      else
        {
          errno = ENOMEM;
          result = -1;
        }
    }
  else
    OSCALL (::setenv (string, "", 1), int, -1, result);

  return result;
#elif defined (ACE_LACKS_ENV) || defined (ACE_LACKS_PUTENV)
  ACE_UNUSED_ARG (string);
  ACE_NOTSUP_RETURN (0);
#else /* ! ACE_HAS_WINCE */
  OSCALL_RETURN (ACE_STD_NAMESPACE::putenv (const_cast <char *> (string)), int, -1);
#endif /* ACE_HAS_WINCE */
}

#if defined (ACE_HAS_WCHAR) && defined (ACE_WIN32)
ACE_INLINE int
OS::putenv (const wchar_t *string)
{
  OS_TRACE ("OS::putenv");
#if defined (ACE_HAS_WINCE)
  // WinCE doesn't have the concept of environment variables.
  ACE_UNUSED_ARG (string);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::_wputenv (string), int, -1);
#endif /* ACE_HAS_WINCE */
}
#endif /* ACE_HAS_WCHAR && ACE_WIN32 */

ACE_INLINE void
OS::qsort (void *base,
               size_t nel,
               size_t width,
               ACE_COMPARE_FUNC compar)
{
#if !defined (ACE_LACKS_QSORT)
  ::qsort (base, nel, width, compar);
#else
  ACE_UNUSED_ARG (base);
  ACE_UNUSED_ARG (nel);
  ACE_UNUSED_ARG (width);
  ACE_UNUSED_ARG (compar);
#endif /* !ACE_LACKS_QSORT */
}

ACE_INLINE int
OS::rand (void)
{
  OS_TRACE ("OS::rand");
  OSCALL_RETURN (::rand (), int, -1);
}

#if !defined (ACE_WIN32)

ACE_INLINE int
OS::rand_r (ACE_RANDR_TYPE &seed)
{
  OS_TRACE ("OS::rand_r");
# if defined (ACE_HAS_REENTRANT_FUNCTIONS) && \
    !defined (ACE_LACKS_RAND_REENTRANT_FUNCTIONS)
#   if defined (DIGITAL_UNIX)
  OSCALL_RETURN (::_Prand_r (&seed), int, -1);
#   elif defined (ACE_HAS_BROKEN_RANDR)
  OSCALL_RETURN (::rand_r (seed), int, -1);
#   else
  OSCALL_RETURN (::rand_r (&seed), int, -1);
#   endif /* DIGITAL_UNIX */
# else
  ACE_UNUSED_ARG (seed);
  OSCALL_RETURN (::rand (), int, -1);
# endif /* ACE_HAS_REENTRANT_FUNCTIONS */
}

#else /* ACE_WIN32 */

ACE_INLINE int
OS::rand_r (ACE_RANDR_TYPE& seed)
{
  OS_TRACE ("OS::rand_r");

  long new_seed = (long) (seed);
  if (new_seed == 0)
    new_seed = 0x12345987;
  long temp = new_seed / 127773;
  new_seed = 16807 * (new_seed - temp * 127773) - 2836 * temp;
  if (new_seed < 0)
    new_seed += 2147483647;
 (seed) = (unsigned int)new_seed;
  return (int) (new_seed & RAND_MAX);
}

#endif /* !ACE_WIN32 */

#if !defined (ACE_HAS_WINCE)
#  if !defined (ACE_LACKS_REALPATH)
ACE_INLINE char *
OS::realpath (const char *file_name,
                  char *resolved_name)
{
#    if defined (ACE_WIN32)
  return ::_fullpath (resolved_name, file_name, PATH_MAX);
#    else /* ACE_WIN32 */
  return ::realpath (file_name, resolved_name);
#    endif /* ! ACE_WIN32 */
}
#  endif /* !ACE_LACKS_REALPATH */

#  if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::realpath (const wchar_t *file_name,
                  wchar_t *resolved_name)
{
#    if defined (ACE_WIN32)
  return ::_wfullpath (resolved_name, file_name, PATH_MAX);
#    else /* ACE_WIN32 */
  ACE_Wide_To_Ascii n_file_name (file_name);
  char n_resolved[PATH_MAX];
  if (0 != OS::realpath (n_file_name.char_rep (), n_resolved))
    {
      ACE_Ascii_To_Wide w_resolved (n_resolved);
      OS::strcpy (resolved_name, w_resolved.wchar_rep ());
      return resolved_name;
    }
  return 0;
#    endif /* ! ACE_WIN32 */
}
#  endif /* ACE_HAS_WCHAR */
#endif /* ACE_HAS_WINCE */

ACE_INLINE ACE_EXIT_HOOK
OS::set_exit_hook (ACE_EXIT_HOOK exit_hook)
{
  ACE_EXIT_HOOK old_hook = exit_hook_;
  exit_hook_ = exit_hook;
  return old_hook;
}

ACE_INLINE void
OS::srand (u_int seed)
{
  OS_TRACE ("OS::srand");
  ::srand (seed);
}

#if !defined (ACE_LACKS_STRTOD)
ACE_INLINE double
OS::strtod (const char *s, char **endptr)
{
  return ::strtod (s, endptr);
}
#endif /* !ACE_LACKS_STRTOD */

#if defined (ACE_HAS_WCHAR) && !defined (ACE_LACKS_WCSTOD)
ACE_INLINE double
OS::strtod (const wchar_t *s, wchar_t **endptr)
{
  return ACE_WCHAR_STD_NAMESPACE::wcstod (s, endptr);
}
#endif /* ACE_HAS_WCHAR && !ACE_LACKS_WCSTOD */

ACE_INLINE long
OS::strtol (const char *s, char **ptr, int base)
{
#if defined (ACE_LACKS_STRTOL)
  return OS::strtol_emulation (s, ptr, base);
#else  /* ACE_LACKS_STRTOL */
  return ::strtol (s, ptr, base);
#endif /* ACE_LACKS_STRTOL */
}

#if defined (ACE_HAS_WCHAR) && !defined (ACE_LACKS_WCSTOL)
ACE_INLINE long
OS::strtol (const wchar_t *s, wchar_t **ptr, int base)
{
  return ACE_WCHAR_STD_NAMESPACE::wcstol (s, ptr, base);
}
#endif /* ACE_HAS_WCHAR && !ACE_LACKS_WCSTOL */

ACE_INLINE unsigned long
OS::strtoul (const char *s, char **ptr, int base)
{
#if defined (ACE_LACKS_STRTOUL)
  return OS::strtoul_emulation (s, ptr, base);
#else /* ACE_LACKS_STRTOUL */
  return ::strtoul (s, ptr, base);
#endif /* ACE_LACKS_STRTOUL */
}

#if defined (ACE_HAS_WCHAR) && !defined (ACE_LACKS_WCSTOUL)
ACE_INLINE unsigned long
OS::strtoul (const wchar_t *s, wchar_t **ptr, int base)
{
  return ACE_WCHAR_STD_NAMESPACE::wcstoul (s, ptr, base);
}
#endif /* ACE_HAS_WCHAR && !ACE_LACKS_WCSTOUL */

ACE_INLINE int
OS::system (const ACE_TCHAR *s)
{
  // OS_TRACE ("OS::system");
#if defined (ACE_LACKS_SYSTEM)
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32) && defined (ACE_USES_WCHAR)
  OSCALL_RETURN (::_wsystem (s), int, -1);
#elif defined (ACE_TANDEM_T1248_PTHREADS)
  OSCALL_RETURN (::spt_system (s), int, -1);
#else
  OSCALL_RETURN (::system (ACE_TEXT_ALWAYS_CHAR (s)), int, -1);
#endif /* ACE_LACKS_SYSTEM */
}

void *
OS::calloc (size_t elements, size_t sizeof_elements)
{
#if !defined (ACE_HAS_WINCE)
  return ACE_CALLOC_FUNC (elements, sizeof_elements);
#else
  // @@ This will probably not work since it doesn't consider
  // alignment properly.
  return ACE_MALLOC_FUNC (elements * sizeof_elements);
#endif /* ACE_HAS_WINCE */
}

void
OS::exit (int status)
{
  OS_TRACE ("OS::exit");

#if defined (ACE_HAS_NONSTATIC_OBJECT_MANAGER) && !defined (ACE_HAS_WINCE) && !defined (ACE_DOESNT_INSTANTIATE_NONSTATIC_OBJECT_MANAGER)
  // Shut down the ACE_Object_Manager, if it had registered its exit_hook.
  // With ACE_HAS_NONSTATIC_OBJECT_MANAGER, the ACE_Object_Manager is
  // instantiated on the main's stack.  ::exit () doesn't destroy it.
  if (exit_hook_)
    (*exit_hook_) ();
#endif /* ACE_HAS_NONSTATIC_OBJECT_MANAGER && !ACE_HAS_WINCE && !ACE_DOESNT_INSTANTIATE_NONSTATIC_OBJECT_MANAGER */

#if !defined (ACE_HAS_WINCE)
# if defined (ACE_WIN32)
  ::ExitProcess ((UINT) status);
# else
  ::exit (status);
# endif /* ACE_WIN32 */
#else
  // @@ This is not exactly the same as ExitProcess.  But this is the
  // closest one I can get.
  ::TerminateProcess (::GetCurrentProcess (), status);
#endif /* ACE_HAS_WINCE */
}

void
OS::free (void *ptr)
{
  ACE_FREE_FUNC (ACE_MALLOC_T (ptr));
}

// You may be asking yourself, why are we doing this?  Well, in winbase.h,
// MS didn't follow their normal Api_FunctionA and Api_FunctionW style,
// so we have to #undef their define to get access to the unicode version.
// And because we don't want to #undef this for the users code, we keep
// this method in the .cpp file.
#if defined (ACE_WIN32) && defined (UNICODE) && !defined (ACE_USES_TCHAR)
#undef GetEnvironmentStrings
#endif /* ACE_WIN32 && UNICODE !ACE_USES_TCHAR */

ACE_TCHAR *
OS::getenvstrings (void)
{
#if defined (ACE_LACKS_ENV)
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_WIN32)
# if defined (ACE_USES_WCHAR)
  return ::GetEnvironmentStringsW ();
# else /* ACE_USES_WCHAR */
  return ::GetEnvironmentStrings ();
# endif /* ACE_USES_WCHAR */
#else /* ACE_WIN32 */
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_WIN32 */
}

// Return a dynamically allocated duplicate of <str>, substituting the
// environment variables of form $VAR_NAME.  Note that the pointer is
// allocated with <OS::malloc> and must be freed by
// <OS::free>.

ACE_TCHAR *
OS::strenvdup (const ACE_TCHAR *str)
{
#if defined (ACE_HAS_WINCE)
  // WinCE doesn't have environment variables so we just skip it.
  return OS::strdup (str);
#elif defined (ACE_LACKS_ENV)
  ACE_UNUSED_ARG (str);
  ACE_NOTSUP_RETURN (0);
#else
  const ACE_TCHAR * start = 0;
  if ((start = OS::strchr (str, ACE_TEXT ('$'))) != 0)
    {
      ACE_TCHAR buf[ACE_DEFAULT_ARGV_BUFSIZ];
      size_t var_len = OS::strcspn (&start[1],
        ACE_TEXT ("$~!#%^&*()-+=\\|/?,.;:'\"`[]{} \t\n\r"));
      OS::strncpy (buf, &start[1], var_len);
      buf[var_len++] = ACE_TEXT ('\0');
#  if defined (ACE_WIN32)
      // Always use the ACE_TCHAR for Windows.
      ACE_TCHAR *temp = OS::getenv (buf);
#  else
      // Use char * for environment on non-Windows.
      char *temp = OS::getenv (ACE_TEXT_ALWAYS_CHAR (buf));
#  endif /* ACE_WIN32 */
      size_t buf_len = OS::strlen (str) + 1;
      if (temp != 0)
        buf_len += OS::strlen (temp) - var_len;
      ACE_TCHAR * buf_p = buf;
      if (buf_len > ACE_DEFAULT_ARGV_BUFSIZ)
        {
          buf_p =
            (ACE_TCHAR *) OS::malloc (buf_len * sizeof (ACE_TCHAR));
          if (buf_p == 0)
            {
              errno = ENOMEM;
              return 0;
            }
        }
      ACE_TCHAR * p = buf_p;
      size_t len = start - str;
      OS::strncpy (p, str, len);
      p += len;
      if (temp != 0)
        {
#  if defined (ACE_WIN32)
          p = OS::strecpy (p, temp) - 1;
#  else
          p = OS::strecpy (p, ACE_TEXT_CHAR_TO_TCHAR (temp)) - 1;
#  endif /* ACE_WIN32 */
        }
      else
        {
          OS::strncpy (p, start, var_len);
          p += var_len;
          *p = ACE_TEXT ('\0');
        }
      OS::strcpy (p, &start[var_len]);
      return (buf_p == buf) ? OS::strdup (buf) : buf_p;
    }
  else
    return OS::strdup (str);
#endif /* ACE_HAS_WINCE */
}

#if !defined (ACE_HAS_ITOA)
char *
OS::itoa_emulation (int value, char *string, int radix)
{
  char *e = string;
  char *b = string;

  // Short circuit if 0

  if (value == 0)
    {
      string[0] = '0';
      string[1] = 0;
      return string;
    }

  // If negative and base 10, print a - and then do the
  // number.

  if (value < 0 && radix == 10)
    {
      string[0] = '-';
      ++b;
      ++e; // Don't overwrite the negative sign.
      value = -value; // Drop negative sign so character selection is correct.
    }

  // Convert to base <radix>, but in reverse order

  while (value != 0)
    {
      int mod = value % radix;
      value = value / radix;

      *e++ = (mod < 10) ? '0' + mod : 'a' + mod - 10;
    }

  *e-- = 0;

  // Now reverse the string to get the correct result

  while (e > b)
    {
      char temp = *e;
      *e = *b;
      *b = temp;
      ++b;
      --e;
    }

  return string;
}
#endif /* !ACE_HAS_ITOA */

#if defined (ACE_HAS_WCHAR) && defined (ACE_LACKS_ITOW)
wchar_t *
OS::itow_emulation (int value, wchar_t *string, int radix)
{
  wchar_t *e = string;
  wchar_t *b = string;

  // Short circuit if 0

  if (value == 0)
    {
      string[0] = '0';
      string[1] = 0;
      return string;
    }

  // If negative and base 10, print a - and then do the
  // number.

  if (value < 0 && radix == 10)
    {
      string[0] = '-';
      b++;
    }

  // Convert to base <radix>, but in reverse order

  while (value != 0)
    {
      int mod = value % radix;
      value = value / radix;

      *e++ = (mod < 10) ? '0' + mod : 'a' + mod - 10;
    }

  *e-- = 0;

  // Now reverse the string to get the correct result

  while (e > b)
  {
    wchar_t temp = *e;
    *e = *b;
    *b = temp;
    ++b;
    --e;
  }

  return string;
}
#endif /* ACE_HAS_WCHAR && ACE_LACKS_ITOW */

void *
OS::malloc (size_t nbytes)
{
  return ACE_MALLOC_FUNC (nbytes);
}

#if defined (ACE_LACKS_MKTEMP)
ACE_TCHAR *
OS::mktemp (ACE_TCHAR *s)
{
  OS_TRACE ("OS::mktemp");
  if (s == 0)
    // check for null template string failed!
    return 0;
  else
    {
      ACE_TCHAR *xxxxxx = OS::strstr (s, ACE_TEXT ("XXXXXX"));

      if (xxxxxx == 0)
        // the template string doesn't contain "XXXXXX"!
        return s;
      else
        {
          ACE_TCHAR unique_letter = ACE_TEXT ('a');
          ACE_stat sb;

          // Find an unused filename for this process.  It is assumed
          // that the user will open the file immediately after
          // getting this filename back (so, yes, there is a race
          // condition if multiple threads in a process use the same
          // template).  This appears to match the behavior of the
          // SunOS 5.5 mktemp().
          OS::sprintf (xxxxxx,
                           ACE_TEXT ("%05d%c"),
                           OS::getpid (),
                           unique_letter);
          while (OS::stat (s, &sb) >= 0)
            {
              if (++unique_letter <= ACE_TEXT ('z'))
                OS::sprintf (xxxxxx,
                                 ACE_TEXT ("%05d%c"),
                                 OS::getpid (),
                                 unique_letter);
              else
                {
                  // maximum of 26 unique files per template, per process
                  OS::sprintf (xxxxxx, ACE_TEXT ("%s"), ACE_TEXT (""));
                  return s;
                }
            }
        }
      return s;
    }
}
#endif /* ACE_LACKS_MKTEMP */

void *
OS::realloc (void *ptr, size_t nbytes)
{
  return ACE_REALLOC_FUNC (ACE_MALLOC_T (ptr), nbytes);
}

#if defined (ACE_LACKS_REALPATH) && !defined (ACE_HAS_WINCE)
char *
OS::realpath (const char *file_name,
                  char *resolved_name)
{
  OS_TRACE ("OS::realpath");

  if (file_name == 0)
    {
      // Single Unix Specification V3:
      //   Return an error if parameter is a null pointer.
      errno = EINVAL;
      return 0;
    }

  if (*file_name == '\0')
    {
      // Single Unix Specification V3:
      //   Return an error if the file_name argument points
      //   to an empty string.
      errno = ENOENT;
      return 0;
    }

  char* rpath;

  if (resolved_name == 0)
    {
      // Single Unix Specification V3:
      //   Return an error if parameter is a null pointer.
      //
      // To match glibc realpath() and Win32 _fullpath() behavior,
      // allocate room for the return value if resolved_name is
      // a null pointer.
      rpath = static_cast<char*>(OS::malloc (PATH_MAX));
      if (rpath == 0)
        {
          errno = ENOMEM;
          return 0;
        }
    }
  else
    {
      rpath = resolved_name;
    }

  char* dest;

  if (*file_name != '/')
    {
      // file_name is relative path so CWD needs to be added
      if (OS::getcwd (rpath, PATH_MAX) == 0)
        {
          if (resolved_name == 0)
            OS::free (rpath);
          return 0;
        }
      dest = OS::strchr (rpath, '\0');
    }
  else
    {
      dest = rpath;
    }

#if !defined (ACE_LACKS_SYMLINKS)
  char expand_buf[PATH_MAX]; // Extra buffer needed to expand symbolic links
  int nlinks = 0;
#endif

  while (*file_name)
    {
      *dest++ = '/';

      // Skip multiple separators
      while (*file_name == '/')
        ++file_name;

      char* start = dest;

      // Process one path component
      while (*file_name && *file_name != '/')
        {
          *dest++ = *file_name++;
          if (dest - rpath > PATH_MAX)
            {
              errno = ENAMETOOLONG;
              if (resolved_name == 0)
                OS::free (rpath);
              return 0;
            }
        }

      if (start == dest) // Are we done?
        {
          if (dest - rpath > 1)
            --dest; // Remove trailing separator if not at root
          break;
        }
      else if (dest - start == 1 && *start == '.')
        {
          dest -= 2; // Remove "./"
        }
      else if (dest - start == 2 && *start == '.' && *(start +1) == '.')
        {
          dest -= 3; // Remove "../"
          if (dest > rpath) // Remove the last path component if not at root
            while (*--dest != '/')
              ;
        }
#  if !defined (ACE_LACKS_SYMLINKS)
      else
        {
          ACE_stat st;

          *dest = '\0';
          if (OS::lstat(rpath, &st) < 0)
            {
              if (resolved_name == 0)
              OS::free (rpath);
                return 0;
            }

          // Check if current path is a link
          if (S_ISLNK (st.st_mode))
            {
              if (++nlinks > MAXSYMLINKS)
                {
                  errno = ELOOP;
                  if (resolved_name == 0)
                    OS::free (rpath);
                  return 0;
                }

              char link_buf[PATH_MAX];

              ssize_t link_len = OS::readlink (rpath, link_buf, PATH_MAX);
              int tail_len = OS::strlen (file_name) + 1;

              // Check if there is room to expand link?
              if (link_len + tail_len > PATH_MAX)
                {
                  errno = ENAMETOOLONG;
                  if (resolved_name == 0)
                    OS::free (rpath);
                  return 0;
                }

              // Move tail and prefix it with expanded link
              OS::memmove (expand_buf + link_len, file_name, tail_len);
              OS::memcpy (expand_buf, link_buf, link_len);

              if (*link_buf == '/') // Absolute link?
                {
                  dest = rpath;
                }
              else // Relative link, remove expanded link component
                {
                  --dest;
                  while (*--dest != '/')
                    ;
                }
              file_name = expand_buf; // Source path is now in expand_buf
            }
        }
#  endif /* ACE_LACKS_SYMLINKS */
    }

  *dest = '\0';

  return rpath;
}
#endif /* ACE_LACKS_REALPATH && !ACE_HAS_WINCE */

#if defined (ACE_LACKS_STRTOL)
long
OS::strtol_emulation (const char *nptr, char **endptr, int base)
{
  register const char *s = nptr;
  register unsigned long acc;
  register int c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
    c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0)
    base = c == '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   *
   * Set any if any `digits' consumed; make it negative to indicate
   * overflow.
   */
  cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
  cutlim = cutoff % (unsigned long)base;
  cutoff /= (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base)
      break;
    if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
      any = -1;
    else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
    errno = ERANGE;
  } else if (neg)
    acc = -acc;
  if (endptr != 0)
    *endptr = any ? (char *)s - 1 : (char *)nptr;
  return (acc);
}
#endif /* ACE_LACKS_STRTOL */

#if defined (ACE_LACKS_STRTOUL)
unsigned long
OS::strtoul_emulation (const char *nptr,
                           char **endptr,
                           register int base)
{
  register const char *s = nptr;
  register unsigned long acc;
  register int c;
  register unsigned long cutoff;
  register int neg = 0, any, cutlim;

  /*
   * See strtol for comments as to the logic used.
   */
  do
    c = *s++;
  while (isspace(c));
  if (c == '-')
    {
      neg = 1;
      c = *s++;
    }
  else if (c == '+')
    c = *s++;
  if ((base == 0 || base == 16) &&
      c == '0' && (*s == 'x' || *s == 'X'))
    {
      c = s[1];
      s += 2;
      base = 16;
    }
  if (base == 0)
    base = c == '0' ? 8 : 10;
  cutoff = (unsigned long) ULONG_MAX / (unsigned long) base;
  cutlim = (unsigned long) ULONG_MAX % (unsigned long) base;

  for (acc = 0, any = 0;; c = *s++)
    {
      if (isdigit(c))
        c -= '0';
      else if (isalpha(c))
        c -= isupper(c) ? 'A' - 10 : 'a' - 10;
      else
        break;
      if (c >= base)
        break;
      if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
        any = -1;
      else
        {
          any = 1;
          acc *= base;
          acc += c;
        }
    }
  if (any < 0)
    {
      acc = ULONG_MAX;
      errno = ERANGE;
    } else if (neg)
    acc = -acc;
  if (endptr != 0)
    *endptr = any ? (char *) s - 1 : (char *) nptr;
  return (acc);
}
#endif /* ACE_LACKS_STRTOUL */


#if defined (ACE_LACKS_MKSTEMP)
ACE_HANDLE
OS::mkstemp_emulation (ACE_TCHAR * s)
{
  if (s == 0)
    {
      errno = EINVAL;
      return ACE_INVALID_HANDLE;
    }

  // The "XXXXXX" template to be filled in.
  ACE_TCHAR * const t  = OS::strstr (s, ACE_TEXT ("XXXXXX"));

  if (t == 0)
    {
      errno = EINVAL;
      return ACE_INVALID_HANDLE;
    }

  static unsigned int const NUM_RETRIES = 50;
  static unsigned int const NUM_CHARS   = 6;  // Do not change!

  // Use TimeValue::msec(ACE_UINT64&) as opposed to
  // TimeValue::msec(void) to avoid truncation.
  ACE_UINT64 msec = 0;

  // Use a const TimeValue to resolve ambiguity between
  // TimeValue::msec (long) and TimeValue::msec(ACE_UINT64&) const.
  TimeValue now = OS::gettimeofday();
  //now.msec ((long)msec); //by qinghua

  // Add the process and thread ids to ensure uniqueness.
  msec += OS::getpid();
  msec += (size_t) OS::thr_self();

  // ACE_thread_t may be a char* (returned by OS::thr_self()) so
  // we need to use a C-style cast as a catch-all in order to use a
  // static_cast<> to an integral type.
  ACE_RANDR_TYPE seed = static_cast<ACE_RANDR_TYPE> (msec);

  // We only care about UTF-8 / ASCII characters in generated
  // filenames.  A UTF-16 or UTF-32 character could potentially cause
  // a very large space to be searched in the below do/while() loop,
  // greatly slowing down this mkstemp() implementation.  It is more
  // practical to limit the search space to UTF-8 / ASCII characters
  // (i.e. 127 characters).
  //
  // Note that we can't make this constant static since the compiler
  // may not inline the return value of ACE_Numeric_Limits::max(),
  // meaning multiple threads could potentially initialize this value
  // in parallel.
  float const MAX_VAL =
    static_cast<float> (std::numeric_limits<char>::max ());

  // Use high-order bits rather than low-order ones (e.g. rand() %
  // MAX_VAL).  See Numerical Recipes in C: The Art of Scientific
  // Computing (William  H. Press, Brian P. Flannery, Saul
  // A. Teukolsky, William T. Vetterling; New York: Cambridge
  // University Press, 1992 (2nd ed., p. 277).
  //
  // e.g.: MAX_VAL * rand() / (RAND_MAX + 1.0)

  // Factor out the constant coefficient.
  float const coefficient =
    static_cast<float> (MAX_VAL / (RAND_MAX + 1.0f));

  // @@ These nested loops may be ineffecient.  Improvements are
  //    welcome.
  for (unsigned int i = 0; i < NUM_RETRIES; ++i)
    {
      for (unsigned int n = 0; n < NUM_CHARS; ++n)
        {
          ACE_TCHAR r;

          // This do/while() loop allows this alphanumeric character
          // selection to work for EBCDIC, as well.
          do
            {
              r = static_cast<ACE_TCHAR> (coefficient * OS::rand_r (seed));
            }
          while (!isalnum (r));

          t[n] = r;
        }

      static int const perms =
#if defined (ACE_WIN32)
        0;      /* Do not share while open. */
#else
        0600;   /* S_IRUSR | S_IWUSR */
#endif  /* ACE_WIN32 */

      // Create the file with the O_EXCL bit set to ensure that we're
      // not subject to a symbolic link attack.
      //
      // Note that O_EXCL is subject to a race condition over NFS
      // filesystems.
      ACE_HANDLE const handle = OS::open (s,
                                              O_RDWR | O_CREAT | O_EXCL,
                                              perms);

      if (handle != ACE_INVALID_HANDLE)
        return handle;
    }

  errno = EEXIST;  // Couldn't create a unique temporary file.
  return ACE_INVALID_HANDLE;
}
#endif /* ACE_LACKS_MKSTEMP */


