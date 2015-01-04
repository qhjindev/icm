// $Id: OS_NS_unistd.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_unistd.h"

ACE_RCSID (ace, OS_NS_unistd, "$Id: OS_NS_unistd.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

//#include "Base_Thread_Adapter.h"
#include "os/OS_NS_stdlib.h"
//#include "os/OS_NS_ctype.h"
#include "os/Default_Constants.h"
#include "os/OS_Memory.h"
#include "os/OS_NS_Thread.h"
//#include "Object_Manager_Base.h"
#include "os/Auto_Ptr.h"
#include "os/os_include/sys/os_pstat.h"
#include "os/os_include/sys/os_sysctl.h"

#include "os/OS_NS_sys_utsname.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_errno.h"
#include "os/OS_NS_macros.h"
#include "os/OS_NS_fcntl.h"
#include "os/Default_Constants.h"
#include "os/OS_Memory.h"
//#include "Truncate.h"

#if defined (ACE_HAS_CLOCK_GETTIME)
# include "os/os_include/os_time.h"
#endif /* ACE_HAS_CLOCK_GETTIME */

#if defined (ACE_LACKS_ACCESS)
#  include "os/OS_NS_stdio.h"
#endif /* ACE_LACKS_ACCESS */


#if defined (ACE_NEEDS_FTRUNCATE)
extern "C" int
ftruncate (ACE_HANDLE handle, long len)
{
  struct flock fl;
  fl.l_whence = 0;
  fl.l_len = 0;
  fl.l_start = len;
  fl.l_type = F_WRLCK;

  return OS::fcntl (handle, F_FREESP, reinterpret_cast <long> (&fl));
}
#endif /* ACE_NEEDS_FTRUNCATE */

#if defined (ACE_VXWORKS) || defined (ACE_HAS_WINCE)
#  include "os_include/os_unistd.h"
#  if defined (ACE_VXWORKS) && ((ACE_VXWORKS >= 0x620) && (ACE_VXWORKS <= 0x650))
#    if defined (__RTP__)
#      include "os_include/os_strings.h"
#    else
#      include "os_include/os_string.h"
#    endif
#  endif
#endif /* VXWORKS || ACE_HAS_WINCE */



ACE_INLINE int
OS::access (const char *path, int amode)
{
  OS_TRACE ("OS::access");
#if defined (ACE_LACKS_ACCESS)
#  if defined (ACE_HAS_WINCE) || defined (ACE_VXWORKS)
  // @@ WINCE: There should be a Win32 API that can do this.
  // Hard coded read access here.
  ACE_UNUSED_ARG (amode);
  FILE* handle = OS::fopen (ACE_TEXT_CHAR_TO_TCHAR(path),
                                ACE_TEXT ("r"));
  if (handle != 0)
    {
      OS::fclose (handle);
      return 0;
    }
  return (-1);
#  else
    ACE_UNUSED_ARG (path);
    ACE_UNUSED_ARG (amode);
    ACE_NOTSUP_RETURN (-1);
#  endif  // ACE_HAS_WINCE
#elif defined(ACE_WIN32)
  // Windows doesn't support checking X_OK(6)
  OSCALL_RETURN (::access (path, amode & 6), int, -1);
#else
  OSCALL_RETURN (::access (path, amode), int, -1);
#endif /* ACE_LACKS_ACCESS */
}


#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::access (const wchar_t *path, int amode)
{
#if defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)
  OSCALL_RETURN (::_waccess (path, amode), int, -1);
#else /* ACE_WIN32 && !ACE_HAS_WINCE */
  return OS::access (ACE_Wide_To_Ascii (path).char_rep (), amode);
#endif /* ACE_WIN32 && !ACE_HAS_WINCE */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE u_int
OS::alarm (u_int secs)
{
  OS_TRACE ("OS::alarm");
#if defined (ACE_LACKS_ALARM)
  ACE_UNUSED_ARG (secs);
  ACE_NOTSUP_RETURN (0);
#else
  return ::alarm (secs);
#endif /* ACE_LACKS_ALARM */
}

ACE_INLINE long
OS::getpagesize (void)
{
  OS_TRACE ("OS::getpagesize");
#if defined (ACE_WIN32) && !defined (ACE_HAS_PHARLAP)
  SYSTEM_INFO sys_info;
  ::GetSystemInfo (&sys_info);
  return (long) sys_info.dwPageSize;
#elif defined (_SC_PAGESIZE) && !defined (ACE_HAS_NOTSUP_SC_PAGESIZE)
  return ::sysconf (_SC_PAGESIZE);
#elif defined (ACE_HAS_GETPAGESIZE)
  return ::getpagesize ();
#else
  // Use the default set in config.h
  return ACE_PAGE_SIZE;
#endif /* ACE_WIN32 */
}

ACE_INLINE long
OS::allocation_granularity (void)
{
#if defined (ACE_WIN32)
  SYSTEM_INFO sys_info;
  ::GetSystemInfo (&sys_info);
  return sys_info.dwAllocationGranularity;
#else
  return OS::getpagesize ();
#endif /* ACE_WIN32 */
}

#if !defined (ACE_LACKS_CHDIR)
ACE_INLINE int
OS::chdir (const char *path)
{
  OS_TRACE ("OS::chdir");
#if defined (ACE_HAS_NONCONST_CHDIR)
  OSCALL_RETURN (::chdir (const_cast<char *> (path)), int, -1);
#elif defined (ACE_HAS_WINCE)
  ACE_UNUSED_ARG (path);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::chdir (path), int, -1);
#endif /* ACE_HAS_NONCONST_CHDIR */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::chdir (const wchar_t *path)
{
#if defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)
  OSCALL_RETURN (::_wchdir (path), int, -1);
#else /* ACE_WIN32 */
  return OS::chdir (ACE_Wide_To_Ascii (path).char_rep ());
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */
#endif /* ACE_LACKS_CHDIR */

ACE_INLINE int
OS::rmdir (const char *path)
{
#if defined (ACE_HAS_WINCE)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::RemoveDirectory (ACE_TEXT_CHAR_TO_TCHAR (path)),
                                          ace_result_),
                        int, -1);
#else
  OSCALL_RETURN (::rmdir (path), int, -1);
#endif /* ACE_WIN32 */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::rmdir (const wchar_t *path)
{
#if defined (ACE_HAS_WINCE)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::RemoveDirectoryW (path),
                                          ace_result_),
                        int, -1);
#elif defined (ACE_WIN32)
  OSCALL_RETURN (::_wrmdir (path), int, -1);
#else
  ACE_Wide_To_Ascii n_path (path);
  return OS::rmdir (n_path.char_rep ());
#endif /* ACE_HAS_WINCE */
}
#endif /* ACE_HAS_WCHAR */

// @todo: which 4 and why???  dhinton
// NOTE: The following four function definitions must appear before
// OS::sema_init ().

ACE_INLINE int
OS::close (ACE_HANDLE handle)
{
  OS_TRACE ("OS::close");
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CloseHandle (handle), ace_result_), int, -1);
#else
  OSCALL_RETURN (::close (handle), int, -1);
#endif /* ACE_WIN32 */
}

ACE_INLINE ACE_HANDLE
OS::dup (ACE_HANDLE handle)
{
  OS_TRACE ("OS::dup");
#if defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)
  ACE_HANDLE new_fd;
  if (::DuplicateHandle(::GetCurrentProcess (),
                        handle,
                        ::GetCurrentProcess(),
                        &new_fd,
                        0,
                        TRUE,
                        DUPLICATE_SAME_ACCESS))
    return new_fd;
  else
    ACE_FAIL_RETURN (ACE_INVALID_HANDLE);
  /* NOTREACHED */
#elif defined (ACE_LACKS_DUP)
  ACE_UNUSED_ARG (handle);
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_HAS_WINCE)
  ACE_UNUSED_ARG (handle);
  ACE_NOTSUP_RETURN (0);
