// -*- C++ -*-

//=============================================================================
/**
 *  @file   OS_NS_math.h
 *
 *  $Id: OS_NS_math.h,v 1.2 2009/03/19 05:29:23 qinghua Exp $
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 *  @author Jesper S. M|ller<stophph@diku.dk>
 *  @author and a cast of thousands...
 *
 *  Originally in OS.h.
 */
//=============================================================================

#ifndef OS_NS_MATH_H
# define OS_NS_MATH_H



# include "os/config-all.h"

# if !defined (ACE_LACKS_PRAGMA_ONCE)
#  pragma once
# endif /* ACE_LACKS_PRAGMA_ONCE */

#include "os/os_include/os_math.h"

#include /**/ "os/ACE_export.h"

#if defined (ACE_EXPORT_MACRO)
#  undef ACE_EXPORT_MACRO
#endif
#define ACE_EXPORT_MACRO ACE_Export


/*
 * We inline and undef some functions that may be implemented
 * as macros on some platforms. This way macro definitions will
 * be usable later as there is no way to save the macro definition
 * using the pre-processor.
 *
 */
inline double ace_log2_helper (double x)
{
#if defined (log2)
  return log2 (x);
#undef log2
#else
#  if !defined (ACE_LACKS_LOG2)
  return ACE_STD_NAMESPACE::log2 (x);
#  else
  /*
    ==================================================================

                log (x)
                   k
      log (x) = -------
         b      log (b)
                   k

    meaning the binary logarithm of x using the natural logarithm, for
    example, is:


                log (x)
                   e
      log (x) = -------
         2      log (2)
                   e

    ==================================================================
   */

  // Precomputed value of 1/log(2.0).  Saves an expensive division and
  // computing log(2.0) in each call.
  double const _1_ln2 = 1.442695040888963407359924681002;

  return log (x) * _1_ln2;
#  endif /* !ACE_LACKS_LOG2 */
#endif /* defined (log2) */
}




namespace OS
{
  /// This method computes the largest integral value not greater than x.
  ACE_NAMESPACE_INLINE_FUNCTION
  double floor (double x);

  /// This method computes the smallest integral value not less than x.
  ACE_NAMESPACE_INLINE_FUNCTION
  double ceil (double x);

  /// This method computes the base-2 logarithm of x.
  ACE_NAMESPACE_INLINE_FUNCTION
  double log2 (double x);

} /* namespace OS */



#endif /* OS_NS_MATH_H */
