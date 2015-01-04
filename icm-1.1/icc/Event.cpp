
#include "icc/Event.h"

#include "icc/Log.h"


ACE_Event::ACE_Event (int manual_reset,
                      int initial_state,
                      int type,
                      const ACE_TCHAR *name,
                      void *arg,
                      LPSECURITY_ATTRIBUTES sa)
  : removed_ (false)
{
  if (OS::event_init (&this->handle_,
                          manual_reset,
                          initial_state,
                          type,
                          name,
                          arg,
                          sa) != 0)
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p\n"),
                ACE_TEXT ("ACE_Event::ACE_Event")));
}

ACE_Event::~ACE_Event (void)
{
  this->remove ();
}

int
ACE_Event::remove (void)
{
  int result = 0;
  if (!this->removed_)
    {
      this->removed_ = true;
      result = OS::event_destroy (&this->handle_);
    }
  return result;
}

int
ACE_Event::wait (void)
{
  return OS::event_wait (&this->handle_);
}

int
ACE_Event::wait (const TimeValue *abstime, int use_absolute_time)
{
  return OS::event_timedwait (&this->handle_,
                                  const_cast <TimeValue *> (abstime),
                                  use_absolute_time);
}

int
ACE_Event::signal (void)
{
  return OS::event_signal (&this->handle_);
}

int
ACE_Event::pulse (void)
{
  return OS::event_pulse (&this->handle_);
}

int
ACE_Event::reset (void)
{
  return OS::event_reset (&this->handle_);
}

ACE_INLINE ACE_event_t
ACE_Event::handle (void) const
{
  return this->handle_;
}

ACE_INLINE void
ACE_Event::handle (ACE_event_t new_handle)
{
  this->handle_ = new_handle;
}

void
ACE_Event::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
#endif /* ACE_HAS_DUMP */
}
