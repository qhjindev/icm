#ifndef ICC_REACTOR_H_
#define ICC_REACTOR_H_

#include "icc/EventHandler.h"

class ReactorImpl;

class Reactor
{
public:
  enum
  {
    /// Retrieve current value of the the "ready" mask or the
    /// "dispatch" mask.
    GET_MASK = 1,
    /// Set value of bits to new mask (changes the entire mask).
    SET_MASK = 2,
    /// Bitwise "or" the value into the mask (only changes enabled
    /// bits).
    ADD_MASK = 3,
    /// Bitwise "and" the negation of the value out of the mask (only
    /// changes enabled bits).
    CLR_MASK = 4
  };

  static Reactor *instance (void);

  static Reactor *instance (Reactor *, int deleteReactor = 0);

  virtual int runReactorEventLoop ();

  Reactor (ReactorImpl *implementaion = 0, int deleteImplementation = 0);

  virtual ~Reactor (void);

  virtual int handleEvents (TimeValue* maxWaitTime = 0);

  virtual int registerHandler (EventHandler *eventHandler, ReactorMask mask);

  virtual int removeHandler (EventHandler *eventHandler, ReactorMask mask);

  virtual long scheduleTimer (EventHandler *eventHandler, 
                              const void* arg, 
                              const TimeValue& delay,
                              const TimeValue& interval = TimeValue::zero);

  virtual int resetTimerInterval (long timerId, const TimeValue& interval);

  virtual int cancelTimer (long timerId, const void** arg = 0, int dontCallHandleClose = 1);

  virtual int cancelTimer (EventHandler *eventHandler, int dontCallHandleClose = 1);

protected:
  ReactorImpl *m_implementation;

  static Reactor *reactor;

};

#endif //ICC_REACTOR_H_
