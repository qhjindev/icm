// -*- C++ -*-
// $Id: OS_NS_netdb.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_netdb.h"

ACE_RCSID(ace, OS_NS_netdb, "$Id: OS_NS_netdb.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

#include "os/OS_NS_macros.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_errno.h"

#include "os/os_include/net/os_if.h"
#include "os/OS_NS_unistd.h"
#if defined (ACE_WIN32) && defined (ACE_HAS_PHARLAP)
#include "os/OS_NS_stdio.h"
#endif
//#include "os/OS_NS_stropts.h"
#include "os/OS_NS_sys_socket.h"

#if defined (ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
# if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
#   define ACE_NETDBCALL_RETURN(OP,TYPE,FAILVALUE,TARGET,SIZE) \
  do \
  { \
    if (OS::netdb_acquire ())  \
      return FAILVALUE; \
    else \
      { \
        TYPE ace_result_; \
        OSCALL (OP, TYPE, FAILVALUE, ace_result_); \
        if (ace_result_ != FAILVALUE) \
          OS::memcpy (TARGET, \
                    ace_result_, \
                    SIZE < sizeof (TYPE) ? SIZE : sizeof (TYPE)); \
        OS::netdb_release (); \
        return ace_result_; \
      } \
  } while(0)
# else /* ! (ACE_MT_SAFE && ACE_MT_SAFE != 0) */
#   define ACE_NETDBCALL_RETURN(OP,TYPE,FAILVALUE,TARGET,SIZE) \
  do \
  { \
        TYPE ace_result_; \
        OSCALL(OP,TYPE,FAILVALUE,ace_result_); \
        if (ace_result_ != FAILVALUE) \
          OS::memcpy (TARGET, \
                    ace_result_, \
                    SIZE < sizeof (TYPE) ? SIZE : sizeof (TYPE)); \
        return ace_result_; \
  } while(0)
# endif /* ACE_MT_SAFE && ACE_MT_SAFE != 0 */
#endif /* ACE_LACKS_NETDB_REENTRANT_FUNCTIONS */



#if !(defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYADDR))

ACE_INLINE struct hostent *
OS::gethostbyaddr (const char *addr, int length, int type)
{
  OS_TRACE ("OS::gethostbyaddr");
# if defined (ACE_LACKS_GETHOSTBYADDR)
  ACE_UNUSED_ARG (addr);
  ACE_UNUSED_ARG (length);
  ACE_UNUSED_ARG (type);
  ACE_NOTSUP_RETURN (0);
# else

  if (0 == addr || '\0' == addr[0])
      return 0;

#   if defined (ACE_VXWORKS)
  // VxWorks 6.x has a gethostbyaddr() that is threadsafe and
  // returns an heap-allocated hostentry structure.
  // just call OS::gethostbyaddr_r () which knows how to handle this.
  struct hostent hentry;
  ACE_HOSTENT_DATA buf;
  int h_error;  // Not the same as errno!
  return OS::gethostbyaddr_r (addr, length, type, &hentry, buf, &h_error);
#   elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_SOCKCALL_RETURN (::gethostbyaddr (const_cast<char *> (addr),
                                        (ACE_SOCKET_LEN) length,
                                        type),
                       struct hostent *,
                       0);
#   else
  ACE_SOCKCALL_RETURN (::gethostbyaddr (addr,
                                        (ACE_SOCKET_LEN) length,
                                        type),
                       struct hostent *,
                       0);
#   endif /* ACE_HAS_NONCONST_GETBY */
# endif /* !ACE_LACKS_GETHOSTBYADDR */
}

#endif

#if !(defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYADDR))

