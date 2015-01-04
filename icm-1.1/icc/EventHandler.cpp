
#include "icc/EventHandler.h"
#include "icc/Reactor.h"

EventHandler::~EventHandler(void) {

}

ACE_HANDLE 
EventHandler::getHandle (void) const
{
  return ACE_INVALID_HANDLE;
}

void 
EventHandler::setHandle (ACE_HANDLE)
{
}

int 
EventHandler::handleInput (ACE_HANDLE fd)
{
  return -1;
}

int 
EventHandler::handleOutput (ACE_HANDLE fd)
{
  return -1;
}

int 
EventHandler::handleTimeout (const TimeValue &currentTime, const void *act)
{
  return -1;
}

int 
EventHandler::handleException (ACE_HANDLE fd)
{
  return -1;
}

int 
EventHandler::handleClose (ACE_HANDLE, ReactorMask)
{
  return -1;
}

Reactor *
EventHandler::getReactor (void) const
{
  return this->m_reactor;
}

void
EventHandler::setReactor (Reactor * reactor)
{
  this->m_reactor = reactor;
}

int
EventHandler::resumeHandler(void)
{
  return EventHandler::ICC_REACTOR_RESUMES_HANDLER;
}

