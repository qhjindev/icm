// -*- C++ -*-
// $Id: OS_NS_arpa_inet.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_arpa_inet.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_errno.h"
#include "os/OS_NS_stdio.h"

ACE_RCSID(ace, OS_NS_arpa_inet, "$Id: OS_NS_arpa_inet.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")




int
OS::inet_aton (const char *host_name, struct in_addr *addr)
{
#if defined (ACE_LACKS_INET_ATON)
#  if defined (ACE_WIN32)
  // Windows Server 2003 changed the behavior of a zero-length input
  // string to inet_addr(). It used to return 0 (INADDR_ANY) but now
  // returns -1 (INADDR_NONE). It will return INADDR_ANY for a 1-space
  // string, though, as do previous versions of Windows.
  if (host_name == 0 || host_name[0] == '\0')
    host_name = " ";
#  endif /* ACE_WIN32 */
  unsigned long ip_addr = OS::inet_addr (host_name);

  if (ip_addr == INADDR_NONE
      // Broadcast addresses are weird...
      && OS::strcmp (host_name, "255.255.255.255") != 0)
    return 0;
  else if (addr == 0)
    return 0;
  else
    {
      addr->s_addr = ip_addr;  // Network byte ordered
      return 1;
    }
#elif defined (ACE_VXWORKS) && (ACE_VXWORKS <= 0x650)
  // inet_aton() returns OK (0) on success and ERROR (-1) on failure.
  // Must reset errno first. Refer to WindRiver SPR# 34949, SPR# 36026
  ::errnoSet(0);
  int result = ERROR;
  OSCALL (::inet_aton (const_cast <char*>(host_name), addr), int, ERROR, result);
  return (result == ERROR) ? 0 : 1;
#else
  // inet_aton() returns 0 upon failure, not -1 since -1 is a valid
  // address (255.255.255.255).
  OSCALL_RETURN (::inet_aton (host_name, addr), int, 0);
#endif  /* ACE_LACKS_INET_ATON */
}


ACE_INLINE unsigned long
OS::inet_addr (const char *name)
{
  OS_TRACE ("OS::inet_addr");
#if defined (ACE_LACKS_INET_ADDR)
  ACE_UNUSED_ARG (name);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_HAS_NONCONST_GETBY)
  return ::inet_addr (const_cast <char*> (name));
#else
  return ::inet_addr (name);
#endif /* ACE_HAS_NONCONST_GETBY */
}

ACE_INLINE char *
OS::inet_ntoa (const struct in_addr addr)
{
  OS_TRACE ("OS::inet_ntoa");
#if defined (ACE_LACKS_INET_NTOA)
  ACE_UNUSED_ARG (addr);
  ACE_NOTSUP_RETURN (0);
#else
  OSCALL_RETURN (::inet_ntoa (addr),
                     char *,
                     0);
#endif
}

ACE_INLINE const char *
OS::inet_ntop (int family, const void *addrptr, char *strptr, size_t len)
{
  OS_TRACE ("OS::inet_ntop");

#if defined (ACE_HAS_IPV6) && !defined (ACE_WIN32)
  OSCALL_RETURN (::inet_ntop (family, addrptr, strptr, len), const char *, 0);
#else
  //const u_char *p = reinterpret_cast<const u_char *> (addrptr);

  if (family == AF_INET)
    {
      char temp[INET_ADDRSTRLEN];

      // Stevens uses snprintf() in his implementation but snprintf()
      // doesn't appear to be very portable.  For now, hope that using
      // sprintf() will not cause any string/memory overrun problems.
      //sprintf (temp, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]); by qinghua maybe mustnot comment ,but ...

      if (OS::strlen (temp) >= len)
        {
          errno = ENOSPC;
          return 0; // Failure
        }

      OS::strcpy (strptr, temp);
      return strptr;
    }

  ACE_NOTSUP_RETURN(0);
#endif /* ACE_HAS_IPV6 */
}

ACE_INLINE int
OS::inet_pton (int family, const char *strptr, void *addrptr)
{
  OS_TRACE ("OS::inet_pton");

#if defined (ACE_HAS_IPV6) && !defined (ACE_WIN32)
  OSCALL_RETURN (::inet_pton (family, strptr, addrptr), int, -1);
#else
  if (family == AF_INET)
    {
      struct in_addr in_val;

      if (OS::inet_aton (strptr, &in_val))
        {
          OS::memcpy (addrptr, &in_val, sizeof (struct in_addr));
          return 1; // Success
        }

      return 0; // Input is not a valid presentation format
    }

  ACE_NOTSUP_RETURN(-1);
#endif  /* ACE_HAS_IPV6 */
}