ACE_INLINE struct hostent *
OS::gethostbyaddr_r (const char *addr,
                         int length,
                         int type,
                         struct hostent *result,
                         ACE_HOSTENT_DATA buffer,
                         int *h_errnop)
{
  OS_TRACE ("OS::gethostbyaddr_r");
# if defined (ACE_LACKS_GETHOSTBYADDR_R)
  ACE_UNUSED_ARG (addr);
  ACE_UNUSED_ARG (length);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (h_errnop);
  ACE_NOTSUP_RETURN (0);
# elif defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE)

  if (0 == addr || '\0' == addr[0])
      return 0;

#   if defined (AIX) || defined (DIGITAL_UNIX)
  OS::memset (buffer, 0, sizeof (ACE_HOSTENT_DATA));

  if (::gethostbyaddr_r ((char *) addr, length, type, result,
                         (struct hostent_data *) buffer)== 0)
    return result;
  else
    {
      *h_errnop = h_errno;
      return (struct hostent *) 0;
    }
#   elif defined (__GLIBC__)
  // GNU C library has a different signature
  OS::memset (buffer, 0, sizeof (ACE_HOSTENT_DATA));

  if (::gethostbyaddr_r ((char *) addr,
                         length,
                         type,
                         result,
                         buffer,
                         sizeof (ACE_HOSTENT_DATA),
                         &result,
                         h_errnop) == 0)
    return result;
  else
    return (struct hostent *) 0;
#   elif defined (ACE_VXWORKS)
  // VxWorks 6.x has a threadsafe gethostbyaddr() which returns a heap-allocated
  // data structure which needs to be freed with hostentFree()
  struct hostent* hp = ::gethostbyaddr (addr, length, type);
  if (hp)
  {
    result->h_addrtype = hp->h_addrtype;
    result->h_length = hp->h_length;

    // buffer layout:
    // buffer[0-3]: h_addr_list[0], pointer to the addr.
    // buffer[4-7]: h_addr_list[1], null terminator for the h_addr_list.
    // buffer[8..(8+h_length)]: the first (and only) addr.
    // buffer[(8+h_length)...]: hostname

    // Store the address list in buffer.
    result->h_addr_list = (char **) buffer;
    // Store the actual address _after_ the address list.
    result->h_addr_list[0] = (char *) &result->h_addr_list[2];
    OS::memcpy (result->h_addr_list[0], hp->h_addr_list[0], hp->h_length);
    // Null-terminate the list of addresses.
    result->h_addr_list[1] = 0;
    // And no aliases, so null-terminate h_aliases.
    result->h_aliases = &result->h_addr_list[1];

    if (((2*sizeof(char*))+hp->h_length+OS::strlen (hp->h_name)+1) > sizeof (ACE_HOSTENT_DATA))
    {
      result->h_name = (char *) result->h_addr_list[0] + hp->h_length;
      OS::strcpy (result->h_name, hp->h_name);
    }
    else
    {
      result->h_name = (char *)0;
    }

    // free hostent memory
    ::hostentFree (hp);

    return result;
  }
  else
  {
    return (struct hostent *) 0;
  }
#   else
#     if defined(ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (h_errnop);
  ACE_NETDBCALL_RETURN (::gethostbyaddr (addr, (ACE_SOCKET_LEN) length, type),
                        struct hostent *, 0,
                        buffer, sizeof (ACE_HOSTENT_DATA));
#     else
  ACE_SOCKCALL_RETURN (::gethostbyaddr_r (addr, length, type, result,
                                          buffer, sizeof (ACE_HOSTENT_DATA),
                                          h_errnop),
                       struct hostent *, 0);
#     endif /* ACE_LACKS_NETDB_REENTRANT_FUNCTIONS */
#   endif /* defined (AIX) || defined (DIGITAL_UNIX) */
# elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (h_errnop);
  ACE_SOCKCALL_RETURN (::gethostbyaddr (const_cast<char *> (addr),
                                        (ACE_SOCKET_LEN) length,
                                        type),
                       struct hostent *,
                       0);
# else
  ACE_UNUSED_ARG (h_errnop);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (result);

  ACE_SOCKCALL_RETURN (::gethostbyaddr (addr,
                                        (ACE_SOCKET_LEN) length,
                                        type),
                       struct hostent *,
                       0);
# endif /* ACE_LACKS_GETHOSTBYADDR_R */
}

#endif

#if !(defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYNAME))

ACE_INLINE struct hostent *
OS::gethostbyname (const char *name)
{
  OS_TRACE ("OS::gethostbyname");
# if defined (ACE_LACKS_GETHOSTBYNAME)
  ACE_UNUSED_ARG (name);
  ACE_NOTSUP_RETURN (0);
# else

  if (0 == name || '\0' == name[0])
      return 0;

#   if defined (ACE_VXWORKS)
  // VxWorks 6.x has a gethostbyname() that is threadsafe and
  // returns an heap-allocated hostentry structure.
  // just call OS::gethostbyname_r () which knows how to handle this.
  struct hostent hentry;
  ACE_HOSTENT_DATA buf;
  int h_error;  // Not the same as errno!
  return OS::gethostbyname_r (name, &hentry, buf, &h_error);
#   elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_SOCKCALL_RETURN (::gethostbyname (const_cast<char *> (name)),
                       struct hostent *,
                       0);
