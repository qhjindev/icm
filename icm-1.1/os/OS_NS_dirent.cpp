// $Id: OS_NS_dirent.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_Errno.h"
#include "os/OS_NS_dirent.h"

ACE_RCSID(ace, OS_NS_dirent, "$Id: OS_NS_dirent.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")


namespace OS
{

ACE_INLINE void
closedir (ACE_DIR *d)
{
#if defined (ACE_HAS_DIRENT)
# if defined (ACE_WIN32) && defined (ACE_LACKS_CLOSEDIR)
  OS::closedir_emulation (d);
  delete [] d->directory_name_;
  delete d;
# elif defined (ACE_HAS_WCLOSEDIR) && defined (ACE_USES_WCHAR)
  ::wclosedir (d);
# else /* ACE_WIN32 && ACE_LACKS_CLOSEDIR */
  ::closedir (d);
# endif /* ACE_WIN32 && ACE_LACKS_CLOSEDIR */

#else /* ACE_HAS_DIRENT */
  ACE_UNUSED_ARG (d);
#endif /* ACE_HAS_DIRENT */
}

ACE_INLINE ACE_DIR *
opendir (const ACE_TCHAR *filename)
{
#if defined (ACE_HAS_DIRENT)
#    if defined (ACE_WIN32) && defined (ACE_LACKS_OPENDIR)
  return ::OS::opendir_emulation (filename);
#  elif defined (ACE_HAS_WOPENDIR) && defined (ACE_USES_WCHAR)
  return ::wopendir (filename);
#    elif defined (ACE_HAS_NONCONST_OPENDIR)
  return ::opendir (const_cast<char *> (filename));
#    else /* ! ACE_WIN32 && ACE_LACKS_OPENDIR */
  return ::opendir (ACE_TEXT_ALWAYS_CHAR (filename));
#    endif /* ACE_WIN32 && ACE_LACKS_OPENDIR */
#else
  ACE_UNUSED_ARG (filename);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_DIRENT */
}

ACE_INLINE struct ACE_DIRENT *
readdir (ACE_DIR *d)
{
#if defined (ACE_HAS_DIRENT)
#  if defined (ACE_WIN32) && defined (ACE_LACKS_READDIR)
     return OS::readdir_emulation (d);
#  elif defined (ACE_HAS_WREADDIR) && defined (ACE_USES_WCHAR)
     return ::wreaddir (d);
#  else /* ACE_WIN32 && ACE_LACKS_READDIR */
     return ::readdir (d);
#  endif /* ACE_WIN32 && ACE_LACKS_READDIR */
#else
  ACE_UNUSED_ARG (d);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_DIRENT */
}

ACE_INLINE int
readdir_r (ACE_DIR *dirp,
           struct ACE_DIRENT *entry,
           struct ACE_DIRENT **result)
{
#if !defined (ACE_HAS_REENTRANT_FUNCTIONS)
  ACE_UNUSED_ARG (entry);
  // <result> has better not be 0!
  *result = OS::readdir (dirp);
  if (*result)
    return 0; // Keep iterating
  else
    return 1; // Oops, some type of error!
#elif defined (ACE_HAS_DIRENT) && !defined (ACE_LACKS_READDIR_R)
#  if defined (ACE_HAS_3_PARAM_READDIR_R)
       return ::readdir_r (dirp, entry, result);
#  else
       // <result> had better not be 0!
       *result = ::readdir_r (dirp, entry);
       return 0;
#  endif /* sun */
#else  /* ! ACE_HAS_DIRENT  ||  ACE_LACKS_READDIR_R */
  ACE_UNUSED_ARG (dirp);
  ACE_UNUSED_ARG (entry);
  ACE_UNUSED_ARG (result);
  ACE_NOTSUP_RETURN (0);

#endif /* ACE_HAS_REENTRANT_FUNCTIONS */
}

ACE_INLINE void
rewinddir (ACE_DIR *d)
{
#if defined (ACE_HAS_DIRENT)
#  if defined (ACE_HAS_WREWINDDIR) && defined (ACE_USES_WCHAR)
  ::wrewinddir (d);
#  elif !defined (ACE_LACKS_REWINDDIR)
  ace_rewinddir_helper (d);
#  else
  ACE_UNUSED_ARG (d);
#  endif /* !defined (ACE_LACKS_REWINDDIR) */
#endif /* ACE_HAS_DIRENT */
}

ACE_INLINE int
scandir (const ACE_TCHAR *dirname,
         struct ACE_DIRENT **namelist[],
         ACE_SCANDIR_SELECTOR selector,
         ACE_SCANDIR_COMPARATOR comparator)
{
#if defined (ACE_HAS_SCANDIR)
  return ::scandir (ACE_TEXT_ALWAYS_CHAR (dirname),
                    namelist,
#  if defined (ACE_SCANDIR_SEL_LACKS_CONST)
                    reinterpret_cast<ACE_SCANDIR_OS_SELECTOR> (selector),
#  else
                    selector,
#  endif /* ACE_SCANDIR_SEL_LACKS_CONST */
#  if defined (ACE_SCANDIR_CMP_USES_VOIDPTR) || \
      defined (ACE_SCANDIR_CMP_USES_CONST_VOIDPTR)
                    reinterpret_cast<ACE_SCANDIR_OS_COMPARATOR> (comparator));
#  else
                    comparator);
#  endif /* ACE_SCANDIR_CMP_USES_VOIDPTR */

#else /* ! defined ( ACE_HAS_SCANDIR) */
  return OS::scandir_emulation (dirname, namelist, selector, comparator);
#endif /* ACE_HAS_SCANDIR */
}