#else
  OSCALL_RETURN (::dup (handle), ACE_HANDLE, ACE_INVALID_HANDLE);
#endif /* ACE_WIN32 && !ACE_HAS_WINCE */
}

ACE_INLINE int
OS::dup2 (ACE_HANDLE oldhandle, ACE_HANDLE newhandle)
{
  OS_TRACE ("OS::dup2");
#if defined (ACE_LACKS_DUP2)
  // msvcrt has _dup2 ?!
  ACE_UNUSED_ARG (oldhandle);
  ACE_UNUSED_ARG (newhandle);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::dup2 (oldhandle, newhandle), int, -1);
#endif /* ACE_LACKS_DUP2 */
}

ACE_INLINE int
OS::execv (const char *path,
               char *const argv[])
{
  OS_TRACE ("OS::execv");
#if defined (ACE_LACKS_EXEC)
  ACE_UNUSED_ARG (path);
  ACE_UNUSED_ARG (argv);

  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
# if defined (__BORLANDC__) /* VSB */
  return ::execv (path, argv);
# elif defined (__MINGW32__)
  return ::_execv (path, (char *const *) argv);
# else
  // Why this odd-looking code? If execv() returns at all, it's an error.
  // Windows defines this as returning an intptr_t rather than a simple int,
  // and the conversion triggers compile warnings. So just return -1 if
  // the call returns.
  ::_execv (path, (const char *const *) argv);
  return -1;
# endif /* __BORLANDC__ */
#else
  OSCALL_RETURN (::execv (path, argv), int, -1);
#endif /* ACE_LACKS_EXEC */
}

ACE_INLINE int
OS::execve (const char *path,
                char *const argv[],
                char *const envp[])
{
  OS_TRACE ("OS::execve");
#if defined (ACE_LACKS_EXEC)
  ACE_UNUSED_ARG (path);
  ACE_UNUSED_ARG (argv);
  ACE_UNUSED_ARG (envp);

  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
# if defined (__BORLANDC__) /* VSB */
  return ::execve (path, argv, envp);
# elif defined (__MINGW32__)
  return ::_execve (path, (char *const *) argv, (char *const *) envp);
# else
  // Why this odd-looking code? If execv() returns at all, it's an error.
  // Windows defines this as returning an intptr_t rather than a simple int,
  // and the conversion triggers compile warnings. So just return -1 if
  // the call returns.
  ::_execve (path, (const char *const *) argv, (const char *const *) envp);
  return -1;
# endif /* __BORLANDC__ */
#else
  OSCALL_RETURN (::execve (path, argv, envp), int, -1);
#endif /* ACE_LACKS_EXEC */
}

ACE_INLINE int
OS::execvp (const char *file,
                char *const argv[])
{
  OS_TRACE ("OS::execvp");
#if defined (ACE_LACKS_EXEC)
  ACE_UNUSED_ARG (file);
  ACE_UNUSED_ARG (argv);

  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
# if defined (__BORLANDC__) /* VSB */
  return ::execvp (file, argv);
# elif defined (__MINGW32__)
  return ::_execvp (file, (char *const *) argv);
# else
  // Why this odd-looking code? If execv() returns at all, it's an error.
  // Windows defines this as returning an intptr_t rather than a simple int,
  // and the conversion triggers compile warnings. So just return -1 if
  // the call returns.
  ::_execvp (file, (const char *const *) argv);
  return -1;
# endif /* __BORLANDC__ */
#else
  OSCALL_RETURN (::execvp (file, argv), int, -1);
#endif /* ACE_LACKS_EXEC */
}

ACE_INLINE pid_t
OS::fork (void)
{
  OS_TRACE ("OS::fork");
#if defined (ACE_LACKS_FORK)
  ACE_NOTSUP_RETURN (pid_t (-1));
#else
  OSCALL_RETURN (::fork (), pid_t, -1);
#endif /* ACE_LACKS_FORK */
}

ACE_INLINE int
OS::fsync (ACE_HANDLE handle)
{
  OS_TRACE ("OS::fsync");
# if defined (ACE_LACKS_FSYNC)
  ACE_UNUSED_ARG (handle);
  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::FlushFileBuffers (handle), ace_result_), int, -1);
# else
  OSCALL_RETURN (::fsync (handle), int, -1);
# endif /* ACE_LACKS_FSYNC */
}

ACE_INLINE int
OS::ftruncate (ACE_HANDLE handle, ACE_OFF_T offset)
{
  OS_TRACE ("OS::ftruncate");
#if defined (ACE_WIN32)
#  if !defined (ACE_LACKS_WIN32_SETFILEPOINTEREX)
  LARGE_INTEGER loff;
  loff.QuadPart = offset;
  if (::SetFilePointerEx (handle, loff, 0, FILE_BEGIN))
#  else
  if (::SetFilePointer (handle,
                        offset,
                        0,
                        FILE_BEGIN) != INVALID_SET_FILE_POINTER)
#  endif
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::SetEndOfFile (handle), ace_result_), int, -1);
  else
    ACE_FAIL_RETURN (-1);
#else
  OSCALL_RETURN (::ftruncate (handle, offset), int, -1);
#endif /* ACE_WIN32 */
}

ACE_INLINE char *
OS::getcwd (char *buf, size_t size)
{
  OS_TRACE ("OS::getcwd");
#if defined (ACE_LACKS_GETCWD)
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (size);
  ACE_NOTSUP_RETURN (0);
#elif defined (ACE_WIN32)
  return ::getcwd (buf, static_cast<int> (size));
#else
  OSCALL_RETURN (::getcwd (buf, size), char *, 0);
#endif /* ACE_LACKS_GETCWD */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE wchar_t *
OS::getcwd (wchar_t *buf, size_t size)
{
#  if defined (ACE_HAS_WINCE)
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (size);
  ACE_NOTSUP_RETURN (0);
#  elif defined (ACE_WIN32)
  return ::_wgetcwd (buf, static_cast<int> (size));
#  else
  char *narrow_buf = new char[size];
  char *result = 0;
  result = OS::getcwd (narrow_buf, size);
  ACE_Ascii_To_Wide wide_buf (result);
  delete [] narrow_buf;
  if (result != 0)
    OS::strsncpy (buf, wide_buf.wchar_rep (), size);
  return result == 0 ? 0 : buf;
#  endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE gid_t
OS::getgid (void)
{
  OS_TRACE ("OS::getgid");
#if defined (ACE_LACKS_GETGID)
  ACE_NOTSUP_RETURN (static_cast<gid_t> (-1));
# else
  OSCALL_RETURN (::getgid (), gid_t, static_cast<gid_t> (-1));
# endif /* ACE_LACKS_GETGID */
}

ACE_INLINE gid_t
OS::getegid (void)
{
  OS_TRACE ("OS::getegid");
#if defined (ACE_LACKS_GETEGID)
  ACE_NOTSUP_RETURN (static_cast<gid_t> (-1));
# else
  OSCALL_RETURN (::getegid (), gid_t, static_cast<gid_t> (-1));
# endif /* ACE_LACKS_GETEGID */
}

ACE_INLINE int
OS::getopt (int argc, char *const *argv, const char *optstring)
{
  OS_TRACE ("OS::getopt");
#if defined (ACE_LACKS_GETOPT)
  ACE_UNUSED_ARG (argc);
  ACE_UNUSED_ARG (argv);
  ACE_UNUSED_ARG (optstring);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::getopt (argc, argv, optstring), int, -1);
# endif /* ACE_LACKS_GETOPT */
}

ACE_INLINE pid_t
OS::getpgid (pid_t pid)
{
  OS_TRACE ("OS::getpgid");
#if defined (ACE_LACKS_GETPGID)
  ACE_UNUSED_ARG (pid);
  ACE_NOTSUP_RETURN (-1);
#elif defined (linux) && __GLIBC__ > 1 && __GLIBC_MINOR__ >= 0
  // getpgid() is from SVR4, which appears to be the reason why GLIBC
  // doesn't enable its prototype by default.
  // Rather than create our own extern prototype, just use the one
  // that is visible (ugh).
  OSCALL_RETURN (::__getpgid (pid), pid_t, -1);
#else
  OSCALL_RETURN (::getpgid (pid), pid_t, -1);
#endif /* ACE_LACKS_GETPGID */
}

ACE_INLINE pid_t
OS::getpid (void)
{
  // OS_TRACE ("OS::getpid");
#if defined (ACE_LACKS_GETPID)
  ACE_NOTSUP_RETURN (-1);
#elif defined (ACE_WIN32)
  return ::GetCurrentProcessId ();
#else
  OSCALL_RETURN (::getpid (), int, -1);
#endif /* ACE_LACKS_GETPID */
}

ACE_INLINE pid_t
OS::getppid (void)
{
  OS_TRACE ("OS::getppid");
#if defined (ACE_LACKS_GETPPID)
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::getppid (), pid_t, -1);
#endif /* ACE_LACKS_GETPPID */
}