#   else
  ACE_SOCKCALL_RETURN (::gethostbyname (name),
                       struct hostent *,
                       0);
#   endif /* ACE_HAS_NONCONST_GETBY */
# endif /* !ACE_LACKS_GETHOSTBYNAME */
}

#endif

#if !(defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYNAME))

ACE_INLINE struct hostent *
OS::gethostbyname_r (const char *name,
                         struct hostent *result,
                         ACE_HOSTENT_DATA buffer,
                         int *h_errnop)
{
  OS_TRACE ("OS::gethostbyname_r");
#if defined (ACE_LACKS_GETHOSTBYNAME)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (h_errnop);
  ACE_NOTSUP_RETURN (0);
# elif defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE)

  if (0 == name || '\0' == name[0])
      return (struct hostent *)0;

#   if defined (DIGITAL_UNIX) || \
       (defined (ACE_AIX_MINOR_VERS) && (ACE_AIX_MINOR_VERS > 2))
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (h_errnop);

  // gethostbyname returns thread-specific storage on Digital Unix and
  // AIX 4.3
  ACE_SOCKCALL_RETURN (::gethostbyname (name), struct hostent *, 0);
#   elif defined (AIX)
  OS::memset (buffer, 0, sizeof (ACE_HOSTENT_DATA));

  if (::gethostbyname_r (name, result, (struct hostent_data *) buffer) == 0)
    return result;
  else
    {
      *h_errnop = h_errno;
      return (struct hostent *) 0;
    }
#   elif defined (__GLIBC__)
  // GNU C library has a different signature
  OS::memset (buffer, 0, sizeof (ACE_HOSTENT_DATA));

  if (::gethostbyname_r (name,
                         result,
                         buffer,
                         sizeof (ACE_HOSTENT_DATA),
                         &result,
                         h_errnop) == 0)
    return result;
  else
    return (struct hostent *) 0;
#   elif defined (ACE_VXWORKS)
  // VxWorks 6.x has a threadsafe gethostbyname() which returns a heap-allocated
  // data structure which needs to be freed with hostentFree()
  struct hostent* hp = ::gethostbyname (name);
  if (hp)
  {
    result->h_addrtype = hp->h_addrtype;
    result->h_length = hp->h_length;

    // buffer layout:
    // buffer[0-3]: h_addr_list[0], pointer to the addr.
    // buffer[4-7]: h_addr_list[1], null terminator for the h_addr_list.
    // buffer[8...]: the first (and only) addr.

    // Store the address list in buffer.
    result->h_addr_list = (char **) buffer;
    // Store the actual address _after_ the address list.
    result->h_addr_list[0] = (char *) &result->h_addr_list[2];
    OS::memcpy (result->h_addr_list[0], hp->h_addr_list[0], hp->h_length);
    // Null-terminate the list of addresses.
    result->h_addr_list[1] = 0;
    // And no aliases, so null-terminate h_aliases.
    result->h_aliases = &result->h_addr_list[1];

    if (((2*sizeof(char*))+hp->h_length+OS::strlen (hp->h_name)+1) > sizeof (ACE_HOSTENT_DATA))
    {
      result->h_name = (char *) result->h_addr_list[0] + hp->h_length;
      OS::strcpy (result->h_name, hp->h_name);
    }
    else
    {
      result->h_name = (char *)0;
    }

    // free hostent memory
    ::hostentFree (hp);

    return result;
  }
  else
  {
    return (struct hostent *) 0;
  }
#   else
#     if defined(ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (h_errnop);
  ACE_NETDBCALL_RETURN (::gethostbyname (name),
                        struct hostent *, 0,
                        buffer, sizeof (ACE_HOSTENT_DATA));
#     else
  ACE_SOCKCALL_RETURN (::gethostbyname_r (name, result, buffer,
                                          sizeof (ACE_HOSTENT_DATA),
                                          h_errnop),
                       struct hostent *,
                       0);
#     endif /* ACE_LACKS_NETDB_REENTRANT_FUNCTIONS */
#   endif /* defined (AIX) || defined (DIGITAL_UNIX) */
# elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (h_errnop);
  ACE_SOCKCALL_RETURN (::gethostbyname (const_cast<char *> (name)),
                       struct hostent *,
                       0);
# else
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (h_errnop);

  ACE_SOCKCALL_RETURN (::gethostbyname (name),
                       struct hostent *,
                       0);
# endif /* defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE) */
}

