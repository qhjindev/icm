// $Id: Auto_Event.cpp 69051 2005-10-28 16:14:56Z ossama $

#include "icc/AutoEvent.h"


ACE_Auto_Event::ACE_Auto_Event (int initial_state,
                                int type,
                                const char *name,
                                void *arg)
  : ACE_Event (0,
               initial_state,
               type,
               ACE_TEXT_CHAR_TO_TCHAR (name),
               arg)
{
}

ACE_Auto_Event::~ACE_Auto_Event (void)
{
}

#if defined (ACE_HAS_WCHAR)
ACE_Auto_Event::ACE_Auto_Event (int initial_state,
                                int type,
                                const wchar_t *name,
                                void *arg)
  : ACE_Event (0,
               initial_state,
               type,
               ACE_TEXT_WCHAR_TO_TCHAR (name),
               arg)
{
}
#endif /* ACE_HAS_WCHAR */

void
ACE_Auto_Event::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_Event::dump ();
#endif /* ACE_HAS_DUMP */
}


