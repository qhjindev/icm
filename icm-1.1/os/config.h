#ifndef ICC_CONFIG_H
#define ICC_CONFIG_H

#if defined(WIN32)
  #include "os/config-win32.h"
#elif defined(__hpux)
  #include "os/config-hpux-11.00.h"
#elif defined(AIX)
  #include "os/config-aix-5.x.h"
#else
  #include "os/config-linux.h"
#endif //WIN32

#define ACE_HAS_EVENT_POLL
#define ICC_LACKS_CDR_ALIGNMENT

#endif //ICC_CONFIG_H