ACE_INLINE uid_t
OS::getuid (void)
{
  OS_TRACE ("OS::getuid");
#if defined (ACE_LACKS_GETUID)
  ACE_NOTSUP_RETURN (static_cast<uid_t> (-1));
# else
  OSCALL_RETURN (::getuid (), uid_t, static_cast<uid_t> (-1));
# endif /* ACE_LACKS_GETUID*/
}

ACE_INLINE uid_t
OS::geteuid (void)
{
  OS_TRACE ("OS::geteuid");
#if defined (ACE_LACKS_GETEUID)
  ACE_NOTSUP_RETURN (static_cast<uid_t> (-1));
# else
  OSCALL_RETURN (::geteuid (), uid_t, (uid_t) -1);
# endif /* ACE_LACKS_GETEUID */
}

ACE_INLINE int
OS::hostname (char name[], size_t maxnamelen)
{
  OS_TRACE ("OS::hostname");
#if defined (ACE_HAS_PHARLAP)
  // PharLap only can do net stuff with the RT version.
#   if defined (ACE_HAS_PHARLAP_RT)
  // @@This is not at all reliable... requires ethernet and BOOTP to be used.
  // A more reliable way is to go thru the devices w/ EtsTCPGetDeviceCfg until
  // a legit IP address is found, then get its name w/ gethostbyaddr.
  ACE_SOCKCALL_RETURN (gethostname (name, maxnamelen), int, SOCKET_ERROR);
#   else
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (maxnamelen);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_PHARLAP_RT */
#elif defined (ACE_VXWORKS) || defined (ACE_HAS_WINCE)
  OSCALL_RETURN (::gethostname (name, maxnamelen), int, -1);
#elif defined (ACE_WIN32)
  if (::gethostname (name, /*ACE_Utils::truncate_cast<int>*/int (maxnamelen)) == 0)
  {
    return 0;
  }
  else
  {
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::GetComputerNameA (name,
                                            LPDWORD (&maxnamelen)),
                                            ace_result_), int, -1);
  }
#else /* ACE_HAS_PHARLAP */
  ACE_utsname host_info;

  if (OS::uname (&host_info) == -1)
    return -1;
  else
    {
      OS::strsncpy (name, host_info.nodename, maxnamelen);
      return 0;
    }
#endif /* ACE_HAS_PHARLAP */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::hostname (wchar_t name[], size_t maxnamelen)
{
#if defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (GetComputerNameW (name,
                                                        LPDWORD (&maxnamelen)),
                                          ace_result_), int, -1);
#else /* ACE_WIN32 && !ACE_HAS_WINCE */
  // Emulate using the char version
  char *char_name = 0;
  int result = 0;

  ACE_NEW_RETURN (char_name, char[maxnamelen], -1);

  result = OS::hostname(char_name, maxnamelen);
  OS::strcpy (name, ACE_Ascii_To_Wide (char_name).wchar_rep ());

  delete [] char_name;
  return result;
#endif /* ACE_WIN32 && !ACE_HAS_WINCE */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
OS::isatty (int handle)
{
  OS_TRACE ("OS::isatty");
#if defined (ACE_LACKS_ISATTY)
  ACE_UNUSED_ARG (handle);
  return 0;
# elif defined (ACE_WIN32)
  return ::_isatty (handle);
# else
  OSCALL_RETURN (::isatty (handle), int, -1);
# endif /* ACE_LACKS_ISATTY */
}

#if defined (ACE_WIN32)
ACE_INLINE int
OS::isatty (ACE_HANDLE handle)
{
#if defined (ACE_LACKS_ISATTY)
  ACE_UNUSED_ARG (handle);
  return 0;
#else
  int fd = ::_open_osfhandle (intptr_t (handle), 0);
  int status = ::_isatty (fd);
  ::_close (fd);
  return status;
#endif /* ACE_LACKS_ISATTY */
}

#endif /* ACE_WIN32 */

ACE_INLINE ACE_OFF_T
OS::lseek (ACE_HANDLE handle, ACE_OFF_T offset, int whence)
{
  OS_TRACE ("OS::lseek");
#if defined (ACE_WIN32)
# if SEEK_SET != FILE_BEGIN || SEEK_CUR != FILE_CURRENT || SEEK_END != FILE_END
  //#error Windows NT is evil AND rude!
  switch (whence)
    {
    case SEEK_SET:
      whence = FILE_BEGIN;
      break;
    case SEEK_CUR:
      whence = FILE_CURRENT;
      break;
    case SEEK_END:
      whence = FILE_END;
      break;
    default:
      errno = EINVAL;
      return static_cast<ACE_OFF_T> (-1); // rather safe than sorry
    }
# endif  /* SEEK_SET != FILE_BEGIN || SEEK_CUR != FILE_CURRENT || SEEK_END != FILE_END */
  LONG low_offset = ACE_LOW_PART(offset);
  LONG high_offset = ACE_HIGH_PART(offset);
  DWORD const result =
    ::SetFilePointer (handle, low_offset, &high_offset, whence);
  if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    ACE_FAIL_RETURN (static_cast<ACE_OFF_T> (-1));
  else
    return result;
#else
  OSCALL_RETURN (::lseek (handle, offset, whence), ACE_OFF_T, -1);
#endif /* ACE_WIN32 */
}

#if defined (ACE_HAS_LLSEEK) || defined (ACE_HAS_LSEEK64)
ACE_INLINE ACE_LOFF_T
OS::llseek (ACE_HANDLE handle, ACE_LOFF_T offset, int whence)
{
  OS_TRACE ("OS::llseek");

#if ACE_SIZEOF_LONG == 8
  /* The native lseek is 64 bit.  Use it. */
  return OS::lseek (handle, offset, whence);
#elif defined (ACE_HAS_LLSEEK) && defined (ACE_HAS_LSEEK64)
# error Either ACE_HAS_LSEEK64 and ACE_HAS_LLSEEK should be defined, not both!
#elif defined (ACE_HAS_LSEEK64)
  OSCALL_RETURN (::lseek64 (handle, offset, whence), ACE_LOFF_T, -1);
#elif defined (ACE_HAS_LLSEEK)
# if defined (ACE_WIN32)
#  ifndef ACE_LACKS_WIN32_SETFILEPOINTEREX
  LARGE_INTEGER distance, new_file_pointer;

  distance.QuadPart = offset;

  return
    (::SetFilePointerEx (handle, distance, &new_file_pointer, whence)
     ? new_file_pointer.QuadPart
     : static_cast<ACE_LOFF_T> (-1));
#  else
  LARGE_INTEGER l_offset;
  l_offset.QuadPart = offset;
  LONG low_offset = l_offset.LowPart;
  LONG high_offset = l_offset.HighPart;

  l_offset.LowPart = ::SetFilePointer (handle,
                                       low_offset,
                                       &high_offset,
                                       whence);
  if (l_offset.LowPart == INVALID_SET_FILE_POINTER &&
      GetLastError () != NO_ERROR)
    return static_cast<ACE_LOFF_T> (-1);
  l_offset.HighPart = high_offset;
  return l_offset.QuadPart;
#  endif  /* ACE_LACKS_WIN32_SETFILEPOINTEREX */
# else
    OSCALL_RETURN (::llseek (handle, offset, whence), ACE_LOFF_T, -1);
# endif /* WIN32 */
#endif
}
#endif /* ACE_HAS_LLSEEK || ACE_HAS_LSEEK64 */

