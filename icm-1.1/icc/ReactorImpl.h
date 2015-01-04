#ifndef _REACTOR_IMPL_H_
#define _REACTOR_IMPL_H_

#include "icc/EventHandler.h"

class ReactorImpl
{
public:

  //virtual int runReactorEventLoop () = 0;

  //Reactor (ReactorImpl *implementaion = 0, int deleteImplementation = 0);

  virtual ~ReactorImpl (void) {}

  virtual int handleEvents (TimeValue* maxWaitTime = 0) = 0;

  virtual int registerHandler (EventHandler *eventHandler, ReactorMask mask) = 0;

  virtual int removeHandler (EventHandler *eventHandler, ReactorMask mask) = 0;

  virtual long scheduleTimer (EventHandler *eventHandler, 
                              const void* arg, 
                              const TimeValue& delay,
                              const TimeValue& interval = TimeValue::zero) = 0;

  virtual int resetTimerInterval (long timerId, const TimeValue& interval) = 0;

  virtual int cancelTimer (long timerId, const void** arg = 0, int dontCallHandleClose = 1) = 0;

  virtual int cancelTimer (EventHandler *eventHandler, int dontCallHandleClose = 1) = 0;

};

#endif //_REACTOR_IMPL_H_