#endif

ACE_INLINE struct hostent *
OS::getipnodebyaddr (const void *src, size_t len, int family)
{
#if defined (ACE_HAS_IPV6) && !defined (ACE_WIN32)
#  if defined (ACE_LACKS_GETIPNODEBYADDR)
  ACE_UNUSED_ARG (src);
  ACE_UNUSED_ARG (len);
  ACE_UNUSED_ARG (family);
  ACE_NOTSUP_RETURN (0);
#  else
  struct hostent *hptr = 0;
  int errnum;
  if ((hptr = ::getipnodebyaddr (src, len, family, &errnum)) == 0)
    {
      errno = errnum;
    }
  return hptr;
#  endif /* ACE_LACKS_GETIPNODEBYADDR */
#else
  // IPv4-only implementation
  if (family == AF_INET)
    return OS::gethostbyaddr (static_cast<const char *> (src),
                                  static_cast<int> (len),
                                  family);

  ACE_NOTSUP_RETURN (0);
# endif /* defined (ACE_HAS_IPV6) && !defined (ACE_WIN32) */
}

ACE_INLINE struct hostent *
OS::getipnodebyname (const char *name, int family, int flags)
{
  OS_TRACE ("OS::getipnodebyname");
# if defined (ACE_HAS_IPV6) && !defined (ACE_LACKS_GETIPNODEBYNAME_IPV6)
#   if defined (ACE_LACKS_GETIPNODEBYNAME)
  ACE_UNUSED_ARG (flags);
#     if defined (ACE_HAS_NONCONST_GETBY)
  ACE_SOCKCALL_RETURN (::gethostbyname2 (const_cast<char *> (name),
                                         family),
                       struct hostent *, 0);
#     else
  ACE_SOCKCALL_RETURN (::gethostbyname2 (name, family),
                       struct hostent *, 0);
#     endif /* ACE_HAS_NONCONST_GETBY */
#   else
  struct hostent *hptr = 0;
  int errnum;
  if ((hptr = ::getipnodebyname (name, family, flags, &errnum)) == 0)
    {
      errno = errnum;
    }
  return hptr;
#   endif /* ACE_LACKS_GETIPNODEBYNAME */
# else
  // IPv4-only implementation
  ACE_UNUSED_ARG (flags);
  if (family == AF_INET)
    return OS::gethostbyname (name);

  ACE_NOTSUP_RETURN (0);
# endif /* defined (ACE_HAS_IPV6) && !ACE_LACKS_GETIPNODEBYNAME_IPV6 */
}

ACE_INLINE struct protoent *
OS::getprotobyname (const char *name)
{
#if defined (ACE_LACKS_GETPROTOBYNAME)
  ACE_UNUSED_ARG (name);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_SOCKCALL_RETURN (::getprotobyname (const_cast<char *> (name)),
                       struct protoent *,
                       0);
#else
  ACE_SOCKCALL_RETURN (::getprotobyname (name),
                       struct protoent *,
                       0);
#endif /* ACE_LACKS_GETPROTOBYNAME */
}

ACE_INLINE struct protoent *
OS::getprotobyname_r (const char *name,
                          struct protoent *result,
                          ACE_PROTOENT_DATA buffer)
{
#if defined (ACE_LACKS_GETPROTOBYNAME)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE)
# if defined (AIX) || defined (DIGITAL_UNIX)
  if (::getprotobyname_r (name, result, (struct protoent_data *) buffer) == 0)
    return result;
  else
    return 0;
# elif defined (__GLIBC__)
  // GNU C library has a different signature
  if (::getprotobyname_r (name,
                          result,
                          buffer,
                          sizeof (ACE_PROTOENT_DATA),
                          &result) == 0)
    return result;
  else
    return 0;
# else
#   if defined(ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
  ACE_UNUSED_ARG (result);
  ACE_NETDBCALL_RETURN (::getprotobyname (name),
                        struct protoent *, 0,
                        buffer, sizeof (ACE_PROTOENT_DATA));
#   else
    ACE_SOCKCALL_RETURN (::getprotobyname_r (name,
                                             result,
                                             buffer,
                                             sizeof (ACE_PROTOENT_DATA)),
                       struct protoent *, 0);
#   endif /* ACE_LACKS_NETDB_REENTRANT_FUNCTIONS */
# endif /* defined (AIX) || defined (DIGITAL_UNIX) */
#elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_SOCKCALL_RETURN (::getprotobyname (const_cast<char *> (name)),
                       struct protoent *, 0);