ACE_INLINE ssize_t
OS::read (ACE_HANDLE handle, void *buf, size_t len)
{
  OS_TRACE ("OS::read");
#if defined (ACE_WIN32)
  DWORD ok_len;
  if (::ReadFile (handle, buf, static_cast<DWORD> (len), &ok_len, 0))
    return (ssize_t) ok_len;
  else
    ACE_FAIL_RETURN (-1);
#else

  ssize_t result;

# if defined (ACE_HAS_CHARPTR_SOCKOPT)
  OSCALL (::read (handle, static_cast <char *> (buf), len), ssize_t, -1, result);
# else
  OSCALL (::read (handle, buf, len), ssize_t, -1, result);
# endif /* ACE_HAS_CHARPTR_SOCKOPT */

# if !(defined (EAGAIN) && defined (EWOULDBLOCK) && EAGAIN == EWOULDBLOCK)
  // Optimize this code out if we can detect that EAGAIN ==
  // EWOULDBLOCK at compile time.  If we cannot detect equality at
  // compile-time (e.g. if EAGAIN or EWOULDBLOCK are not preprocessor
  // macros) perform the check at run-time.  The goal is to avoid two
  // TSS accesses in the _REENTRANT case when EAGAIN == EWOULDBLOCK.
  if (result == -1
#  if !defined (EAGAIN) || !defined (EWOULDBLOCK)
      && EAGAIN != EWOULDBLOCK
#  endif  /* !EAGAIN || !EWOULDBLOCK */
      && errno == EAGAIN)
    {
      errno = EWOULDBLOCK;
    }
# endif /* EAGAIN != EWOULDBLOCK*/

  return result;
#endif /* ACE_WIN32 */
}

ACE_INLINE ssize_t
OS::read (ACE_HANDLE handle, void *buf, size_t len,
              ACE_OVERLAPPED *overlapped)
{
  OS_TRACE ("OS::read");
  overlapped = overlapped;
#if defined (ACE_WIN32)
  DWORD ok_len;
  DWORD short_len = static_cast<DWORD> (len);
  if (::ReadFile (handle, buf, short_len, &ok_len, overlapped))
    return (ssize_t) ok_len;
  else
    ACE_FAIL_RETURN (-1);
#else
  return OS::read (handle, buf, len);
#endif /* ACE_WIN32 */
}

ACE_INLINE ssize_t
OS::readlink (const char *path, char *buf, size_t bufsiz)
{
  OS_TRACE ("OS::readlink");
# if defined (ACE_LACKS_READLINK)
  ACE_UNUSED_ARG (path);
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (bufsiz);
  ACE_NOTSUP_RETURN (-1);
# else
#   if !defined(ACE_HAS_NONCONST_READLINK)
      OSCALL_RETURN (::readlink (path, buf, bufsiz), ssize_t, -1);
#   else
      OSCALL_RETURN (
        ::readlink (const_cast <char *>(path), buf, bufsiz), ssize_t, -1);
#   endif
# endif /* ACE_LACKS_READLINK */
}

ACE_INLINE int
OS::pipe (ACE_HANDLE fds[])
{
  OS_TRACE ("OS::pipe");
# if defined (ACE_LACKS_PIPE)
  ACE_UNUSED_ARG (fds);
  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL
                        (::CreatePipe (&fds[0], &fds[1], 0, 0),
                         ace_result_), int, -1);
# else
  OSCALL_RETURN (::pipe (fds), int, -1);
# endif /* ACE_LACKS_PIPE */
}

ACE_INLINE void *
OS::sbrk (intptr_t brk)
{
#if defined (ACE_LACKS_SBRK)
  ACE_UNUSED_ARG (brk);
  ACE_NOTSUP_RETURN (0);
#else
  OSCALL_RETURN (::sbrk (brk), void *, 0);
#endif /* ACE_LACKS_SBRK */
}

ACE_INLINE int
OS::setgid (gid_t gid)
{
  OS_TRACE ("OS::setgid");
#if defined (ACE_LACKS_SETGID)
  ACE_UNUSED_ARG (gid);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::setgid (gid), int,  -1);
# endif /* ACE_LACKS_SETGID */
}

ACE_INLINE int
OS::setegid (gid_t gid)
{
  OS_TRACE ("OS::setegid");
#if defined (ACE_LACKS_SETEGID)
  ACE_UNUSED_ARG (gid);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::setegid (gid), int,  -1);
# endif /* ACE_LACKS_SETEGID */
}

ACE_INLINE int
OS::setpgid (pid_t pid, pid_t pgid)
{
  OS_TRACE ("OS::setpgid");
#if defined (ACE_LACKS_SETPGID)
  ACE_UNUSED_ARG (pid);
  ACE_UNUSED_ARG (pgid);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::setpgid (pid, pgid), int, -1);
#endif /* ACE_LACKS_SETPGID */
}

ACE_INLINE int
OS::setregid (gid_t rgid, gid_t egid)
{
  OS_TRACE ("OS::setregid");
#if defined (ACE_LACKS_SETREGID)
  ACE_UNUSED_ARG (rgid);
  ACE_UNUSED_ARG (egid);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::setregid (rgid, egid), int, -1);
#endif /* ACE_LACKS_SETREGID */
}

ACE_INLINE int
OS::setreuid (uid_t ruid, uid_t euid)
{
  OS_TRACE ("OS::setreuid");
#if defined (ACE_LACKS_SETREUID)
  ACE_UNUSED_ARG (ruid);
  ACE_UNUSED_ARG (euid);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::setreuid (ruid, euid), int, -1);
#endif /* ACE_LACKS_SETREUID */
}

ACE_INLINE pid_t
OS::setsid (void)
{
  OS_TRACE ("OS::setsid");
#if defined (ACE_LACKS_SETSID)
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::setsid (), int, -1);
# endif /* ACE_LACKS_SETSID */
}

ACE_INLINE int
OS::setuid (uid_t uid)
{
  OS_TRACE ("OS::setuid");
#if defined (ACE_LACKS_SETUID)
  ACE_UNUSED_ARG (uid);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::setuid (uid), int,  -1);
# endif /* ACE_LACKS_SETUID */
}

ACE_INLINE int
OS::seteuid (uid_t uid)
{
  OS_TRACE ("OS::seteuid");
#if defined (ACE_LACKS_SETEUID)
  ACE_UNUSED_ARG (uid);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::seteuid (uid), int,  -1);
# endif /* ACE_LACKS_SETEUID */
}

ACE_INLINE int
OS::sleep (u_int seconds)
{
  OS_TRACE ("OS::sleep");
#if defined (ACE_WIN32)
  ::Sleep (seconds * ACE_ONE_SECOND_IN_MSECS);
  return 0;
#elif defined (ACE_HAS_CLOCK_GETTIME)
  struct timespec rqtp;
  // Initializer doesn't work with Green Hills 1.8.7
  rqtp.tv_sec = seconds;
  rqtp.tv_nsec = 0L;
  OSCALL_RETURN (::nanosleep (&rqtp, 0), int, -1);
#else
  OSCALL_RETURN (::sleep (seconds), int, -1);
#endif /* ACE_WIN32 */
}

