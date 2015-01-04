
#include "icc/Reactor.h"
#include "icc/SelectReactor.h"

Reactor *Reactor::reactor = 0;

Reactor::Reactor (ReactorImpl *impl, int deleteImplementation)
 : m_implementation (0)
{
  this->m_implementation = impl;

  if (this->m_implementation == 0) {
    impl = new SelectReactor();
    this->m_implementation = impl;
  }
}

Reactor::~Reactor (void)
{
}

Reactor *
Reactor::instance (void)
{
  if (Reactor::reactor == 0) {
    Reactor::reactor = new Reactor;
  }

  return Reactor::reactor;
}

Reactor *
Reactor::instance (Reactor *r, int deleteReactor)
{
  Reactor *t = Reactor::reactor;
  Reactor::reactor = r;
  return t;
}

int
Reactor::runReactorEventLoop (void)
{
  while (1)   {

      this->m_implementation->handleEvents ();

    }

  return 0;
}

int
Reactor::handleEvents (TimeValue* maxWaitTime)
{
  return this->m_implementation->handleEvents (maxWaitTime);
}

int
Reactor::registerHandler (EventHandler *eventHandler, ReactorMask mask)
{
  Reactor *oldReactor = eventHandler->getReactor ();
  eventHandler->setReactor (this);

  int result = this->m_implementation->registerHandler (eventHandler, mask);
  if (result == -1)
    eventHandler->setReactor (oldReactor);

  return result;
}

int
Reactor::removeHandler (EventHandler *eventHandler, ReactorMask mask)
{
  return this->m_implementation->removeHandler (eventHandler, mask);
}

long
Reactor::scheduleTimer (EventHandler *eventHandler,
                       const void *arg,
                       const TimeValue &delta,
                       const TimeValue &interval)
{
  Reactor *oldReactor = eventHandler->getReactor ();
  eventHandler->setReactor (this);

  int result = this->m_implementation->scheduleTimer (eventHandler, arg, delta, interval);
  if (result == -1)
    eventHandler->setReactor (oldReactor);

  return result;
}

int
Reactor::cancelTimer (long timerId, const void** arg, int dontCallHandleClose)
{
  return this->m_implementation->cancelTimer (timerId, arg, dontCallHandleClose);
}

int
Reactor::cancelTimer (EventHandler *eventHandler, int dontCallHandleClose)
{
  return this->m_implementation->cancelTimer (eventHandler, dontCallHandleClose);
}

int
Reactor::resetTimerInterval (long timerId, const TimeValue &interval)
{
  return this->m_implementation ->resetTimerInterval (timerId, interval);
}