#else
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (result);

  ACE_SOCKCALL_RETURN (::getprotobyname (name),
                       struct protoent *,
                       0);
#endif /* defined (ACE_HAS_REENTRANT_FUNCTIONS) !defined (UNIXWARE) */
}

ACE_INLINE struct protoent *
OS::getprotobynumber (int proto)
{
#if defined (ACE_LACKS_GETPROTOBYNUMBER)
  ACE_UNUSED_ARG (proto);
  ACE_NOTSUP_RETURN (0);
#else
  ACE_SOCKCALL_RETURN (::getprotobynumber (proto),
                       struct protoent *, 0);
#endif /* ACE_LACKS_GETPROTOBYNUMBER */
}

ACE_INLINE struct protoent *
OS::getprotobynumber_r (int proto,
                            struct protoent *result,
                            ACE_PROTOENT_DATA buffer)
{
#if defined (ACE_LACKS_GETPROTOBYNUMBER)
  ACE_UNUSED_ARG (proto);
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buffer);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE)
# if defined (AIX) || defined (DIGITAL_UNIX)
  if (::getprotobynumber_r (proto, result, (struct protoent_data *) buffer) == 0)
    return result;
  else
    return 0;
# elif defined (__GLIBC__)
  // GNU C library has a different signature
  if (::getprotobynumber_r (proto,
                            result,
                            buffer,
                            sizeof (ACE_PROTOENT_DATA),
                            &result) == 0)
    return result;
  else
    return 0;
# else
#   if defined(ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
  ACE_UNUSED_ARG (result);
  ACE_NETDBCALL_RETURN (::getprotobynumber (proto),
                        struct protoent *, 0,
                        buffer, sizeof (ACE_PROTOENT_DATA));
#   else
  ACE_SOCKCALL_RETURN (::getprotobynumber_r (proto, result, buffer, sizeof (ACE_PROTOENT_DATA)),
                       struct protoent *, 0);
#   endif /* ACE_LACKS_NETDB_REENTRANT_FUNCTIONS */
# endif /* defined (AIX) || defined (DIGITAL_UNIX) */
#else
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (result);

  ACE_SOCKCALL_RETURN (::getprotobynumber (proto),
                       struct protoent *, 0);
#endif /* defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE) */
}

ACE_INLINE struct servent *
OS::getservbyname (const char *svc, const char *proto)
{
  OS_TRACE ("OS::getservbyname");
#if defined (ACE_LACKS_GETSERVBYNAME)
  ACE_UNUSED_ARG (svc);
  ACE_UNUSED_ARG (proto);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_SOCKCALL_RETURN (::getservbyname (const_cast<char *> (svc),
                                        const_cast<char *> (proto)),
                       struct servent *,
                       0);
#else
  ACE_SOCKCALL_RETURN (::getservbyname (svc,
                                        proto),
                       struct servent *,
                       0);
#endif /* ACE_HAS_NONCONST_GETBY */
}

ACE_INLINE struct servent *
OS::getservbyname_r (const char *svc,
                         const char *proto,
                         struct servent *result,
                         ACE_SERVENT_DATA buf)
{
  OS_TRACE ("OS::getservbyname_r");
#if defined (ACE_LACKS_GETSERVBYNAME)
  ACE_UNUSED_ARG (svc);
  ACE_UNUSED_ARG (proto);
  ACE_UNUSED_ARG (result);
  ACE_UNUSED_ARG (buf);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE)
# if defined (AIX) || defined (DIGITAL_UNIX)
  OS::memset (buf, 0, sizeof (ACE_SERVENT_DATA));

  if (::getservbyname_r (svc, proto, result, (struct servent_data *) buf) == 0)
    return result;
  else
    return (struct servent *) 0;