ACE_INLINE int
OS::sleep (const TimeValue &tv)
{
  OS_TRACE ("OS::sleep");
#if defined (ACE_WIN32)
  ::Sleep (tv.msec ());
  return 0;
#elif defined (ACE_HAS_CLOCK_GETTIME)
  timespec_t rqtp = tv;
  OSCALL_RETURN (::nanosleep (&rqtp, 0), int, -1);
#else
# if defined (ACE_HAS_NONCONST_SELECT_TIMEVAL)
  // Copy the timeval, because this platform doesn't declare the timeval
  // as a pointer to const.
  timeval tv_copy = tv;
#  if defined(ACE_TANDEM_T1248_PTHREADS)
     OSCALL_RETURN (::spt_select (0, 0, 0, 0, &tv_copy), int, -1);
#  else
     OSCALL_RETURN (::select (0, 0, 0, 0, &tv_copy), int, -1);
#  endif
# else  /* ! ACE_HAS_NONCONST_SELECT_TIMEVAL */
  const timeval *tvp = tv;
  OSCALL_RETURN (::select (0, 0, 0, 0, tvp), int, -1);
# endif /* ACE_HAS_NONCONST_SELECT_TIMEVAL */
#endif /* ACE_WIN32 */
}

ACE_INLINE void
OS::swab (const void *src,
              void *dest,
              ssize_t length)
{
#if defined (ACE_LACKS_SWAB)
  const char *from = static_cast<const char*> (src);
  char *to = static_cast<char *> (dest);
  ssize_t ptr = 0;
  for (ptr = 1; ptr < length; ptr += 2)
    {
      char p = from[ptr];
      char q = from[ptr-1];
      to[ptr-1] = p;
      to[ptr  ] = q;
    }
  if (ptr == length) /* I.e., if length is odd, */
    to[ptr-1] = 0;   /* then pad with a NUL. */
#elif defined (ACE_HAS_NONCONST_SWAB)
  const char *tmp = static_cast<const char*> (src);
  char *from = const_cast<char *> (tmp);
  char *to = static_cast<char *> (dest);
  ::swab (from, to, length);
#elif defined (ACE_HAS_CONST_CHAR_SWAB)
  const char *from = static_cast<const char*> (src);
  char *to = static_cast<char *> (dest);
  ::swab (from, to, length);
#else
  ::swab (src, dest, length);
#endif /* ACE_LACKS_SWAB */

}

ACE_INLINE long
OS::sysconf (int name)
{
  OS_TRACE ("OS::sysconf");
#if defined (ACE_LACKS_SYSCONF)
  ACE_UNUSED_ARG (name);
  ACE_NOTSUP_RETURN (-1);
#else
  OSCALL_RETURN (::sysconf (name), long, -1);
#endif /* ACE_LACKS_SYSCONF */
}

ACE_INLINE long
OS::sysinfo (int cmd, char *buf, long count)
{
  OS_TRACE ("OS::sysinfo");
#if defined (ACE_HAS_SYSINFO)
  OSCALL_RETURN (::sysinfo (cmd, buf, count), long, -1);
#else
  ACE_UNUSED_ARG (cmd);
  ACE_UNUSED_ARG (buf);
  ACE_UNUSED_ARG (count);

  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_SYSINFO */
}

ACE_INLINE int
OS::truncate (const ACE_TCHAR *filename,
                  ACE_OFF_T offset)
{
  OS_TRACE ("OS::truncate");
#if defined (ACE_WIN32)
  ACE_HANDLE handle = OS::open (filename,
                                    O_WRONLY,
                                    ACE_DEFAULT_FILE_PERMS);

#  if !defined (ACE_LACKS_WIN32_SETFILEPOINTEREX)
  LARGE_INTEGER loffset;
  loffset.QuadPart = offset;
#else
  LONG low_offset = ACE_LOW_PART(offset);
  LONG high_offset = ACE_HIGH_PART(offset);
#endif

  if (handle == ACE_INVALID_HANDLE)
    ACE_FAIL_RETURN (-1);

#  if !defined (ACE_LACKS_WIN32_SETFILEPOINTEREX)
  else if (::SetFilePointerEx (handle,
                               loffset,
                               0,
                               FILE_BEGIN))
#  else
  else if (::SetFilePointer (handle,
                             low_offset,
                             &high_offset,
                             FILE_BEGIN) != INVALID_SET_FILE_POINTER
           || GetLastError () == NO_ERROR)
#  endif /* ACE_LACKS_WIN32_SETFILEPOINTEREX */
    {
      BOOL result = ::SetEndOfFile (handle);
      ::CloseHandle (handle);
      ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (result, ace_result_), int, -1);
    }
  else
    {
      ::CloseHandle (handle);
      ACE_FAIL_RETURN (-1);
    }
  /* NOTREACHED */
#elif !defined (ACE_LACKS_TRUNCATE)
  OSCALL_RETURN
    (::truncate (ACE_TEXT_ALWAYS_CHAR (filename), offset), int, -1);
#else
  ACE_UNUSED_ARG (filename);
  ACE_UNUSED_ARG (offset);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

ACE_INLINE useconds_t
OS::ualarm (useconds_t usecs, useconds_t interval)
{
  OS_TRACE ("OS::ualarm");

#if defined (ACE_HAS_UALARM)
  return ::ualarm (usecs, interval);
#elif !defined (ACE_LACKS_UNIX_SIGNALS)
  ACE_UNUSED_ARG (interval);
  return ::alarm (usecs * ACE_ONE_SECOND_IN_USECS);
#else
  ACE_UNUSED_ARG (usecs);
  ACE_UNUSED_ARG (interval);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_UALARM */
}

ACE_INLINE useconds_t
OS::ualarm (const TimeValue &tv,
                const TimeValue &tv_interval)
{
  OS_TRACE ("OS::ualarm");

#if defined (ACE_HAS_UALARM)
  useconds_t usecs = (tv.sec () * ACE_ONE_SECOND_IN_USECS) + tv.usec ();
  useconds_t interval =
    (tv_interval.sec () * ACE_ONE_SECOND_IN_USECS) + tv_interval.usec ();
  return ::ualarm (usecs, interval);
#elif !defined (ACE_LACKS_UNIX_SIGNALS)
  ACE_UNUSED_ARG (tv_interval);
  return ::alarm (tv.sec ());
#else
  ACE_UNUSED_ARG (tv_interval);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_UALARM */
}

ACE_INLINE int
OS::unlink (const char *path)
{
  OS_TRACE ("OS::unlink");
# if defined (ACE_HAS_NONCONST_UNLINK)
  OSCALL_RETURN (::unlink (const_cast<char *> (path)), int, -1);
# elif defined (ACE_HAS_WINCE)
  // @@ The problem is, DeleteFile is not actually equals to unlink. ;(
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::DeleteFile (ACE_TEXT_CHAR_TO_TCHAR (path)), ace_result_),
                        int, -1);
# elif defined (ACE_LACKS_UNLINK)
  ACE_UNUSED_ARG (path);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::unlink (path), int, -1);
# endif /* ACE_HAS_NONCONST_UNLINK */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::unlink (const wchar_t *path)
{
  OS_TRACE ("OS::unlink");
# if defined (ACE_HAS_WINCE)
  // @@ The problem is, DeleteFile is not actually equals to unlink. ;(
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::DeleteFileW (path), ace_result_),
                        int, -1);
# elif defined (ACE_WIN32)
  OSCALL_RETURN (::_wunlink (path), int, -1);
# else
  ACE_Wide_To_Ascii npath (path);
  return OS::unlink (npath.char_rep ());
