// $Id: OS_NS_fcntl.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_fcntl.h"

ACE_RCSID(ace, OS_NS_fcntl, "$Id: OS_NS_fcntl.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

#include "os/OS_NS_errno.h"
#include "os/OS_NS_stdio.h"
#include "os/OS_NS_Thread.h"
#include "os/OS_NS_macros.h"


ACE_INLINE int
OS::fcntl (ACE_HANDLE handle, int cmd, long arg)
{
  OS_TRACE ("OS::fcntl");
# if defined (ACE_LACKS_FCNTL)
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (cmd);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
# else
  OSCALL_RETURN (::fcntl (handle, cmd, arg), int, -1);
# endif /* ACE_LACKS_FCNTL */
}

ACE_HANDLE
OS::open (const char *filename,
              int mode,
              mode_t perms,
              LPSECURITY_ATTRIBUTES sa)
{
  OS_TRACE ("OS::open");

#if defined (ACE_WIN32)
  DWORD access = GENERIC_READ;
  if (ACE_BIT_ENABLED (mode, O_WRONLY))
    access = GENERIC_WRITE;
  else if (ACE_BIT_ENABLED (mode, O_RDWR))
    access = GENERIC_READ | GENERIC_WRITE;

  DWORD creation = OPEN_EXISTING;

  if ((mode & (_O_CREAT | _O_EXCL)) == (_O_CREAT | _O_EXCL))
    creation = CREATE_NEW;
  else if ((mode & (_O_CREAT | _O_TRUNC)) == (_O_CREAT | _O_TRUNC))
    creation = CREATE_ALWAYS;
  else if (ACE_BIT_ENABLED (mode, _O_CREAT))
    creation = OPEN_ALWAYS;
  else if (ACE_BIT_ENABLED (mode, _O_TRUNC))
    creation = TRUNCATE_EXISTING;

  DWORD flags = 0;

  if (ACE_BIT_ENABLED (mode, _O_TEMPORARY))
    flags |= FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY;

  if (ACE_BIT_ENABLED (mode, FILE_FLAG_WRITE_THROUGH))
    flags |= FILE_FLAG_WRITE_THROUGH;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_OVERLAPPED))
    flags |= FILE_FLAG_OVERLAPPED;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_NO_BUFFERING))
    flags |= FILE_FLAG_NO_BUFFERING;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_RANDOM_ACCESS))
    flags |= FILE_FLAG_RANDOM_ACCESS;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_SEQUENTIAL_SCAN))
    flags |= FILE_FLAG_SEQUENTIAL_SCAN;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_DELETE_ON_CLOSE))
    flags |= FILE_FLAG_DELETE_ON_CLOSE;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_BACKUP_SEMANTICS))
    flags |= FILE_FLAG_BACKUP_SEMANTICS;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_POSIX_SEMANTICS))
    flags |= FILE_FLAG_POSIX_SEMANTICS;


  DWORD shared_mode = perms;
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;

#if defined (ACE_HAS_WINCE)
  ACE_HANDLE h = ::CreateFileW (ACE_Ascii_To_Wide (filename).wchar_rep (),
                                access,
                                shared_mode,
                                OS::default_win32_security_attributes_r
                                  (sa, &sa_buffer, &sd_buffer),
                                creation,
                                flags,
                                0);
#else /* ACE_HAS_WINCE */
  ACE_HANDLE h = ::CreateFileA (filename,
                                access,
                                shared_mode,
                                OS::default_win32_security_attributes_r
                                  (sa, &sa_buffer, &sd_buffer),
                                creation,
                                flags,
                                0);
#endif /* ACE_HAS_WINCE */

  if (ACE_BIT_ENABLED (mode, _O_APPEND))
    {
      LONG high_size = 0;
      if (h != ACE_INVALID_HANDLE
          && ::SetFilePointer (h,
                               0,
                               &high_size,
                               FILE_END) == INVALID_SET_FILE_POINTER
          && GetLastError () != NO_ERROR)
        {
          //ACE_MT (OS::thread_mutex_unlock (ace_os_monitor_lock);)
          ACE_FAIL_RETURN (ACE_INVALID_HANDLE);
        }

      //ACE_MT (OS::thread_mutex_unlock (ace_os_monitor_lock);)
    }

  if (h == ACE_INVALID_HANDLE)
    ACE_FAIL_RETURN (h);
  else
    return h;
