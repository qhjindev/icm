// $Id: OS_NS_strings.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_strings.h"

ACE_RCSID(ace, OS_NS_strings, "$Id: OS_NS_strings.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

#if defined (ACE_LACKS_STRCASECMP)
#  include "OS_NS_ctype.h"
#endif /* ACE_LACKS_STRCASECMP */


#if defined (ACE_HAS_WCHAR)
#  include "os/OS_NS_wchar.h"
#endif /* ACE_HAS_WCHAR */



ACE_INLINE int
OS::strcasecmp (const char *s, const char *t)
{
#if defined (ACE_LACKS_STRCASECMP)
  return OS::strcasecmp_emulation (s, t);
#elif defined (ACE_STRCASECMP_EQUIVALENT)
  return ACE_STRCASECMP_EQUIVALENT (s, t);
#else /* ACE_LACKS_STRCASECMP */
  return ::strcasecmp (s, t);
#endif /* ACE_LACKS_STRCASECMP */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::strcasecmp (const wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSICMP)
  return OS::wcsicmp_emulation (s, t);
#  else  /* ACE_LACKS_WCSICMP */
  return ::_wcsicmp (s, t);
#  endif /* ACE_LACKS_WCSICMP */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
OS::strncasecmp (const char *s, const char *t, size_t len)
{
#if defined (ACE_LACKS_STRCASECMP)
  return OS::strncasecmp_emulation (s, t, len);
#elif defined (ACE_STRNCASECMP_EQUIVALENT)
  return ACE_STRNCASECMP_EQUIVALENT (s, t, len);
#else /* ACE_LACKS_STRCASECMP */
  return ::strncasecmp (s, t, len);
#endif /* ACE_LACKS_STRCASECMP */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::strncasecmp (const wchar_t *s, const wchar_t *t, size_t len)
{
#if defined (ACE_LACKS_WCSNICMP)
  return OS::wcsnicmp_emulation (s, t, len);
#else  /* ACE_LACKS_WCSNICMP */
  return ::_wcsnicmp (s, t, len);
#endif /* ACE_LACKS_WCSNICMP */
}
#endif /* ACE_HAS_WCHAR */




#if defined (ACE_LACKS_STRCASECMP)
int
OS::strcasecmp_emulation (const char *s, const char *t)
{
  const char *scan1 = s;
  const char *scan2 = t;

  while (*scan1 != 0
         && OS::ace_tolower (*scan1)
            == OS::ace_tolower (*scan2))
    {
      ++scan1;
      ++scan2;
    }

  // The following case analysis is necessary so that characters which
  // look negative collate low against normal characters but high
  // against the end-of-string NUL.

  if (*scan1 == '\0' && *scan2 == '\0')
    return 0;
  else if (*scan1 == '\0')
    return -1;
  else if (*scan2 == '\0')
    return 1;
  else
    return OS::ace_tolower (*scan1) - OS::ace_tolower (*scan2);
}
#endif /* ACE_LACKS_STRCASECMP */

#if defined (ACE_LACKS_STRCASECMP)
int
OS::strncasecmp_emulation (const char *s,
                               const char *t,
                               size_t len)
{
  const char *scan1 = s;
  const char *scan2 = t;
  size_t count = 0;

  while (count++ < len
         && *scan1 != 0
         && OS::ace_tolower (*scan1)
            == OS::ace_tolower (*scan2))
    {
      ++scan1;
      ++scan2;
    }

  if (count > len)
    return 0;

  // The following case analysis is necessary so that characters which
  // look negative collate low against normal characters but high
  // against the end-of-string NUL.

  if (*scan1 == '\0' && *scan2 == '\0')
    return 0;
  else if (*scan1 == '\0')
    return -1;
  else if (*scan2 == '\0')
    return 1;
  else
    return OS::ace_tolower (*scan1) - OS::ace_tolower (*scan2);
}
#endif /* ACE_LACKS_STRCASECMP */