# endif /* ACE_HAS_WINCE */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE ssize_t
OS::write (ACE_HANDLE handle, const void *buf, size_t nbyte)
{
  OS_TRACE ("OS::write");
#if defined (ACE_WIN32)
  DWORD bytes_written; // This is set to 0 byte WriteFile.

  // Strictly correctly, we should loop writing all the data if more
  // than a DWORD length can hold.
  DWORD short_nbyte = static_cast<DWORD> (nbyte);
  if (::WriteFile (handle, buf, short_nbyte, &bytes_written, 0))
    return (ssize_t) bytes_written;
  else
    ACE_FAIL_RETURN (-1);
#else
# if defined (ACE_HAS_CHARPTR_SOCKOPT)
  OSCALL_RETURN (::write (handle, static_cast <char *> (const_cast <void *> (buf)), nbyte), ssize_t, -1);
# else
  OSCALL_RETURN (::write (handle, buf, nbyte), ssize_t, -1);
# endif /* ACE_HAS_CHARPTR_SOCKOPT */
#endif /* ACE_WIN32 */
}

ACE_INLINE ssize_t
OS::write (ACE_HANDLE handle,
               const void *buf,
               size_t nbyte,
               ACE_OVERLAPPED *overlapped)
{
  OS_TRACE ("OS::write");
  overlapped = overlapped;
#if defined (ACE_WIN32)
  DWORD bytes_written; // This is set to 0 byte WriteFile.

  DWORD short_nbyte = static_cast<DWORD> (nbyte);
  if (::WriteFile (handle, buf, short_nbyte, &bytes_written, overlapped))
    return (ssize_t) bytes_written;
  else
    ACE_FAIL_RETURN (-1);
#else
  return OS::write (handle, buf, nbyte);
#endif /* ACE_WIN32 */
}

int
OS::argv_to_string (ACE_TCHAR **argv,
                        ACE_TCHAR *&buf,
                        bool substitute_env_args,
                        bool quote_args)
{
  if (argv == 0 || argv[0] == 0)
    return 0;

  size_t buf_len = 0;

  // Determine the length of the buffer.

  int argc;
  for (argc = 0; argv[argc] != 0; ++argc)
    continue;
  ACE_TCHAR **argv_p = argv;

  for (int i = 0; i < argc; ++i)
    {
#if !defined (ACE_LACKS_ENV)
      // Account for environment variables.
      if (substitute_env_args
          && OS::strchr (argv[i], ACE_TEXT ('$')) != 0)
        {
          if (argv_p == argv)
            {
              argv_p = (ACE_TCHAR **) OS::malloc (argc * sizeof (ACE_TCHAR *));
              if (argv_p == 0)
                {
                  errno = ENOMEM;
                  return 0;
                }
              OS::memcpy (argv_p, argv, argc * sizeof (ACE_TCHAR *));
            }
          argv_p[i] = OS::strenvdup (argv[i]);
          if (argv_p[i] == 0)
            {
              OS::free (argv_p);
              errno = ENOMEM;
              return 0;
            }
        }
#endif /* ACE_LACKS_ENV */
      if (quote_args
          && OS::strchr (argv_p[i], ACE_TEXT (' ')) != 0)
        {
          if (argv_p == argv)
            {
              argv_p = (ACE_TCHAR **) OS::malloc (argc * sizeof (ACE_TCHAR *));
              if (argv_p == 0)
                {
                  errno = ENOMEM;
                  return 0;
                }
              OS::memcpy (argv_p, argv, argc * sizeof (ACE_TCHAR *));
            }
          int quotes = 0;
          ACE_TCHAR *temp = argv_p[i];
          if (OS::strchr (temp, ACE_TEXT ('"')) != 0)
            {
              for (int j = 0; temp[j] != 0; ++j)
                if (temp[j] == ACE_TEXT ('"'))
                  ++quotes;
            }
          argv_p[i] =
            (ACE_TCHAR *) OS::malloc (OS::strlen (temp) * sizeof (ACE_TCHAR) + quotes + 3);
          if (argv_p[i] == 0)
            {
              OS::free (argv_p);
              errno = ENOMEM;
              return 0;
            }
          ACE_TCHAR *end = argv_p[i];

          *end++ = ACE_TEXT ('"');

          if (quotes > 0)
            {
              for (ACE_TCHAR *p = temp;
                   *p != 0;
                   *end++ = *p++)
                if (*p == ACE_TEXT ('"'))
                  *end++ = ACE_TEXT ('\\');

              *end++ = ACE_TEXT ('\0');
            }
          else
            end = OS::strecpy (end, temp);

          end[-1] = ACE_TEXT ('"');

          *end = ACE_TEXT ('\0');
          if (temp != argv[i])
            OS::free (temp);
        }
      buf_len += OS::strlen (argv_p[i]);

      // Add one for the extra space between each string.
      buf_len++;
    }

  // Step through all argv params and copy each one into buf; separate
  // each param with white space.

  ACE_NEW_RETURN (buf,
                  ACE_TCHAR[buf_len + 1],
                  0);

  // Initial null charater to make it a null string.
  buf[0] = ACE_TEXT ('\0');
  ACE_TCHAR *end = buf;

  for (int i = 0; i < argc; ++i)
    {
      end = OS::strecpy (end, argv_p[i]);
      if (argv_p[i] != argv[i])
        OS::free (argv_p[i]);

      // Replace the null char that strecpy put there with white
      // space.
      end[-1] = ACE_TEXT (' ');
    }
  // Null terminate the string.
  *end = ACE_TEXT ('\0');

  if (argv_p != argv)
    OS::free (argv_p);

  // The number of arguments.
  return argc;
}

int
OS::execl (const char * /* path */, const char * /* arg0 */, ...)
{
  OS_TRACE ("OS::execl");
  ACE_NOTSUP_RETURN (-1);
  // Need to write this code.
  // OSCALL_RETURN (::execv (path, argv), int, -1);
}

int
OS::execle (const char * /* path */, const char * /* arg0 */, ...)
{
  OS_TRACE ("OS::execle");
  ACE_NOTSUP_RETURN (-1);
  // Need to write this code.
  //  OSCALL_RETURN (::execve (path, argv, envp), int, -1);
}

int
OS::execlp (const char * /* file */, const char * /* arg0 */, ...)
{
  OS_TRACE ("OS::execlp");
  ACE_NOTSUP_RETURN (-1);
  // Need to write this code.
  //  OSCALL_RETURN (::execvp (file, argv), int, -1);
}

pid_t
OS::fork (const ACE_TCHAR *program_name)
{
  OS_TRACE ("OS::fork");
# if defined (ACE_LACKS_FORK)
  ACE_UNUSED_ARG (program_name);
  ACE_NOTSUP_RETURN (pid_t (-1));
# else
  pid_t const pid =
# if defined (ACE_HAS_STHREADS)
    ::fork1 ();
#else
    ::fork ();
#endif /* ACE_HAS_STHREADS */

#if !defined (ACE_HAS_MINIMAL_OS)
  //if (pid == 0)
    //ACE_Base_Thread_Adapter::sync_log_msg (program_name);
#endif /* ! ACE_HAS_MINIMAL_OS */

  return pid;
# endif /* ACE_WIN32 */
}

// Create a contiguous command-line argument buffer with each arg
// separated by spaces.