#elif defined (INTEGRITY)
  ACE_UNUSED_ARG (sa);
  if(!strcmp(filename,ACE_DEV_NULL)) {
      OSCALL_RETURN (::AllocateNullConsoleDescriptor(), ACE_HANDLE, -1);
  }
  else {
      OSCALL_RETURN (::open (filename, mode, perms), ACE_HANDLE, -1);
  }
#else
  ACE_UNUSED_ARG (sa);
  OSCALL_RETURN (::open (filename, mode, perms), ACE_HANDLE, ACE_INVALID_HANDLE);
#endif /* ACE_WIN32 */
}

#if defined (ACE_HAS_WCHAR)
ACE_HANDLE
OS::open (const wchar_t *filename,
              int mode,
              mode_t perms,
              LPSECURITY_ATTRIBUTES sa)
{
#if defined (ACE_WIN32)
  // @@ (brunsch) Yuck, maybe there is a way to combine the code
  // here with the char version

  DWORD access = GENERIC_READ;
  if (ACE_BIT_ENABLED (mode, O_WRONLY))
    access = GENERIC_WRITE;
  else if (ACE_BIT_ENABLED (mode, O_RDWR))
    access = GENERIC_READ | GENERIC_WRITE;

  DWORD creation = OPEN_EXISTING;

  if ((mode & (_O_CREAT | _O_EXCL)) == (_O_CREAT | _O_EXCL))
    creation = CREATE_NEW;
  else if ((mode & (_O_CREAT | _O_TRUNC)) == (_O_CREAT | _O_TRUNC))
    creation = CREATE_ALWAYS;
  else if (ACE_BIT_ENABLED (mode, _O_CREAT))
    creation = OPEN_ALWAYS;
  else if (ACE_BIT_ENABLED (mode, _O_TRUNC))
    creation = TRUNCATE_EXISTING;

  DWORD flags = 0;

  if (ACE_BIT_ENABLED (mode, _O_TEMPORARY))
    flags |= FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY;

  if (ACE_BIT_ENABLED (mode, FILE_FLAG_WRITE_THROUGH))
    flags |= FILE_FLAG_WRITE_THROUGH;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_OVERLAPPED))
    flags |= FILE_FLAG_OVERLAPPED;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_NO_BUFFERING))
    flags |= FILE_FLAG_NO_BUFFERING;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_RANDOM_ACCESS))
    flags |= FILE_FLAG_RANDOM_ACCESS;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_SEQUENTIAL_SCAN))
    flags |= FILE_FLAG_SEQUENTIAL_SCAN;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_DELETE_ON_CLOSE))
    flags |= FILE_FLAG_DELETE_ON_CLOSE;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_BACKUP_SEMANTICS))
    flags |= FILE_FLAG_BACKUP_SEMANTICS;
  if (ACE_BIT_ENABLED (mode, FILE_FLAG_POSIX_SEMANTICS))
    flags |= FILE_FLAG_POSIX_SEMANTICS;

  ACE_MT (ACE_thread_mutex_t *ace_os_monitor_lock = 0;)
/*
  if (ACE_BIT_ENABLED (mode, _O_APPEND))
    {
      ACE_MT
        (
          ace_os_monitor_lock = static_cast <ACE_thread_mutex_t *> (
            OS_Object_Manager::preallocated_object[
              OS_Object_Manager::OS_MONITOR_LOCK]);
          OS::thread_mutex_lock (ace_os_monitor_lock);
        )
    }
*/
  DWORD shared_mode = perms;
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;

  ACE_HANDLE h = ::CreateFileW (filename,
                                access,
                                shared_mode,
                                OS::default_win32_security_attributes_r
                                  (sa, &sa_buffer, &sd_buffer),
                                creation,
                                flags,
                                0);

  if (ACE_BIT_ENABLED (mode, _O_APPEND))
    {
      LONG high_size = 0;
      if (h != ACE_INVALID_HANDLE
          && ::SetFilePointer (h,
                               0,
                               &high_size,
                               FILE_END) == INVALID_SET_FILE_POINTER
          && GetLastError () != NO_ERROR)
        {
          //ACE_MT (OS::thread_mutex_unlock (ace_os_monitor_lock);)
          ACE_FAIL_RETURN (ACE_INVALID_HANDLE);
        }

      //ACE_MT (OS::thread_mutex_unlock (ace_os_monitor_lock);)
    }

  if (h == ACE_INVALID_HANDLE)
    ACE_FAIL_RETURN (h);
  else
    return h;
#else /* ACE_WIN32 */
  // Just emulate with ascii version
  return OS::open (ACE_Wide_To_Ascii (filename).char_rep (),
                       mode,
                       perms,
                       sa);
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */


