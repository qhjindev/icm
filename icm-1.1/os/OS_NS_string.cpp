// $Id: OS_NS_string.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_string.h"
#include "os/OS_NS_stdlib.h"
//#include "ACE.h"
#include "os/OS_Errno.h"

#if defined (ACE_HAS_WCHAR)
#  include "os/OS_NS_stdlib.h"
#endif /* ACE_HAS_WCHAR */

#if !defined (ACE_LACKS_STRERROR)
#  include "os/OS_NS_stdio.h"
#endif /* ACE_LACKS_STRERROR */


// OS_NS_wchar.h is only needed to get the emulation methods.
// Perhaps they should be moved.  dhinton
#include "os/OS_NS_wchar.h"
#include "os/os_include/os_string.h"



ACE_INLINE const void *
OS::memchr (const void *s, int c, size_t len)
{
#if !defined (ACE_LACKS_MEMCHR)
  return ::memchr (s, c, len);
#else /* ACE_LACKS_MEMCHR */
  return OS::memchr_emulation (s, c, len);
#endif /* !ACE_LACKS_MEMCHR */
}

ACE_INLINE void *
OS::memchr (void *s, int c, size_t len)
{
  return const_cast<void *> (OS::memchr (static_cast<const void *> (s),
                                             c,
                                             len));
}

ACE_INLINE int
OS::memcmp (const void *t, const void *s, size_t len)
{
  return ::memcmp (t, s, len);
}

ACE_INLINE void *
OS::memcpy (void *t, const void *s, size_t len)
{
#if defined (ACE_HAS_MEMCPY_LOOP_UNROLL)
  return fast_memcpy (t, s, len);
#else
  return ::memcpy (t, s, len);
#endif /* ACE_HAS_MEMCPY_LOOP_UNROLL */
}

ACE_INLINE void *
OS::memmove (void *t, const void *s, size_t len)
{
  return ::memmove (t, s, len);
}

ACE_INLINE void *
OS::memset (void *s, int c, size_t len)
{
#if defined (ACE_HAS_SLOW_MEMSET)
  // This section requires a high optimization level (-xO4 with SunCC)
  // in order to actually be inlined.
  char* ptr = static_cast<char*> (s);
  switch (len)
    {
    case 16:
      ptr[15] = c;
    case 15:
      ptr[14] = c;
    case 14:
      ptr[13] = c;
    case 13:
      ptr[12] = c;
    case 12:
      ptr[11] = c;
    case 11:
      ptr[10] = c;
    case 10:
      ptr[9] = c;
    case 9:
      ptr[8] = c;
    case 8:
      ptr[7] = c;
    case 7:
      ptr[6] = c;
    case 6:
      ptr[5] = c;
    case 5:
      ptr[4] = c;
    case 4:
      ptr[3] = c;
    case 3:
      ptr[2] = c;
    case 2:
      ptr[1] = c;
    case 1:
      ptr[0] = c;
      break;
    default:
      for (size_t i = 0; i < len; ++i)
        {
          ptr[i] = c;
        }
    }

  return s;
#else
  return ::memset (s, c, len);
#endif /* ACE_HAS_SLOW_MEMSET */
}