ACE_INLINE int
alphasort (const void *a, const void *b)
{
#if defined (ACE_LACKS_ALPHASORT)
  return strcmp ((*static_cast<const struct ACE_DIRENT * const *>(a))->d_name,
                          (*static_cast<const struct ACE_DIRENT * const *>(b))->d_name);
#else
#  if defined (ACE_SCANDIR_CMP_USES_VOIDPTR)
  return ::alphasort (const_cast<void *>(a),
                      const_cast<void *>(b));
#  elif defined (ACE_SCANDIR_CMP_USES_CONST_VOIDPTR)
  return ::alphasort (a, b);
#  else
  return ::alphasort ((const struct ACE_DIRENT **)a,
                      (const struct ACE_DIRENT **)b);
#  endif
#endif
}

ACE_INLINE void
seekdir (ACE_DIR *d, long loc)
{
#if defined (ACE_HAS_DIRENT) && !defined (ACE_LACKS_SEEKDIR)
  ::seekdir (d, loc);
#else  /* ! ACE_HAS_DIRENT  ||  ACE_LACKS_SEEKDIR */
  ACE_UNUSED_ARG (d);
  ACE_UNUSED_ARG (loc);
#endif /* ! ACE_HAS_DIRENT  ||  ACE_LACKS_SEEKDIR */
}

ACE_INLINE long
telldir (ACE_DIR *d)
{
#if defined (ACE_HAS_DIRENT) && !defined (ACE_LACKS_TELLDIR)
  return ::telldir (d);
#else  /* ! ACE_HAS_DIRENT  ||  ACE_LACKS_TELLDIR */
  ACE_UNUSED_ARG (d);
  errno = ENOTSUP ; 
  return -1;
#endif /* ! ACE_HAS_DIRENT  ||  ACE_LACKS_TELLDIR */
}

}


#include "os/OS_NS_errno.h"
#include "os/OS_NS_string.h"
//#include "Log_Msg.h"
#include "os/OS_NS_stdlib.h"