# elif defined (__GLIBC__)
  // GNU C library has a different signature
  OS::memset (buf, 0, sizeof (ACE_SERVENT_DATA));

  if (::getservbyname_r (svc,
                         proto,
                         result,
                         buf,
                         sizeof (ACE_SERVENT_DATA),
                         &result) == 0)
    return result;
  else
    return (struct servent *) 0;
# else
#   if defined(ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
  ACE_UNUSED_ARG (result);
  ACE_NETDBCALL_RETURN (::getservbyname (svc, proto),
                        struct servent *, 0,
                        buf, sizeof (ACE_SERVENT_DATA));
#   else
  ACE_SOCKCALL_RETURN (::getservbyname_r (svc, proto, result, buf,
                                          sizeof (ACE_SERVENT_DATA)),
                       struct servent *, 0);
#   endif /* ACE_LACKS_NETDB_REENTRANT_FUNCTIONS */
# endif /* defined (AIX) || defined (DIGITAL_UNIX) */
#elif defined (ACE_HAS_NONCONST_GETBY)
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (result);
  ACE_SOCKCALL_RETURN (::getservbyname (const_cast<char *> (svc),
                                        const_cast<char *> (proto)),
                       struct servent *,
                       0);
#else
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (result);

  ACE_SOCKCALL_RETURN (::getservbyname (svc,
                                        proto),
                       struct servent *,
                       0);
#endif /* defined (ACE_HAS_REENTRANT_FUNCTIONS) && !defined (UNIXWARE) */
}




#if defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYADDR)

struct hostent *
OS::gethostbyaddr (const char *addr, int length, int type)
{
  OS_TRACE ("OS::gethostbyaddr");

  if (length != 4 || type != AF_INET)
    {
      errno = EINVAL;
      return 0;
    }

  // not thread safe!
  static hostent ret;
  static char name [MAXNAMELEN + 1];
  static char *hostaddr[2];
  static char *aliases[1];

  if (::hostGetByAddr (*(int *) addr, name) != 0)
    {
      // errno will have been set to S_hostLib_UNKNOWN_HOST.
      return 0;
    }

  // Might not be official: just echo input arg.
  hostaddr[0] = (char *) addr;
  hostaddr[1] = 0;
  aliases[0] = 0;

  ret.h_name = name;
  ret.h_addrtype = AF_INET;
  ret.h_length = 4;  // VxWorks 5.2/3 doesn't define IP_ADDR_LEN;
  ret.h_addr_list = hostaddr;
  ret.h_aliases = aliases;

  return &ret;
}

#endif /* ACE_VXWORKS && ACE_LACKS_GETHOSTBYADDR */

#if defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYADDR)

struct hostent *
OS::gethostbyaddr_r (const char *addr, int length, int type,
                         hostent *result, ACE_HOSTENT_DATA buffer,
                         int *h_errnop)
{
  OS_TRACE ("OS::gethostbyaddr_r");
  if (length != 4 || type != AF_INET)
    {
      errno = EINVAL;
      return 0;
    }

  if (OS::netdb_acquire ())
    return 0;
  else
    {
      // buffer layout:
      // buffer[0-3]: h_addr_list[0], the first (and only) addr.
      // buffer[4-7]: h_addr_list[1], the null terminator for the h_addr_list.
      // buffer[8]: the name of the host, null terminated.

      // Call ::hostGetByAddr (), which puts the (one) hostname into
      // buffer.
      if (::hostGetByAddr (*(int *) addr, &buffer[8]) == 0)
        {
          // Store the return values in result.
          result->h_name = &buffer[8];  // null-terminated host name
          result->h_addrtype = AF_INET;
          result->h_length = 4;  // VxWorks 5.2/3 doesn't define IP_ADDR_LEN.

          result->h_addr_list = (char **) buffer;
          // Might not be official: just echo input arg.
          result->h_addr_list[0] = (char *) addr;
          // Null-terminate the list of addresses.
          result->h_addr_list[1] = 0;
          // And no aliases, so null-terminate h_aliases.
          result->h_aliases = &result->h_addr_list[1];
        }
      else
        {
          // errno will have been set to S_hostLib_UNKNOWN_HOST.
          result = 0;
        }
    }

  OS::netdb_release ();
  *h_errnop = errno;
  return result;
}

#endif /* ACE_VXWORKS && ACE_LACKS_GETHOSTBYADDR */

