// $Id: OS_NS_math.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_math.h"

ACE_RCSID(ace, OS_NS_math, "$Id: OS_NS_math.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

namespace OS {

  ACE_INLINE double
  floor (double x)
  {
    // This method computes the largest integral value not greater than x.
    if(x > 0)
      return static_cast<long> (x);
    else if (static_cast<long> (x) == x)
      return x;
    else
      return static_cast<long>(x) - 1;
  }

  ACE_INLINE double
  ceil (double x)
  {
    // This method computes the smallest integral value not less than x.
    if (x < 0)
      return static_cast<long> (x);
    else if (static_cast<long> (x) == x)
      return x;
    else
      return static_cast<long> (x) + 1;
  }

  ACE_INLINE double
  log2 (double x)
  {
    return ace_log2_helper (x);
  }

} // OS namespace