#if defined (ACE_LACKS_CLOSEDIR)
void
OS::closedir_emulation (ACE_DIR *d)
{
#if defined (ACE_WIN32)
  if (d->current_handle_ != INVALID_HANDLE_VALUE)
    ::FindClose (d->current_handle_);

  d->current_handle_ = INVALID_HANDLE_VALUE;
  d->started_reading_ = 0;
  if (d->dirent_ != 0)
    {
      OS::free (d->dirent_->d_name);
      OS::free (d->dirent_);
    }
#else /* ACE_WIN32 */
  ACE_UNUSED_ARG (d);
#endif /* ACE_WIN32 */
}
#endif /* ACE_LACKS_CLOSEDIR */

#if defined (ACE_LACKS_OPENDIR)
ACE_DIR *
OS::opendir_emulation (const ACE_TCHAR *filename)
{
#if defined (ACE_WIN32)
#  if defined (ACE_HAS_WINCE) && !defined (INVALID_FILE_ATTRIBUTES)
#    define INVALID_FILE_ATTRIBUTES 0xFFFFFFFF
#  endif

  ACE_DIR *dir;
  ACE_TCHAR extra[3] = {0,0,0};

   // Check if filename is a directory.
   DWORD fileAttribute = ACE_TEXT_GetFileAttributes (filename);
   if (fileAttribute == INVALID_FILE_ATTRIBUTES
       || !(fileAttribute & FILE_ATTRIBUTE_DIRECTORY))
     return 0;

/*
  Note: the semantics of the win32 function FindFirstFile take the
  basename(filename) as a pattern to be matched within the dirname(filename).
  This is contrary to the behavior of the posix function readdir which treats
  basename(filename) as a directory to be opened and read.

  For this reason, we append a slash-star or backslash-star to the supplied
  filename so the result is that FindFirstFile will do what we need.

  According to the documentation for FindFirstFile, either a '/' or a '\' may
  be used as a directory name separator.

  Of course, it is necessary to ensure that this is only done if the trailing
  filespec is not already there.

  Phil Mesnier
*/

  size_t lastchar = OS::strlen (filename);
  if (lastchar > 0)
    {
      if (filename[lastchar-1] != '*')
        {
          if (filename[lastchar-1] != '/' && filename[lastchar-1] != '\\')
            OS::strcpy (extra, ACE_TEXT ("/*"));
          else
            OS::strcpy (extra, ACE_TEXT ("*"));
        }
    }

  dir = new ACE_DIR;
  dir->directory_name_ = new ACE_TCHAR[lastchar + OS::strlen (extra) + 1];

  OS::strcpy (dir->directory_name_, filename);
  if (extra[0])
    OS::strcat (dir->directory_name_, extra);
  dir->current_handle_ = INVALID_HANDLE_VALUE;
  dir->started_reading_ = 0;
  dir->dirent_ = 0;
  return dir;
#else /* WIN32 */
  ACE_UNUSED_ARG (filename);
  ACE_NOTSUP_RETURN (0);
#endif /* WIN32 */
}
#endif /* ACE_LACKS_CLOSEDIR */

#if defined (ACE_LACKS_READDIR)
struct ACE_DIRENT *
OS::readdir_emulation (ACE_DIR *d)
{
#if defined (ACE_WIN32)
  if (d->dirent_ != 0)
    {
      OS::free (d->dirent_->d_name);
      OS::free (d->dirent_);
      d->dirent_ = 0;
    }

  if (!d->started_reading_)
    {
      d->current_handle_ = ACE_TEXT_FindFirstFile (d->directory_name_,
                                                   &d->fdata_);
      d->started_reading_ = 1;
    }
  else
    {
      int retval = ACE_TEXT_FindNextFile (d->current_handle_,
                                          &d->fdata_);
      if (retval == 0)
        {
          // Make sure to close the handle explicitly to avoid a leak!
          ::FindClose (d->current_handle_);
          d->current_handle_ = INVALID_HANDLE_VALUE;
        }
    }

  if (d->current_handle_ != INVALID_HANDLE_VALUE)
    {
      d->dirent_ = (ACE_DIRENT *)
        OS::malloc (sizeof (ACE_DIRENT));

      if (d->dirent_ != 0)
        {
          d->dirent_->d_name = (ACE_TCHAR*)
            OS::malloc ((OS::strlen (d->fdata_.cFileName) + 1)
                            * sizeof (ACE_TCHAR));
          OS::strcpy (d->dirent_->d_name, d->fdata_.cFileName);
          d->dirent_->d_reclen = sizeof (ACE_DIRENT);
        }

      return d->dirent_;
    }
  else
    return 0;
#else /* ACE_WIN32 */
  ACE_UNUSED_ARG (d);
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_WIN32 */
}
#endif /* ACE_LACKS_READDIR */

