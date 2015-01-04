#ifndef _ICC_EVENT_HANDLER_H_
#define _ICC_EVENT_HANDLER_H_

#include "os/OS.h"

class Reactor;

typedef unsigned long ReactorMask;

class EventHandler
{
public:
  enum
  {
    LO_PRIORITY = 0,
    HI_PRIORITY = 10,
    NULL_MASK = 0,
    READ_MASK = (1 << 0),
    WRITE_MASK = (1 << 1),
    EXCEPT_MASK = (1 << 2),
    ACCEPT_MASK = (1 << 3),
    CONNECT_MASK = (1 << 4),
    TIMER_MASK = (1 << 5),
    QOS_MASK = (1 << 6),
    GROUP_QOS_MASK = (1 << 7),
    SIGNAL_MASK = (1 << 8),
    ALL_EVENTS_MASK = READ_MASK |
                      WRITE_MASK |
                      EXCEPT_MASK |
                      ACCEPT_MASK |
                      CONNECT_MASK |
                      TIMER_MASK |
                      QOS_MASK |
                      GROUP_QOS_MASK |
                      SIGNAL_MASK,
    RWE_MASK = READ_MASK |
               WRITE_MASK |
               EXCEPT_MASK,
    DONT_CALL = (1 << 9)
  };

  virtual ~EventHandler(void);

  virtual ACE_HANDLE getHandle (void) const;

  virtual void setHandle (ACE_HANDLE);

  virtual int handleInput (ACE_HANDLE fd = ACE_INVALID_HANDLE);

  virtual int handleOutput (ACE_HANDLE fd = ACE_INVALID_HANDLE);

  virtual int handleTimeout (const TimeValue &currentTime, const void *act = 0);

  virtual int handleException (ACE_HANDLE fd = ACE_INVALID_HANDLE);

  virtual int handleClose (ACE_HANDLE, ReactorMask);

  virtual Reactor *getReactor (void) const;

  virtual void setReactor (Reactor * reactor);

  int resumeHandler(void);

  enum
    {
      /// The handler is not resumed at all. Could lead to deadlock..
      ICC_EVENT_HANDLER_NOT_RESUMED = -1,
      /// The reactor takes responsibility of resuming the handler and
      /// is the default
      ICC_REACTOR_RESUMES_HANDLER = 0,
      /// The application takes responsibility of resuming the handler
      ICC_APPLICATION_RESUMES_HANDLER
    };

private:

  Reactor *m_reactor;

};

#endif //_ICC_EVENT_HANDLER_H_