ACE_INLINE char *
OS::strcat (char *s, const char *t)
{
  return ::strcat (s, t);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strcat (wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSCAT)
  return OS::wcscat_emulation (s, t);
#  else /* ACE_LACKS_WCSCAT */
  return ::wcscat (s, t);
#  endif /* ACE_LACKS_WCSCAT */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE const char *
OS::strchr (const char *s, int c)
{
  return const_cast <const char *> (::strchr (s, c));
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE const wchar_t *
OS::strchr (const wchar_t *s, wchar_t c)
{
#  if defined (ACE_LACKS_WCSCHR)
  return OS::wcschr_emulation (s, c);
#  else /* ACE_LACKS_WCSCHR */
  return ::wcschr (s, c);
#  endif /* ACE_LACKS_WCSCHR */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE char *
OS::strchr (char *s, int c)
{
  return ::strchr (s, c);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strchr (wchar_t *s, wchar_t c)
{
  return
    const_cast<wchar_t *> (OS::strchr (const_cast<const wchar_t *> (s),
                                           c));
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
OS::strcmp (const char *s, const char *t)
{
  return ::strcmp (s, t);
}

ACE_INLINE int
OS::strcmp (const ACE_WCHAR_T *s, const ACE_WCHAR_T *t)
{
#  if !defined (ACE_HAS_WCHAR) || defined (ACE_LACKS_WCSCMP)
  return OS::wcscmp_emulation (s, t);
#  else /* !ACE_HAS_WCHAR || ACE_LACKS_WCSCMP */
  return ::wcscmp (s, t);
#  endif /* !ACE_HAS_WCHAR || ACE_LACKS_WCSCMP */
}

ACE_INLINE char *
OS::strcpy (char *s, const char *t)
{
  return ::strcpy (s, t);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strcpy (wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSCPY)
  return OS::wcscpy_emulation (s, t);
#  else /* ACE_LACKS_WCSCPY */
  return ::wcscpy (s, t);
#  endif /* ACE_LACKS_WCSCPY */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE size_t
OS::strcspn (const char *s, const char *reject)
{
  return ::strcspn (s, reject);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE size_t
OS::strcspn (const wchar_t *s, const wchar_t *reject)
{
#  if defined (ACE_LACKS_WCSCSPN)
  return OS::wcscspn_emulation (s, reject);
#  else /* ACE_LACKS_WCSCSPN */
  return ::wcscspn (s, reject);
#  endif /* ACE_LACKS_WCSCSPN */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE char *
OS::strdup (const char *s)
{
#  if (defined (ACE_LACKS_STRDUP) && !defined(ACE_STRDUP_EQUIVALENT)) \
  || defined (ACE_HAS_STRDUP_EMULATION)
  return OS::strdup_emulation (s);
#  elif defined (ACE_STRDUP_EQUIVALENT)
  return ACE_STRDUP_EQUIVALENT (s);
#  elif defined (ACE_HAS_NONCONST_STRDUP)
  return ::strdup (const_cast<char *> (s));
#else
  return ::strdup (s);
#  endif /* (ACE_LACKS_STRDUP && !ACE_STRDUP_EQUIVALENT) || ... */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strdup (const wchar_t *s)
{
#  if (defined (ACE_LACKS_WCSDUP) && !defined (ACE_WCSDUP_EQUIVALENT)) \
  || defined (ACE_HAS_WCSDUMP_EMULATION)
  return OS::strdup_emulation (s);
#  elif defined (ACE_WCSDUP_EQUIVALENT)
  return ACE_WCSDUP_EQUIVALENT (s);
#  elif defined (ACE_HAS_NONCONST_WCSDUP)
  return ::wcsdup (const_cast<wchar_t*> (s));
#  else
  return ::wcsdup (s);
#  endif /* (ACE_LACKS_WCSDUP && !ACE_WCSDUP_EQUIVALENT) || ... */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE size_t
OS::strlen (const char *s)
{
  return ::strlen (s);
}

ACE_INLINE size_t
OS::strlen (const ACE_WCHAR_T *s)
{
# if !defined (ACE_HAS_WCHAR) || defined (ACE_LACKS_WCSLEN)
  return OS::wcslen_emulation (s);
# else  /* !ACE_HAS_WCHAR || ACE_LACKS_WCSLEN */
  return ::wcslen (s);
# endif /* !ACE_HAS_WCHAR || ACE_LACKS_WCSLEN */
}

ACE_INLINE char *
OS::strncat (char *s, const char *t, size_t len)
{
#if 0 /* defined (ACE_HAS_TR24731_2005_CRT) */
  strncat_s (s, len + 1, t, _TRUNCATE);
  return s;
#else
  return ::strncat (s, t, len);
#endif /* ACE_HAS_TR24731_2005_CRT */
}

ACE_INLINE ACE_WCHAR_T *
OS::strncat (ACE_WCHAR_T *s, const ACE_WCHAR_T *t, size_t len)
{
#  if !defined (ACE_HAS_WCHAR) || defined (ACE_LACKS_WCSNCAT)
  return OS::wcsncat_emulation (s, t, len);
#  elif 0 /* defined (ACE_HAS_TR24731_2005_CRT) */
  wcsncat_s (s, len + 1, t, _TRUNCATE);
  return s;
#  else /* !ACE_HAS_WCHAR || ACE_LACKS_WCSNCAT */
  return ::wcsncat (s, t, len);
#  endif /* !ACE_HAS_WCHAR || ACE_LACKS_WCSNCAT */
}

ACE_INLINE char *
OS::strnchr (char *s, int c, size_t len)
{
  return const_cast<char *> (OS::strnchr (static_cast<const char *> (s),
                                              c,
                                              len));
}

ACE_INLINE ACE_WCHAR_T *
OS::strnchr (ACE_WCHAR_T *s, ACE_WCHAR_T c, size_t len)
{
  return
    const_cast<ACE_WCHAR_T *> (OS::strnchr (
                                 const_cast<const ACE_WCHAR_T *> (s),
                                 c,
                                 len));
}

ACE_INLINE int
OS::strncmp (const char *s, const char *t, size_t len)
{
  return ::strncmp (s, t, len);
}

ACE_INLINE int
OS::strncmp (const ACE_WCHAR_T *s, const ACE_WCHAR_T *t, size_t len)
{
#  if !defined (ACE_HAS_WCHAR) || defined (ACE_LACKS_WCSNCMP)
  return OS::wcsncmp_emulation (s, t, len);
#  else /* !ACE_HAS_WCHAR || ACE_LACKS_WCSNCMP */
  return ::wcsncmp (s, t, len);
#  endif /* !ACE_HAS_WCHAR || ACE_LACKS_WCSNCMP */
}

ACE_INLINE char *
OS::strncpy (char *s, const char *t, size_t len)
{
  return ::strncpy (s, t, len);
}

ACE_INLINE ACE_WCHAR_T *
OS::strncpy (ACE_WCHAR_T *s, const ACE_WCHAR_T *t, size_t len)
{
#  if !defined (ACE_HAS_WCHAR) || defined (ACE_LACKS_WCSNCPY)
  return OS::wcsncpy_emulation (s, t, len);
#  else /* !ACE_HAS_WCHAR || ACE_LACKS_WCSNCPY */
  return ::wcsncpy (s, t, len);
#  endif /* !ACE_HAS_WCHAR || ACE_LACKS_WCSNCPY */
}

ACE_INLINE size_t
OS::strnlen (const char *s, size_t maxlen)
{
#if defined (ACE_HAS_STRNLEN)
  return ::strnlen (s, maxlen);
#else /* ACE_HAS_STRNLEN */
  size_t i;
  for (i = 0; i < maxlen; ++i)
    if (s[i] == '\0')
      break;
  return i;
#endif /* ACE_HAS_STRNLEN */
}

ACE_INLINE size_t
OS::strnlen (const ACE_WCHAR_T *s, size_t maxlen)
{
#if defined (ACE_HAS_WCHAR) && defined (ACE_HAS_WCSNLEN)
  return wcsnlen (s, maxlen);
#else /* ACE_HAS_WCSNLEN */
  size_t i;
  for (i = 0; i < maxlen; ++i)
    if (s[i] == '\0')
      break;
  return i;
#endif /* ACE_HAS_WCSNLEN */
}

ACE_INLINE char *
OS::strnstr (char *s, const char *t, size_t len)
{
  return
    const_cast <char *> (OS::strnstr (const_cast <const char *> (s), t, len));
}

ACE_INLINE ACE_WCHAR_T *
OS::strnstr (ACE_WCHAR_T *s, const ACE_WCHAR_T *t, size_t len)
{
  return
    const_cast<ACE_WCHAR_T *> (OS::strnstr (
                                 static_cast<const ACE_WCHAR_T *> (s),
                                 t,
                                 len));
}

ACE_INLINE const char *
OS::strpbrk (const char *s1, const char *s2)
{
  return const_cast <const char *> (::strpbrk (s1, s2));
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE const wchar_t *
OS::strpbrk (const wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSPBRK)
  return OS::wcspbrk_emulation (s, t);
#  else /* ACE_LACKS_WCSPBRK */
  return ::wcspbrk (s, t);
#  endif /* ACE_LACKS_WCSPBRK */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE char *
OS::strpbrk (char *s1, const char *s2)
{
  return ::strpbrk (s1, s2);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strpbrk (wchar_t *s, const wchar_t *t)
{
  return const_cast<wchar_t *> (OS::strpbrk (
                                  const_cast<const wchar_t *> (s), t));
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE const char *
OS::strrchr (const char *s, int c)
{
#if defined (ACE_LACKS_STRRCHR)
  return OS::strrchr_emulation (s, c);
#else  /* ! ACE_LACKS_STRRCHR */
  return (const char *) ::strrchr (s, c);
#endif /* ! ACE_LACKS_STRRCHR */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE const wchar_t *
OS::strrchr (const wchar_t *s, wchar_t c)
{
#if defined (ACE_LACKS_WCSRCHR)
  return OS::wcsrchr_emulation (s, c);
#else /* ! ACE_LACKS_WCSRCHR */
  return const_cast <const wchar_t *> (::wcsrchr (s, c));
#endif /* ! ACE_LACKS_WCSRCHR */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE char *
OS::strrchr (char *s, int c)
{
#if defined (ACE_LACKS_STRRCHR)
  return OS::strrchr_emulation (s, c);
#else  /* ! ACE_LACKS_STRRCHR */
  return ::strrchr (s, c);
#endif /* ! ACE_LACKS_STRRCHR */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strrchr (wchar_t *s, wchar_t c)
{
  return const_cast<wchar_t *> (OS::strrchr (
                     const_cast<const wchar_t *> (s), c));
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE size_t
OS::strspn (const char *s, const char *t)
{
  return ::strspn (s, t);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE size_t
OS::strspn (const wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSSPN)
  return OS::wcsspn_emulation (s, t);
#  else /* ACE_LACKS_WCSSPN */
  return ::wcsspn (s, t);
#  endif /* ACE_LACKS_WCSSPN */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE const char *
OS::strstr (const char *s, const char *t)
{
  return (const char *) ::strstr (s, t);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE const wchar_t *
OS::strstr (const wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSSTR)
  return OS::wcsstr_emulation (s, t);
#  elif defined (HPUX)
  return const_cast <const wchar_t *> (::wcswcs (s, t));
#  else /* ACE_LACKS_WCSSTR */
  return const_cast <const wchar_t *> (::wcsstr (s, t));
#  endif /* ACE_LACKS_WCSSTR */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE char *
OS::strstr (char *s, const char *t)
{
  return ::strstr (s, t);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::strstr (wchar_t *s, const wchar_t *t)
{
#  if defined (ACE_LACKS_WCSSTR)
  return OS::wcsstr_emulation (s, t);
#  elif defined (HPUX)
  return ::wcswcs (s, t);
#  else /* ACE_LACKS_WCSSTR */
  return ::wcsstr (s, t);
#  endif /* ACE_LACKS_WCSSTR */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE char *
OS::strtok (char *s, const char *tokens)
{
  return ::strtok (s, tokens);
}

#if defined (ACE_HAS_WCHAR) && !defined (ACE_LACKS_WCSTOK)
ACE_INLINE wchar_t *
OS::strtok (wchar_t *s, const wchar_t *tokens)
{
#if defined (ACE_HAS_3_PARAM_WCSTOK)
  static wchar_t *lasts = 0;
  return ::wcstok (s, tokens, &lasts);
#else
  return ::wcstok (s, tokens);
#endif /* ACE_HAS_3_PARAM_WCSTOK */
}
#endif /* ACE_HAS_WCHAR && !ACE_LACKS_WCSTOK */

ACE_INLINE char *
OS::strtok_r (char *s, const char *tokens, char **lasts)
{
#if defined (ACE_HAS_TR24731_2005_CRT)
  return strtok_s (s, tokens, lasts);
#elif defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (ACE_LACKS_STRTOK_R)
  return ::strtok_r (s, tokens, lasts);
#else
  return OS::strtok_r_emulation (s, tokens, lasts);
#endif /* (ACE_HAS_REENTRANT_FUNCTIONS) */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t*
OS::strtok_r (ACE_WCHAR_T *s, const ACE_WCHAR_T *tokens, ACE_WCHAR_T **lasts)
{
#if defined (ACE_HAS_TR24731_2005_CRT)
  return wcstok_s (s, tokens, lasts);
#elif defined (ACE_LACKS_WCSTOK)
  return OS::strtok_r_emulation (s, tokens, lasts);
#else
#  if defined (ACE_HAS_3_PARAM_WCSTOK)
  return ::wcstok (s, tokens, lasts);
#  else /* ACE_HAS_3_PARAM_WCSTOK */
  *lasts = ::wcstok (s, tokens);
  return *lasts;
#  endif /* ACE_HAS_3_PARAM_WCSTOK */
#endif  /* ACE_LACKS_WCSTOK */
}
#endif  // ACE_HAS_WCHAR




#if defined (ACE_LACKS_MEMCHR)
const void *
OS::memchr_emulation (const void *s, int c, size_t len)
{
  const unsigned char *t = (const unsigned char *) s;
  const unsigned char *e = (const unsigned char *) s + len;

  while (t < e)
    if (((int) *t) == c)
      return t;
    else
      ++t;

  return 0;
}
#endif /* ACE_LACKS_MEMCHR */

#if (defined (ACE_LACKS_STRDUP) && !defined (ACE_STRDUP_EQUIVALENT)) \
  || defined (ACE_HAS_STRDUP_EMULATION)
char *
OS::strdup_emulation (const char *s)
{
  char *t = (char *) OS::malloc (OS::strlen (s) + 1);
  if (t == 0)
    return 0;

  return OS::strcpy (t, s);
}
#endif /* (ACE_LACKS_STRDUP && !ACE_STRDUP_EQUIVALENT) || ... */

#if defined (ACE_HAS_WCHAR)
#if (defined (ACE_LACKS_WCSDUP) && !defined (ACE_WCSDUP_EQUIVALENT)) \
  || defined (ACE_HAS_WCSDUP_EMULATION)
wchar_t *
OS::strdup_emulation (const wchar_t *s)
{
  wchar_t *buffer =
    (wchar_t *) OS::malloc ((OS::strlen (s) + 1)
                                * sizeof (wchar_t));
  if (buffer == 0)
    return 0;

  return OS::strcpy (buffer, s);
}
#endif /* (ACE_LACKS_WCSDUP && !ACE_WCSDUP_EQUIVALENT) || ... */
#endif /* ACE_HAS_WCHAR */

char *
OS::strecpy (char *s, const char *t)
{
  register char *dscan = s;
  register const char *sscan = t;

  while ((*dscan++ = *sscan++) != '\0')
    continue;

  return dscan;
}

#if defined (ACE_HAS_WCHAR)
wchar_t *
OS::strecpy (wchar_t *s, const wchar_t *t)
{
  register wchar_t *dscan = s;
  register const wchar_t *sscan = t;

  while ((*dscan++ = *sscan++) != ACE_TEXT_WIDE ('\0'))
    continue;

  return dscan;
}
#endif /* ACE_HAS_WCHAR */

bool
OS::is_sock_error (int error)
{
#if defined (ACE_WIN32)
  switch (error)
    {
    case WSAVERNOTSUPPORTED:
    case WSASYSNOTREADY:
    case WSAEINVAL:
    case WSAHOST_NOT_FOUND:
    case WSATRY_AGAIN:
    case WSANO_RECOVERY:
    case WSANO_DATA:
      /*
        case WSANO_ADDRESS:
      */
    case WSANOTINITIALISED:
    case WSAENETDOWN:
    case WSAEINPROGRESS:
    case WSAEINTR:
    case WSAEAFNOSUPPORT:
    case WSAEMFILE:
    case WSAENOBUFS:
    case WSAEPROTONOSUPPORT:
    case WSAEPROTOTYPE:
    case WSAESOCKTNOSUPPORT:
    case WSAENOTSOCK:
    case WSAEWOULDBLOCK:
    case WSAEADDRINUSE:
    case WSAECONNABORTED:
    case WSAECONNRESET:
    case WSAENOTCONN:
    case WSAETIMEDOUT:
    case WSAECONNREFUSED:
    case WSAEHOSTDOWN:
    case WSAEHOSTUNREACH:
    case WSAEADDRNOTAVAIL:
    case WSAEISCONN:
    case WSAENETRESET:
    case WSAEMSGSIZE:
    case WSAENETUNREACH:
    case WSAEFAULT:
    case WSAEDISCON:
    case WSAEACCES:
    case WSAESHUTDOWN:
    case WSAEPROCLIM:
    case WSAEALREADY:
    case WSAEPFNOSUPPORT:
    case WSAENOPROTOOPT:
    case WSATYPE_NOT_FOUND:
    case WSAEOPNOTSUPP:
      return true;
    }
#else
  ACE_UNUSED_ARG (error);
#endif /* ACE_WIN32 */
  return false;
}

const ACE_TCHAR *
OS::sock_error (int error)
{
#if defined (ACE_WIN32)
  static ACE_TCHAR unknown_msg[64];

  switch (error)
    {
    case WSAVERNOTSUPPORTED:
      return ACE_TEXT ("version of WinSock not supported");
      /* NOTREACHED */
    case WSASYSNOTREADY:
      return ACE_TEXT ("WinSock not present or not responding");
      /* NOTREACHED */
    case WSAEINVAL:
      return ACE_TEXT ("app version not supported by DLL");
      /* NOTREACHED */
    case WSAHOST_NOT_FOUND:
      return ACE_TEXT ("Authoritive: Host not found");
      /* NOTREACHED */
    case WSATRY_AGAIN:
      return ACE_TEXT ("Non-authoritive: host not found or server failure");
      /* NOTREACHED */
    case WSANO_RECOVERY:
      return ACE_TEXT ("Non-recoverable: refused or not implemented");
      /* NOTREACHED */
    case WSANO_DATA:
      return ACE_TEXT ("Valid name, no data record for type");
      /* NOTREACHED */
      /*
        case WSANO_ADDRESS:
        return "Valid name, no MX record";
      */
    case WSANOTINITIALISED:
      return ACE_TEXT ("WSA Startup not initialized");
      /* NOTREACHED */
    case WSAENETDOWN:
      return ACE_TEXT ("Network subsystem failed");
      /* NOTREACHED */
    case WSAEINPROGRESS:
      return ACE_TEXT ("Blocking operation in progress");
      /* NOTREACHED */
    case WSAEINTR:
      return ACE_TEXT ("Blocking call cancelled");
      /* NOTREACHED */
    case WSAEAFNOSUPPORT:
      return ACE_TEXT ("address family not supported");
      /* NOTREACHED */
    case WSAEMFILE:
      return ACE_TEXT ("no file handles available");
      /* NOTREACHED */
    case WSAENOBUFS:
      return ACE_TEXT ("no buffer space available");
      /* NOTREACHED */
    case WSAEPROTONOSUPPORT:
      return ACE_TEXT ("specified protocol not supported");
      /* NOTREACHED */
    case WSAEPROTOTYPE:
      return ACE_TEXT ("protocol wrong type for this socket");
      /* NOTREACHED */
    case WSAESOCKTNOSUPPORT:
      return ACE_TEXT ("socket type not supported for address family");
      /* NOTREACHED */
    case WSAENOTSOCK:
      return ACE_TEXT ("handle is not a socket");
      /* NOTREACHED */
    case WSAEWOULDBLOCK:
      return ACE_TEXT ("resource temporarily unavailable");
      /* NOTREACHED */
    case WSAEADDRINUSE:
      return ACE_TEXT ("address already in use");
      /* NOTREACHED */
    case WSAECONNABORTED:
      return ACE_TEXT ("connection aborted");
      /* NOTREACHED */
    case WSAECONNRESET:
      return ACE_TEXT ("connection reset");
      /* NOTREACHED */
    case WSAENOTCONN:
      return ACE_TEXT ("not connected");
      /* NOTREACHED */
    case WSAETIMEDOUT:
      return ACE_TEXT ("connection timed out");
      /* NOTREACHED */
    case WSAECONNREFUSED:
      return ACE_TEXT ("connection refused");
      /* NOTREACHED */
    case WSAEHOSTDOWN:
      return ACE_TEXT ("host down");
      /* NOTREACHED */
    case WSAEHOSTUNREACH:
      return ACE_TEXT ("host unreachable");
      /* NOTREACHED */
    case WSAEADDRNOTAVAIL:
      return ACE_TEXT ("address not available");
      /* NOTREACHED */
    case WSAEISCONN:
      return ACE_TEXT ("socket is already connected");
      /* NOTREACHED */
    case WSAENETRESET:
      return ACE_TEXT ("network dropped connection on reset");
      /* NOTREACHED */
    case WSAEMSGSIZE:
      return ACE_TEXT ("message too long");
      /* NOTREACHED */
    case WSAENETUNREACH:
      return ACE_TEXT ("network is unreachable");
      /* NOTREACHED */
    case WSAEFAULT:
      return ACE_TEXT ("bad address");
      /* NOTREACHED */
    case WSAEDISCON:
      return ACE_TEXT ("graceful shutdown in progress");
      /* NOTREACHED */
    case WSAEACCES:
      return ACE_TEXT ("permission denied");
      /* NOTREACHED */
    case WSAESHUTDOWN:
      return ACE_TEXT ("cannot send after socket shutdown");
      /* NOTREACHED */
    case WSAEPROCLIM:
      return ACE_TEXT ("too many processes");
      /* NOTREACHED */
    case WSAEALREADY:
      return ACE_TEXT ("operation already in progress");
      /* NOTREACHED */
    case WSAEPFNOSUPPORT:
      return ACE_TEXT ("protocol family not supported");
      /* NOTREACHED */
    case WSAENOPROTOOPT:
      return ACE_TEXT ("bad protocol option");
      /* NOTREACHED */
    case WSATYPE_NOT_FOUND:
      return ACE_TEXT ("class type not found");
      /* NOTREACHED */
    case WSAEOPNOTSUPP:
      return ACE_TEXT ("operation not supported");
      /* NOTREACHED */
    case WSAEDESTADDRREQ:
      return ACE_TEXT ("destination address required");
      /* NOTREACHED */
    default:
      OS::sprintf (unknown_msg, ACE_TEXT ("unknown error: %d"), error);
      return unknown_msg;
      /* NOTREACHED */
    }
#else
  ACE_UNUSED_ARG (error);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_WIN32 */
}

char *
OS::strerror (int errnum)
{
  static char ret_errortext[128];

  if (OS::is_sock_error (errnum))
    {
      const ACE_TCHAR *errortext = OS::sock_error (errnum);
      OS::strncpy (ret_errortext,
                       ACE_TEXT_ALWAYS_CHAR (errortext),
                       sizeof (ret_errortext));
      return ret_errortext;
    }
#if defined (ACE_LACKS_STRERROR)
  errno = EINVAL;
  return OS::strerror_emulation (errnum);
#else /* ACE_LACKS_STRERROR */
  // Adapt to the various ways that strerror() indicates a bad errnum.
  // Most modern systems set errno to EINVAL. Some older platforms return
  // a pointer to a NULL string. This code makes the behavior more consistent
  // across platforms. On a bad errnum, we make a string with the error number
  // and set errno to EINVAL.
  ACE_Errno_Guard g (errno);
  errno = 0;
  char *errmsg;

#if defined (ACE_HAS_TR24731_2005_CRT)
  errmsg = ret_errortext;
  ACE_SECURECRTCALL (strerror_s (ret_errortext, sizeof (ret_errortext), errno),
                     char *, 0, errmsg);
  return errmsg;
#elif defined (ACE_WIN32)
  if (errnum < 0 || errnum >= _sys_nerr)
    errno = EINVAL;
#endif /* ACE_WIN32 */
  errmsg = ::strerror (errnum);

  if (errno == EINVAL || errmsg == 0 || errmsg[0] == 0)
    {
      //sprintf (ret_errortext, "Unknown error %d", errnum);
      errmsg = ret_errortext;
      g = EINVAL;
    }
  return errmsg;
#endif /* ACE_LACKS_STRERROR */
}

#if defined (ACE_LACKS_STRERROR)
/**
 * Just returns "Unknown Error" all the time.
 */
char *
OS::strerror_emulation (int errnum)
{
  return "Unknown Error";
}
#endif /* ACE_LACKS_STRERROR */

const char *
OS::strnchr (const char *s, int c, size_t len)
{
  for (size_t i = 0; i < len; ++i)
    if (s[i] == c)
      return s + i;

  return 0;
}

const ACE_WCHAR_T *
OS::strnchr (const ACE_WCHAR_T *s, ACE_WCHAR_T c, size_t len)
{
  for (size_t i = 0; i < len; ++i)
    if (s[i] == c)
      return s + i;

  return 0;
}

const char *
OS::strnstr (const char *s1, const char *s2, size_t len2)
{
  // Substring length
  size_t const len1 = OS::strlen (s1);

  // Check if the substring is longer than the string being searched.
  if (len2 > len1)
    return 0;

  // Go upto <len>
  size_t const len = len1 - len2;

  for (size_t i = 0; i <= len; i++)
    {
      if (OS::memcmp (s1 + i, s2, len2) == 0)
        // Found a match!  Return the index.
        return s1 + i;
    }

  return 0;
}

const ACE_WCHAR_T *
OS::strnstr (const ACE_WCHAR_T *s1, const ACE_WCHAR_T *s2, size_t len2)
{
  // Substring length
  const size_t len1 = OS::strlen (s1);

  // Check if the substring is longer than the string being searched.
  if (len2 > len1)
    return 0;

  // Go upto <len>
  const size_t len = len1 - len2;

  for (size_t i = 0; i <= len; i++)
    {
      if (OS::memcmp (s1 + i, s2, len2 * sizeof (ACE_WCHAR_T)) == 0)
        // Found a match!  Return the index.
        return s1 + i;
    }

  return 0;
}

#if defined (ACE_HAS_MEMCPY_LOOP_UNROLL)
void *
OS::fast_memcpy (void *t, const void *s, size_t len)
{
  unsigned char* to = static_cast<unsigned char*> (t) ;
  const unsigned char* from = static_cast<const unsigned char*> (s) ;
  // Unroll the loop...
  switch (len)
    {
    case 16: to[15] = from[15];
    case 15: to[14] = from[14];
    case 14: to[13] = from[13];
    case 13: to[12] = from[12];
    case 12: to[11] = from[11];
    case 11: to[10] = from[10];
    case 10: to[9] = from[9];
    case  9: to[8] = from[8];
    case  8: to[7] = from[7];
    case  7: to[6] = from[6];
    case  6: to[5] = from[5];
    case  5: to[4] = from[4];
    case  4: to[3] = from[3];
    case  3: to[2] = from[2];
    case  2: to[1] = from[1];
    case  1: to[0] = from[0];
    case  0: return t;
    default: return ::memcpy (t, s, len);
    }
}
#endif /* ACE_HAS_MEMCPY_LOOP_UNROLL */

#if defined (ACE_LACKS_STRRCHR)
char *
OS::strrchr_emulation (char *s, int c)
{
  char *p = s + OS::strlen (s);

  while (*p != c)
    if (p == s)
      return 0;
    else
      --p;

  return p;
}

const char *
OS::strrchr_emulation (const char *s, int c)
{
  const char *p = s + OS::strlen (s);

  while (*p != c)
    if (p == s)
      return 0;
    else
      --p;

  return p;
}
#endif /* ACE_LACKS_STRRCHR */

char *
OS::strsncpy (char *dst, const char *src, size_t maxlen)
{
  register char *rdst = dst;
  register const char *rsrc = src;
  register size_t rmaxlen = maxlen;

  if (rmaxlen > 0)
    {
      if (rdst!=rsrc)
        {
          *rdst = '\0';
          if (rsrc != 0)
            strncat (rdst, rsrc, --rmaxlen);
        }
      else
        {
          rdst += (rmaxlen - 1);
          *rdst = '\0';
        }
    }
  return dst;
}

ACE_WCHAR_T *
OS::strsncpy (ACE_WCHAR_T *dst, const ACE_WCHAR_T *src, size_t maxlen)
{
  register ACE_WCHAR_T *rdst = dst;
  register const ACE_WCHAR_T *rsrc = src;
  register size_t rmaxlen = maxlen;

  if (rmaxlen > 0)
    {
      if (rdst!=rsrc)
        {
          *rdst = ACE_TEXT_WIDE ('\0');
          if (rsrc != 0)
            strncat (rdst, rsrc, --rmaxlen);
        }
      else
        {
          rdst += (rmaxlen - 1);
          *rdst = ACE_TEXT_WIDE ('\0');
        }
    }
  return dst;
}

#if (!defined (ACE_HAS_REENTRANT_FUNCTIONS) || defined (ACE_LACKS_STRTOK_R)) \
    && !defined (ACE_HAS_TR24731_2005_CRT)
char *
OS::strtok_r_emulation (char *s, const char *tokens, char **lasts)
{
  if (s == 0)
    s = *lasts;
  else
    *lasts = s;
  if (*s == 0)                  // We have reached the end
    return 0;
  size_t l_org = OS::strlen (s);
  s = ::strtok (s, tokens);
  if (s == 0)
    return 0;
  const size_t l_sub = OS::strlen (s);
  if (s + l_sub < *lasts + l_org)
    *lasts = s + l_sub + 1;
  else
    *lasts = s + l_sub;
  return s ;
}
#endif /* !ACE_HAS_REENTRANT_FUNCTIONS */

# if defined (ACE_HAS_WCHAR) && defined (ACE_LACKS_WCSTOK)
wchar_t*
OS::strtok_r_emulation (ACE_WCHAR_T *s,
                            const ACE_WCHAR_T *tokens,
                            ACE_WCHAR_T **lasts)
{
  if (s == 0)
    s = *lasts;
  else
    *lasts = s;
  if (*s == 0)                  // We have reached the end
    return 0;
  int l_org = OS::strlen (s);
  s = OS::strtok (s, tokens);
  if (s == 0)
    return 0;
  const int l_sub = OS::strlen (s);
  if (s + l_sub < *lasts + l_org)
    *lasts = s + l_sub + 1;
  else
    *lasts = s + l_sub;
  return s ;
}
# endif  /* ACE_HAS_WCHAR && ACE_LACKS_WCSTOK */