#if !defined (ACE_HAS_SCANDIR)
int
OS::scandir_emulation (const ACE_TCHAR *dirname,
                           ACE_DIRENT **namelist[],
                           ACE_SCANDIR_SELECTOR selector,
                           ACE_SCANDIR_COMPARATOR comparator)
{
  ACE_DIR *dirp = OS::opendir (dirname);

  if (dirp == 0)
    return -1;
  // A sanity check here.  "namelist" had better not be zero.
  else if (namelist == 0)
    return -1;

  ACE_DIRENT **vector = 0;
  ACE_DIRENT *dp = 0;
  int arena_size = 0;

  int nfiles = 0;
  int fail = 0;

  // @@ This code shoulduse readdir_r() rather than readdir().
  for (dp = OS::readdir (dirp);
       dp != 0;
       dp = OS::readdir (dirp))
    {
      if (selector && (*selector)(dp) == 0)
        continue;

      // If we get here, we have a dirent that the user likes.
      if (nfiles == arena_size)
        {
          ACE_DIRENT **newv = 0;
          if (arena_size == 0)
            arena_size = 10;
          else
            arena_size *= 2;

          newv = (ACE_DIRENT **) OS::realloc (vector,
                                              arena_size * sizeof (ACE_DIRENT *));
          if (newv == 0)
            {
              fail = 1;
              break;
            }
          vector = newv;
        }

#if defined (ACE_LACKS_STRUCT_DIR)
      ACE_DIRENT *newdp = (ACE_DIRENT *) OS::malloc (sizeof (ACE_DIRENT));
#else
      size_t dsize =
        sizeof (ACE_DIRENT) +
        ((OS::strlen (dp->d_name) + 1) * sizeof (ACE_TCHAR));
      ACE_DIRENT *newdp = (ACE_DIRENT *) OS::malloc (dsize);
#endif /* ACE_LACKS_STRUCT_DIR */

      if (newdp == 0)
        {
          fail = 1;
          break;
        }

#if defined (ACE_LACKS_STRUCT_DIR)
      newdp->d_name = (ACE_TCHAR*) OS::malloc (
        (OS::strlen (dp->d_name) + 1) * sizeof (ACE_TCHAR));

      if (newdp->d_name == 0)
        {
          fail = 1;
          OS::free (newdp);
          break;
        }

      // Don't use memcpy here since d_name is now a pointer
      newdp->d_ino = dp->d_ino;
      newdp->d_off = dp->d_off;
      newdp->d_reclen = dp->d_reclen;
      OS::strcpy (newdp->d_name, dp->d_name);
      vector[nfiles++] = newdp;
#else
      vector[nfiles++] = (ACE_DIRENT *) OS::memcpy (newdp, dp, dsize);
#endif /* ACE_LACKS_STRUCT_DIR */
    }

  if (fail)
    {
      OS::closedir (dirp);
      while (vector && nfiles-- > 0)
        {
#if defined (ACE_LACKS_STRUCT_DIR)
          OS::free (vector[nfiles]->d_name);
#endif /* ACE_LACKS_STRUCT_DIR */
          OS::free (vector[nfiles]);
        }
      OS::free (vector);
      return -1;
    }

  OS::closedir (dirp);

  *namelist = vector;

  if (comparator)
    OS::qsort (*namelist,
                   nfiles,
                   sizeof (ACE_DIRENT *),
                   (ACE_COMPARE_FUNC) comparator);

  return nfiles;
}
#endif /* !ACE_HAS_SCANDIR */


