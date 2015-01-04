/* -*- C++ -*- */
//=============================================================================
/**
 *  @file   config-win32.h
 *
 *  $Id: config-win32.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @brief  Microsoft Windows configuration file.
 *
 *  This file is the ACE configuration file for all of Microsoft Windows
 *  platforms that ACE runs on.  Based on preprocessor definitions, it
 *  includes other more specific configuration files.
 *
 *  @author Darrell Brunsch <brunsch@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_CONFIG_WIN32_H
#define ACE_CONFIG_WIN32_H


// NOTE: Please do not add anything besides #include's here.  Put other stuff
//       (definitions, etc.) in the included headers

// We need to ensure that for Borland vcl.h can be included before
// windows.h.  So we will not include config-win32-common.h from here,
// but instead let it be included at the appropriate place in
// config-win32-borland.h.
#if !defined (__BORLANDC__)
#    include "os/config-win32-common.h"
#endif /* !__BORLANDC__ */

// Include the config-win32-* file specific to the compiler
#if defined (__BORLANDC__)
#    include "config-win32-borland.h"
#elif defined (_MSC_VER)
#    include "os/config-win32-msvc.h"
#elif defined (ghs)
#    include "config-win32-ghs.h"
#elif defined (__MINGW32__)
#    include "config-win32-mingw.h"
#elif defined (__DMC__)
#    include "config-win32-dmc.h"
#else
#    error Compiler is not supported
#endif


#endif /* ACE_CONFIG_WIN32_H */