#if defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYNAME)

struct hostent *
OS::gethostbyname (const char *name)
{
  OS_TRACE ("OS::gethostbyname");

  // not thread safe!
  static hostent ret;
  static int first_addr;
  static char *hostaddr[2];
  static char *aliases[1];

  if (0 == name || '\0' == name[0])
    return 0;

  OSCALL (::hostGetByName ((char *) name), int, -1, first_addr);
  if (first_addr == -1)
    return 0;

  hostaddr[0] = (char *) &first_addr;
  hostaddr[1] = 0;
  aliases[0] = 0;

  // Might not be official: just echo input arg.
  ret.h_name = (char *) name;
  ret.h_addrtype = AF_INET;
  ret.h_length = 4;  // VxWorks 5.2/3 doesn't define IP_ADDR_LEN;
  ret.h_addr_list = hostaddr;
  ret.h_aliases = aliases;

  return &ret;
}

#endif /* ACE_VXWORKS && ACE_LACKS_GETHOSTBYNAME */

#if defined (ACE_VXWORKS) && defined (ACE_LACKS_GETHOSTBYNAME)

struct hostent *
OS::gethostbyname_r (const char *name, hostent *result,
                         ACE_HOSTENT_DATA buffer,
                         int *h_errnop)
{
  OS_TRACE ("OS::gethostbyname_r");

  if (0 == name || '\0' == name[0])
    return 0;

  if (OS::netdb_acquire ())
    return 0;
  else
    {
      int addr;
      OSCALL (::hostGetByName ((char *) name), int, -1, addr);

      if (addr == -1)
        {
          // errno will have been set to S_hostLib_UNKNOWN_HOST
          result = 0;
        }
      else
        {
          // Might not be official: just echo input arg.
          result->h_name = (char *) name;
          result->h_addrtype = AF_INET;
          result->h_length = 4;  // VxWorks 5.2/3 doesn't define IP_ADDR_LEN;

          // buffer layout:
          // buffer[0-3]: h_addr_list[0], pointer to the addr.
          // buffer[4-7]: h_addr_list[1], null terminator for the h_addr_list.
          // buffer[8-11]: the first (and only) addr.

          // Store the address list in buffer.
          result->h_addr_list = (char **) buffer;
          // Store the actual address _after_ the address list.
          result->h_addr_list[0] = (char *) &result->h_addr_list[2];
          result->h_addr_list[2] = (char *) addr;
          // Null-terminate the list of addresses.
          result->h_addr_list[1] = 0;
          // And no aliases, so null-terminate h_aliases.
          result->h_aliases = &result->h_addr_list[1];
        }
    }

  OS::netdb_release ();
  *h_errnop = errno;
  return result;
}

#endif /* ACE_VXWORKS && ACE_LACKS_GETHOSTBYNAME*/



// Include if_arp so that getmacaddr can use the
// arp structure.
#if defined (sun)
# include /**/ <net/if_arp.h>
#endif