pid_t
OS::fork_exec (ACE_TCHAR *argv[])
{
# if defined (ACE_WIN32)

  ACE_TCHAR *buf = 0;
  ACE_Auto_Basic_Array_Ptr<ACE_TCHAR> safe_ptr (buf);
  if (OS::argv_to_string (argv, buf) != -1)
    {
      PROCESS_INFORMATION process_info;
#   if !defined (ACE_HAS_WINCE)
      ACE_TEXT_STARTUPINFO startup_info;
      OS::memset ((void *) &startup_info,
                      0,
                      sizeof startup_info);
      startup_info.cb = sizeof startup_info;

      if (ACE_TEXT_CreateProcess (0,
                                  buf,
                                  0, // No process attributes.
                                  0,  // No thread attributes.
                                  TRUE, // Allow handle inheritance.
                                  0, // Don't create a new console window.
                                  0, // No environment.
                                  0, // No current directory.
                                  &startup_info,
                                  &process_info))
#   else
      if (ACE_TEXT_CreateProcess (0,
                                  buf,
                                  0, // No process attributes.
                                  0,  // No thread attributes.
                                  FALSE, // Can's inherit handles on CE
                                  0, // Don't create a new console window.
                                  0, // No environment.
                                  0, // No current directory.
                                  0, // Can't use startup info on CE
                                  &process_info))
#   endif /* ! ACE_HAS_WINCE */
        {
          // Free resources allocated in kernel.
          OS::close (process_info.hThread);
          OS::close (process_info.hProcess);
          // Return new process id.
          return process_info.dwProcessId;
        }
    }

  // CreateProcess failed.
  return -1;
# else
      pid_t const result = OS::fork ();

#   if defined (ACE_USES_WCHAR)
      // Wide-char builds need to convert the command-line args to
      // narrow char strings for execv ().
      char **cargv = 0;
      int arg_count;
#   endif /* ACE_HAS_WCHAR */

      switch (result)
        {
        case -1:
          // Error.
          return -1;
        case 0:
          // Child process.
#   if defined (ACE_USES_WCHAR)
          for (arg_count = 0; argv[arg_count] != 0; ++arg_count)
            ;
          ++arg_count;    // Need a 0-pointer end-of-array marker
          ACE_NEW_NORETURN (cargv, char*[arg_count]);
          if (cargv == 0)
            OS::exit (errno);
          --arg_count;    // Back to 0-indexed
          cargv[arg_count] = 0;
          while (--arg_count >= 0)
            cargv[arg_count] = ACE_Wide_To_Ascii::convert (argv[arg_count]);
          // Don't worry about freeing the cargv or the strings it points to.
          // Either the process will be replaced, or we'll exit.
          if (OS::execv (cargv[0], cargv) == -1)
            OS::exit (errno);
#   else
          if (OS::execv (argv[0], argv) == -1)
            {
              // The OS layer should not print stuff out
              // ACE_ERROR ((LM_ERROR,
              //             "%p Exec failed\n"));

              // If the execv fails, this child needs to exit.
              OS::exit (errno);
            }
#   endif /* ACE_HAS_WCHAR */

        default:
          // Server process.  The fork succeeded.
          return result;
        }
# endif /* ACE_WIN32 */
}

long
OS::num_processors (void)
{
  OS_TRACE ("OS::num_processors");

#if defined (ACE_HAS_PHARLAP)
  return 1;
#elif defined (ACE_WIN32)
  SYSTEM_INFO sys_info;
  ::GetSystemInfo (&sys_info);
  return sys_info.dwNumberOfProcessors;
#elif defined (_SC_NPROCESSORS_CONF)
  return ::sysconf (_SC_NPROCESSORS_CONF);
#elif defined (ACE_HAS_SYSCTL)
  int num_processors;
  int mib[2] = { CTL_HW, HW_NCPU };
  size_t len = sizeof (num_processors);
  if (::sysctl (mib, 2, &num_processors, &len, 0, 0) != -1)
    return num_processors;
  else
    return -1;
#elif defined (__hpux)
  struct pst_dynamic psd;
  if (::pstat_getdynamic (&psd, sizeof (psd), (size_t) 1, 0) != -1)
    return psd.psd_max_proc_cnt;
  else
    return -1;
#else
  ACE_NOTSUP_RETURN (-1);
#endif
}

long
OS::num_processors_online (void)
{
  OS_TRACE ("OS::num_processors_online");

#if defined (ACE_HAS_PHARLAP)
  return 1;
#elif defined (ACE_WIN32)
  SYSTEM_INFO sys_info;
  ::GetSystemInfo (&sys_info);
  return sys_info.dwNumberOfProcessors;
#elif defined (_SC_NPROCESSORS_ONLN)
  return ::sysconf (_SC_NPROCESSORS_ONLN);
#elif defined (ACE_HAS_SYSCTL)
  int num_processors;
  int mib[2] = { CTL_HW, HW_NCPU };
  size_t len = sizeof (num_processors);
  if (::sysctl (mib, 2, &num_processors, &len, 0, 0) != -1)
    return num_processors;
  else
    return -1;
#elif defined (__hpux)
  struct pst_dynamic psd;
  if (::pstat_getdynamic (&psd, sizeof (psd), (size_t) 1, 0) != -1)
    return psd.psd_proc_cnt;
  else
    return -1;
#else
  ACE_NOTSUP_RETURN (-1);
#endif
}

