// $Id: OS_NS_sys_time.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_sys_time.h"

#include "os/os_include/sys/os_time.h"
#include "os/os_include/os_errno.h"

#if defined (ACE_VXWORKS)
#  include "icc/ OS_NS_time.h"
#endif /* ACE_VXWORKS */



#if defined (ACE_WIN32) && defined (_WIN32_WCE)
// Something is a bit brain-damaged here and I'm not sure what... this code
// compiled before the OS reorg for ACE 5.4. Since then it hasn't - eVC
// complains that the operators that return TimeValue are C-linkage
// functions that can't return a C++ class. The only way I've found to
// defeat this is to wrap the whole class in extern "C++".
//    - Steve Huston, 23-Aug-2004
extern "C++" {
#endif

ACE_INLINE TimeValue
OS::gettimeofday (void)
{
  // OS_TRACE ("OS::gettimeofday");

#if !defined (ACE_HAS_WINCE)&& !defined (ACE_WIN32)
  timeval tv;
  int result = 0;
#endif // !defined (ACE_HAS_WINCE)&& !defined (ACE_WIN32)

#if (0)
  struct timespec ts;

  OSCALL (OS::clock_gettime (CLOCK_REALTIME, &ts), int, -1, result);
  tv.tv_sec = ts.tv_sec;
  tv.tv_usec = ts.tv_nsec / 1000L;  // timespec has nsec, but timeval has usec

#elif defined (ACE_HAS_WINCE)
  SYSTEMTIME tsys;
  FILETIME   tfile;
  ::GetSystemTime (&tsys);
  ::SystemTimeToFileTime (&tsys, &tfile);
  return TimeValue (tfile);
#elif defined (ACE_WIN32)
  FILETIME   tfile;
  ::GetSystemTimeAsFileTime (&tfile);
  return TimeValue (tfile);
#if 0
  // From Todd Montgomery...
  struct _timeb tb;
  ::_ftime (&tb);
  tv.tv_sec = tb.time;
  tv.tv_usec = 1000 * tb.millitm;
#endif /* 0 */
#elif defined (ACE_HAS_AIX_HI_RES_TIMER)
  timebasestruct_t tb;

  ::read_real_time (&tb, TIMEBASE_SZ);
  ::time_base_to_time (&tb, TIMEBASE_SZ);

  tv.tv_sec = tb.tb_high;
  tv.tv_usec = tb.tb_low / 1000L;
#else
# if defined (ACE_HAS_TIMEZONE_GETTIMEOFDAY) || \
  defined(ACE_HAS_VOIDPTR_GETTIMEOFDAY) || \
  (defined (ACE_HAS_SVR4_GETTIMEOFDAY) && !defined (SCO))
  OSCALL (::gettimeofday (&tv, 0), int, -1, result);
# elif defined (ACE_VXWORKS)
  // Assumes that struct timespec is same size as struct timeval,
  // which assumes that time_t is a long: it currently (VxWorks
  // 5.2/5.3) is.
  struct timespec ts;

  OSCALL (OS::clock_gettime (CLOCK_REALTIME, &ts), int, -1, result);
  tv.tv_sec = ts.tv_sec;
  tv.tv_usec = ts.tv_nsec / 1000L;  // timespec has nsec, but timeval has usec
# else
  OSCALL (::gettimeofday (&tv), int, -1, result);
# endif /* ACE_HAS_SVR4_GETTIMEOFDAY */
#endif /* 0 */
#if !defined (ACE_HAS_WINCE)&& !defined (ACE_WIN32)
  if (result == -1)
    return TimeValue ((time_t)-1);
  else
    return TimeValue (tv);
#endif // !defined (ACE_HAS_WINCE)&& !defined (ACE_WIN32)
}

int
OS::gettimeofday(struct timeval* tp, void*)
{
  *tp = (timeval)OS::gettimeofday();
  return 0;
}

#if defined (ACE_WIN32) && defined (_WIN32_WCE)
}
#endif