int
OS::getmacaddress (struct macaddr_node_t *node)
{
  OS_TRACE ("OS::getmacaddress");

#if defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)
# if !defined (ACE_HAS_PHARLAP)
    /** Define a structure for use with the netbios routine */
    struct ADAPTERSTAT
    {
      ADAPTER_STATUS adapt;
      NAME_BUFFER    NameBuff [30];
    };

    NCB         ncb;
    LANA_ENUM   lenum;
    unsigned char result;

    OS::memset (&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBENUM;
    ncb.ncb_buffer  = reinterpret_cast<unsigned char*> (&lenum);
    ncb.ncb_length  = sizeof(lenum);

    result = Netbios (&ncb);

    for(int i = 0; i < lenum.length; i++)
      {
        OS::memset (&ncb, 0, sizeof(ncb));
        ncb.ncb_command  = NCBRESET;
        ncb.ncb_lana_num = lenum.lana [i];

        /** Reset the netbios */
        result = Netbios (&ncb);

        if (ncb.ncb_retcode != NRC_GOODRET)
        {
          return -1;
        }

        ADAPTERSTAT adapter;
        OS::memset (&ncb, 0, sizeof (ncb));
        OS::strcpy (reinterpret_cast<char*> (ncb.ncb_callname), "*");
        ncb.ncb_command     = NCBASTAT;
        ncb.ncb_lana_num    = lenum.lana[i];
        ncb.ncb_buffer      = reinterpret_cast<unsigned char*> (&adapter);
        ncb.ncb_length      = sizeof (adapter);

        result = Netbios (&ncb);

        if (result == 0)
        {
          OS::memcpy (node->node,
              adapter.adapt.adapter_address,
              6);
          return 0;
        }
      }
    return 0;
# else
#   if defined (ACE_HAS_PHARLAP_RT)
      DEVHANDLE ip_dev = (DEVHANDLE)0;
      EK_TCPIPCFG *devp;
      size_t i;
      ACE_TCHAR dev_name[16];

      for (i = 0; i < 10; i++)
        {
          // Ethernet.
          OS::sprintf (dev_name,
                           "ether%d",
                           i);
          ip_dev = EtsTCPGetDeviceHandle (dev_name);
          if (ip_dev != 0)
            break;
        }
      if (ip_dev == 0)
        return -1;
      devp = EtsTCPGetDeviceCfg (ip_dev);
      if (devp == 0)
        return -1;
      OS::memcpy (node->node,
            &devp->EthernetAddress[0],
            6);
      return 0;
#   else
      ACE_UNUSED_ARG (node);
      ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_PHARLAP_RT */
# endif /* ACE_HAS_PHARLAP */
#elif defined (sun)

  /** obtain the local host name */
  char hostname [MAXHOSTNAMELEN];
  OS::hostname (hostname, sizeof (hostname));

  /** Get the hostent to use with ioctl */
  struct hostent *phost =
    OS::gethostbyname (hostname);

  if (phost == 0)
    return -1;

  ACE_HANDLE handle =
    OS::socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (handle == ACE_INVALID_HANDLE)
    return -1;

  char **paddrs = phost->h_addr_list;

  struct arpreq ar;

  struct sockaddr_in *psa =
    (struct sockaddr_in *)&(ar.arp_pa);

  OS::memset (&ar,
                  0,
                  sizeof (struct arpreq));

  psa->sin_family = AF_INET;

  OS::memcpy (&(psa->sin_addr),
                  *paddrs,
                  sizeof (struct in_addr));

  if (OS::ioctl (handle,
                     SIOCGARP,
                     &ar) == -1)
    {
      OS::close (handle);
      return -1;
    }

  OS::close (handle);

  OS::memcpy (node->node,
                  ar.arp_ha.sa_data,
                  6);

  return 0;

#elif defined (linux) && !defined (ACE_LACKS_NETWORKING)

  struct ifreq ifr;

  ACE_HANDLE handle =
    OS::socket (PF_INET, SOCK_DGRAM, 0);

  if (handle == ACE_INVALID_HANDLE)
    return -1;

  OS::strcpy (ifr.ifr_name, "eth0");

  if (OS::ioctl (handle/*s*/, SIOCGIFHWADDR, &ifr) < 0)
    {
      OS::close (handle);
      return -1;
    }

  struct sockaddr* sa =
    (struct sockaddr *) &ifr.ifr_addr;

  OS::close (handle);

  OS::memcpy (node->node,
                  sa->sa_data,
                  6);

  return 0;

#else
  ACE_UNUSED_ARG (node);
  ACE_NOTSUP_RETURN (-1);
#endif
}



# if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0) && defined (ACE_LACKS_NETDB_REENTRANT_FUNCTIONS)
#   include "OS_NS_Thread.h"
//#   include "Object_Manager_Base.h"


/*
int
OS::netdb_acquire (void)
{
  return OS::thread_mutex_lock ((ACE_thread_mutex_t *)
    OS_Object_Manager::preallocated_object[
      OS_Object_Manager::OS_MONITOR_LOCK]);
}

int
OS::netdb_release (void)
{
  return OS::thread_mutex_unlock ((ACE_thread_mutex_t *)
    OS_Object_Manager::preallocated_object[
      OS_Object_Manager::OS_MONITOR_LOCK]);
}
*/

static ACE_thread_mutex_t mutex;
int
OS::netdb_acquire (void)
{
  return OS::thread_mutex_lock (&mutex);
}

int
OS::netdb_release (void)
{
  return OS::thread_mutex_unlock (&mutex);
}
# endif /* defined (ACE_LACKS_NETDB_REENTRANT_FUNCTIONS) */

