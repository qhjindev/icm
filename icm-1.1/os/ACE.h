
#ifndef ICC_ACE_H
#define ICC_ACE_H



#include "os/config-all.h"

namespace ACE {

/**
 * Reset the limit on the number of open handles.  If @a new_limit
 * == -1 set the limit to the maximum allowable.  Otherwise, set
 * the limit value to @a new_limit.  If @a increase_limit_only is
 * non-0 then only allow increases to the limit.
 */
int set_handle_limit (int new_limit = -1,  int increase_limit_only = 0);

/**
 * Returns the maximum number of open handles currently permitted in
 * this process.  This maximum may be extended using
 * @c ACE::set_handle_limit.
 */
 int max_handles (void);

}

#endif //ICC_ACE_H