ssize_t
OS::read_n (ACE_HANDLE handle,
                void *buf,
                size_t len,
                size_t *bt)
{
  size_t temp;
  size_t &bytes_transferred = bt == 0 ? temp : *bt;
  ssize_t n = 0;

  for (bytes_transferred = 0;
       bytes_transferred < len;
       bytes_transferred += n)
    {
      n = OS::read (handle,
                        (char *) buf + bytes_transferred,
                        len - bytes_transferred);

      if (n == -1 || n == 0)
        return n;
    }

  return bytes_transferred;
}
/*
ssize_t
OS::pread (ACE_HANDLE handle,
               void *buf,
               size_t nbytes,
               ACE_OFF_T offset)
{
# if defined (ACE_HAS_P_READ_WRITE)
#   if defined (ACE_WIN32)

  OS_GUARD

  // Remember the original file pointer position
  LONG original_high_position = 0;
  DWORD original_low_position = ::SetFilePointer (handle,
                                                  0,
                                                  &original_high_position,
                                                  FILE_CURRENT);

  if (original_low_position == INVALID_SET_FILE_POINTER
      && GetLastError () != NO_ERROR)
    return -1;

  // Go to the correct position
  LONG low_offset = ACE_LOW_PART (offset);
  LONG high_offset = ACE_HIGH_PART (offset);
  DWORD altered_position = ::SetFilePointer (handle,
                                             low_offset,
                                             &high_offset,
                                             FILE_BEGIN);
  if (altered_position == INVALID_SET_FILE_POINTER
      && GetLastError () != NO_ERROR)
    return -1;

  DWORD bytes_read;

#     if defined (ACE_HAS_WIN32_OVERLAPPED_IO)

  OVERLAPPED overlapped;
  overlapped.Internal = 0;
  overlapped.InternalHigh = 0;
  overlapped.Offset = low_offset;
  overlapped.OffsetHigh = high_offset;
  overlapped.hEvent = 0;

  BOOL result = ::ReadFile (handle,
                            buf,
                            static_cast <DWORD> (nbytes),
                            &bytes_read,
                            &overlapped);

  if (result == FALSE)
    {
      if (::GetLastError () != ERROR_IO_PENDING)
        return -1;

      else
        {
          result = ::GetOverlappedResult (handle,
                                          &overlapped,
                                          &bytes_read,
                                          TRUE);
          if (result == FALSE)
            return -1;
        }
    }

#     else 

  BOOL result = ::ReadFile (handle,
                            buf,
                            nbytes,
                            &bytes_read,
                            0);
  if (result == FALSE)
    return -1;

#     endif

  // Reset the original file pointer position
  if (::SetFilePointer (handle,
                        original_low_position,
                        &original_high_position,
                        FILE_BEGIN) == INVALID_SET_FILE_POINTER
      && GetLastError () != NO_ERROR)
    return -1;

  return (ssize_t) bytes_read;

#   else 

  return ::pread (handle, buf, nbytes, offset);

#   endif 

# else 

  OS_GUARD

  // Remember the original file pointer position
  ACE_OFF_T original_position = OS::lseek (handle,
                                               0,
                                               SEEK_CUR);

  if (original_position == -1)
    return -1;

  // Go to the correct position
  ACE_OFF_T altered_position = OS::lseek (handle, offset, SEEK_SET);

  if (altered_position == -1)
    return -1;

  ssize_t const bytes_read = OS::read (handle, buf, nbytes);

  if (bytes_read == -1)
    return -1;

  if (OS::lseek (handle,
                     original_position,
                     SEEK_SET) == -1)
    return -1;

  return bytes_read;

# endif 
}

ssize_t
OS::pwrite (ACE_HANDLE handle,
                const void *buf,
                size_t nbytes,
                ACE_OFF_T offset)
{
# if defined (ACE_HAS_P_READ_WRITE)
#   if defined (ACE_WIN32)

  OS_GUARD

  // Remember the original file pointer position
  LARGE_INTEGER orig_position;
  orig_position.QuadPart = 0;
  orig_position.LowPart = ::SetFilePointer (handle,
                                            0,
                                            &orig_position.HighPart,
                                            FILE_CURRENT);
  if (orig_position.LowPart == INVALID_SET_FILE_POINTER
      && GetLastError () != NO_ERROR)
    return -1;

  DWORD bytes_written;
  LARGE_INTEGER loffset;
  loffset.QuadPart = offset;

#     if defined (ACE_HAS_WIN32_OVERLAPPED_IO)

  OVERLAPPED overlapped;
  overlapped.Internal = 0;
  overlapped.InternalHigh = 0;
  overlapped.Offset = loffset.LowPart;
  overlapped.OffsetHigh = loffset.HighPart;
  overlapped.hEvent = 0;

  BOOL result = ::WriteFile (handle,
                             buf,
                             static_cast <DWORD> (nbytes),
                             &bytes_written,
                             &overlapped);

  if (result == FALSE)
    {
      if (::GetLastError () != ERROR_IO_PENDING)
        return -1;

      result = ::GetOverlappedResult (handle,
                                      &overlapped,
                                      &bytes_written,
                                      TRUE);
      if (result == FALSE)
        return -1;
    }

#     else 

  // Go to the correct position; if this is a Windows variant without
  // overlapped I/O, it probably doesn't have SetFilePointerEx either,
  // so manage this with SetFilePointer, changing calls based on the use
  // of 64 bit offsets.
  DWORD newpos;
#       if defined (_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
  newpos = ::SetFilePointer (handle,
                             loffset.LowPart,
                             &loffset.HighPart,
                             FILE_BEGIN);
#       else
  newpos = ::SetFilePointer (handle,
                             loffset.LowPart,
                             0,
                             FILE_BEGIN);
#       endif 
  if (newpos == 0xFFFFFFFF && ::GetLastError () != NO_ERROR)
    {
      OS::set_errno_to_last_error ();
      return -1;
    }

  BOOL result = ::WriteFile (handle,
                             buf,
                             nbytes,
                             &bytes_written,
                             0);
  if (result == FALSE)
    return -1;

#     endif 

  // Reset the original file pointer position
  if (::SetFilePointer (handle,
                        orig_position.LowPart,
                        &orig_position.HighPart,
                        FILE_BEGIN) == INVALID_SET_FILE_POINTER
      && GetLastError () != NO_ERROR)
    return -1;

  return (ssize_t) bytes_written;

#   else 

  return ::pwrite (handle, buf, nbytes, offset);
#   endif 
# else 

  OS_GUARD

  // Remember the original file pointer position
  ACE_OFF_T original_position = OS::lseek (handle,
                                               0,
                                               SEEK_CUR);
  if (original_position == -1)
    return -1;

  // Go to the correct position
  ACE_OFF_T altered_position = OS::lseek (handle,
                                              offset,
                                              SEEK_SET);
  if (altered_position == -1)
    return -1;

  ssize_t const bytes_written = OS::write (handle,
                                               buf,
                                               nbytes);
  if (bytes_written == -1)
    return -1;

  if (OS::lseek (handle,
                     original_position,
                     SEEK_SET) == -1)
    return -1;

  return bytes_written;
# endif 
}

int
OS::string_to_argv (ACE_TCHAR *buf,
                        int &argc,
                        ACE_TCHAR **&argv,
                        bool substitute_env_args)
{
  // Reset the number of arguments
  argc = 0;

  if (buf == 0)
    return -1;

  ACE_TCHAR *cp = buf;

  // First pass: count arguments.

  // '#' is the start-comment token..
  while (*cp != ACE_TEXT ('\0') && *cp != ACE_TEXT ('#'))
    {
      // Skip whitespace..
      while (OS::ace_isspace (*cp))
        ++cp;

      // Increment count and move to next whitespace..
      if (*cp != ACE_TEXT ('\0'))
        ++argc;

      while (*cp != ACE_TEXT ('\0') && !OS::ace_isspace (*cp))
        {
          // Grok quotes....
          if (*cp == ACE_TEXT ('\'') || *cp == ACE_TEXT ('"'))
            {
              ACE_TCHAR quote = *cp;

              // Scan past the string..
              for (++cp; *cp != ACE_TEXT ('\0')
                   && (*cp != quote || cp[-1] == ACE_TEXT ('\\')); ++cp)
                continue;

              // '\0' implies unmatched quote..
              if (*cp == ACE_TEXT ('\0'))
                {
                  --argc;
                  break;
                }
              else
                ++cp;
            }
          else
            ++cp;
        }
    }

  // Second pass: copy arguments.
  ACE_TCHAR arg[ACE_DEFAULT_ARGV_BUFSIZ];
  ACE_TCHAR *argp = arg;

  // Make sure that the buffer we're copying into is always large
  // enough.
  if (cp - buf >= ACE_DEFAULT_ARGV_BUFSIZ)
    ACE_NEW_RETURN (argp,
                    ACE_TCHAR[cp - buf + 1],
                    -1);

  // Make a new argv vector of argc + 1 elements.
  ACE_NEW_RETURN (argv,
                  ACE_TCHAR *[argc + 1],
                  -1);

  ACE_TCHAR *ptr = buf;

  for (int i = 0; i < argc; ++i)
    {
      // Skip whitespace..
      while (OS::ace_isspace (*ptr))
        ++ptr;

      // Copy next argument and move to next whitespace..
      cp = argp;
      while (*ptr != ACE_TEXT ('\0') && !OS::ace_isspace (*ptr))
        if (*ptr == ACE_TEXT ('\'') || *ptr == ACE_TEXT ('"'))
          {
            ACE_TCHAR quote = *ptr++;

            while (*ptr != ACE_TEXT ('\0')
                   && (*ptr != quote || ptr[-1] == ACE_TEXT ('\\')))
              {
                if (*ptr == quote && ptr[-1] == ACE_TEXT ('\\')) --cp;
                *cp++ = *ptr++;
              }

            if (*ptr == quote)
              ++ptr;
          }
        else
          *cp++ = *ptr++;

      *cp = ACE_TEXT ('\0');

#if !defined (ACE_LACKS_ENV)
      // Check for environment variable substitution here.
      if (substitute_env_args) {
          argv[i] = OS::strenvdup (argp);

          if (argv[i] == 0)
            {
              if (argp != arg)
                delete [] argp;
              errno = ENOMEM;
              return -1;
            }
      }
      else
#endif 
        {
          argv[i] = OS::strdup (argp);

          if (argv[i] == 0)
            {
              if (argp != arg)
                delete [] argp;
              errno = ENOMEM;
              return -1;
            }
        }
    }

  if (argp != arg)
    delete [] argp;

  argv[argc] = 0;
  return 0;
}
*/
// Write <len> bytes from <buf> to <handle> (uses the <write>
// system call on UNIX and the <WriteFile> call on Win32).

ssize_t
OS::write_n (ACE_HANDLE handle,
                 const void *buf,
                 size_t len,
                 size_t *bt)
{
  size_t temp;
  size_t &bytes_transferred = bt == 0 ? temp : *bt;
  ssize_t n;

  for (bytes_transferred = 0;
       bytes_transferred < len;
       bytes_transferred += n)
    {
      n = OS::write (handle,
                         (char *) buf + bytes_transferred,
                         len - bytes_transferred);

      if (n == -1 || n == 0)
        return n;
    }

  return bytes_transferred;
}


