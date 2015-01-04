// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_sys_mman.h
 *
 *  $Id: OS_NS_sys_mman.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_SYS_MMAN_H
# define OS_NS_SYS_MMAN_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/Global_Macros.h"
#include "os/os_include/sys/os_types.h"
#include "os/os_include/sys/os_mman.h"
#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export



namespace OS
{

  //@{ @name A set of wrappers for memory mapped files.
  ACE_NAMESPACE_INLINE_FUNCTION
  int madvise (caddr_t addr,
               size_t len,
               int map_advice);

  ACE_NAMESPACE_INLINE_FUNCTION
  void *mmap (void *addr,
              size_t len,
              int prot,
              int flags,
              ACE_HANDLE handle,
              ACE_OFF_T off = 0,
              ACE_HANDLE *file_mapping = 0,
              LPSECURITY_ATTRIBUTES sa = 0,
              const ACE_TCHAR *file_mapping_name = 0);

  ACE_NAMESPACE_INLINE_FUNCTION
  int mprotect (void *addr,
                size_t len,
                int prot);

  ACE_NAMESPACE_INLINE_FUNCTION
  int msync (void *addr,
             size_t len,
             int sync);

  ACE_NAMESPACE_INLINE_FUNCTION
  int munmap (void *addr,
              size_t len);
  //@}

  ACE_NAMESPACE_INLINE_FUNCTION
  ACE_HANDLE shm_open (const ACE_TCHAR *filename,
                       int mode,
                       mode_t perms = 0,
                       LPSECURITY_ATTRIBUTES sa = 0);

  ACE_NAMESPACE_INLINE_FUNCTION
  int shm_unlink (const ACE_TCHAR *path);

} /* namespace OS */



#endif /* OS_NS_SYS_MMAN_H */
