
#include "os/OS.h"
#include "icc/SelectReactor.h"
#include "icc/Reactor.h"
#include "icc/Thread.h"
#include "icc/SocketAcceptor.h"
#include "icc/SocketConnector.h"
#include "icc/TimerHeap.h"
#include "os/OS_NS_fcntl.h"
#include <map>

using namespace std;

EventHandler *
SelectReactorHandlerRepository::find (ACE_HANDLE handle)
{
  map<ACE_HANDLE,EventHandler *>::iterator iter = this->eventHandlers.find (handle);
  if (iter != this->eventHandlers.end ())
    return iter->second;
  else
    return 0;
}

int
SelectReactorHandlerRepository::bind(ACE_HANDLE handle, EventHandler *eventHandler, ReactorMask mask)
{
  if (eventHandler == 0)
    return -1;

  if (handle == ACE_INVALID_HANDLE)
    handle = eventHandler->getHandle ();

  this->eventHandlers.insert(std::pair<ACE_HANDLE,EventHandler*>(handle, eventHandler));

#if !defined (ACE_WIN32)
  if (this->m_maxHandleP1 < handle + 1)
    this->m_maxHandleP1 = handle + 1;
#endif //ACE_WIN32

  this->selectReactor.bitOps (handle, mask, this->selectReactor.waitSet, Reactor::ADD_MASK);

  return 0;
}

ACE_HANDLE
SelectReactorHandlerRepository::maxHandleP1 (void) const
{
#ifdef ACE_WIN32
  return (ACE_HANDLE)this->eventHandlers.size ();
#else
  return this->m_maxHandleP1;
#endif  /* ACE_WIN32 */
}

SelectReactorHandlerRepository::mapType::iterator
SelectReactorHandlerRepository::findEh (ACE_HANDLE handle)
{
  return this->eventHandlers.find (handle);
}

int SelectReactorHandlerRepository::unbind (ACE_HANDLE handle, ReactorMask mask)
{
  EventHandler * eventHandler = 0;
  mapType::iterator it = this->findEh (handle);
  if (it == this->eventHandlers.end ())
    return -1;
  else
    eventHandler = it->second;

  this->selectReactor.bitOps (handle, mask, this->selectReactor.waitSet, Reactor::CLR_MASK);
  // If there are no longer any outstanding events on this <handle>
  // then we can totally shut down the Event_Handler.

  bool const hasAnyWaitMask =
    (this->selectReactor.waitSet.rdMask.isSet (handle)
     || this->selectReactor.waitSet.wrMask.isSet (handle)
     || this->selectReactor.waitSet.exMask.isSet (handle));

//  bool complete_removal = false;

  if (!hasAnyWaitMask)
    {
//#if defined (ACE_WIN32)
      //if (eventHandler != 0 && this->eventHandlers.erase(it) == -1)
        //return -1;  // Should not happen!
	  if (eventHandler != 0)
		  this->eventHandlers.erase(it);

      // The handle has been completely removed.
//      complete_removal = true;
    }

  if (eventHandler == 0)
    return -1;

  // Close down the <Event_Handler> unless we've been instructed not
  // to.
  if (ACE_BIT_ENABLED (mask, EventHandler::DONT_CALL) == 0)
    (void) eventHandler->handleClose (handle, mask);

  return 0;
}

SelectReactor::SelectReactor ()
  :handlerRep (*this)
{
  this->timerQueue = new TimerHeap;
}

int SelectReactor::registerHandler (EventHandler *eventHandler, ReactorMask mask)
{
  return this->registerHandlerInternal (eventHandler->getHandle(), eventHandler, mask);
}

int SelectReactor::registerHandlerInternal (ACE_HANDLE handle, EventHandler *eventHandler, ReactorMask mask)
{
  return this->handlerRep.bind(handle, eventHandler, mask);
}

int SelectReactor::removeHandler (EventHandler *eventHandler, ReactorMask mask)
{
  return this->removeHandlerInternal (eventHandler->getHandle(), mask);
}

int SelectReactor::removeHandlerInternal (ACE_HANDLE handle, ReactorMask mask)
{
  return this->handlerRep.unbind (handle, mask);
}

long SelectReactor::scheduleTimer (EventHandler *eventHandler, 
                              const void* arg, 
                              const TimeValue& delay,
                              const TimeValue& interval)
{
  if (0 != this->timerQueue)
    return this->timerQueue->schedule (eventHandler,
                                       arg,
                                       OS::gettimeofday () + delay,
                                       interval);

  errno = ESHUTDOWN;
  return -1;
}

int SelectReactor::resetTimerInterval (long timerId, const TimeValue& interval)
{
  return this->timerQueue->resetInterval (timerId, interval);
}

int SelectReactor::cancelTimer (long timerId, const void** arg, int dontCallHandleClose)
{
  return this->timerQueue->cancel (timerId, arg, dontCallHandleClose);
}

int SelectReactor::cancelTimer (EventHandler* handler, int dontCallHandleClose)
{
  return this->timerQueue->cancel (handler, dontCallHandleClose);
}

int SelectReactor::handleEvents (TimeValue* maxWaitTime)
{
  int result = -1;

  this->dispatchSet.rdMask.reset ();
  this->dispatchSet.wrMask.reset ();
  this->dispatchSet.exMask.reset ();

  int numberOfActiveHandles = this->waitForMultipleEvents (this->dispatchSet,maxWaitTime);
  result = this->dispatch (numberOfActiveHandles, this->dispatchSet);

  return result;
}

int SelectReactor::waitForMultipleEvents (SelectReactorHandleSet &dispatchSet, TimeValue* maxWaitTime)
{
  TimeValue timerBuf (0);
  TimeValue *thisTimeout = 0;

  int numActiveHandles = 0;

  if (numActiveHandles == 0) {
    do {
      if (this->timerQueue == 0)
        return 0;

      thisTimeout = this->timerQueue->calculateTimeout (maxWaitTime, &timerBuf);

      dispatchSet.rdMask = this->waitSet.rdMask;
      dispatchSet.wrMask = this->waitSet.wrMask;
      dispatchSet.exMask = this->waitSet.exMask;

#ifdef ACE_WIN32
          // This arg is ignored on Windows and causes pointer
          // truncation warnings on 64-bit compiles.
          int const width = 0;
#else
          int const width = this->handlerRep.maxHandleP1 ();
#endif  /* ACE_WIN32 */


      numActiveHandles = OS::select (width,
                                   dispatchSet.rdMask,
                                   dispatchSet.wrMask,
                                   dispatchSet.exMask,
                                   thisTimeout);
    } while (numActiveHandles == -1 && errno != EINTR);

    if (numActiveHandles > 0) {
#if !defined (ACE_WIN32)
      dispatchSet.rdMask.sync(this->handlerRep.maxHandleP1());
      dispatchSet.wrMask.sync(this->handlerRep.maxHandleP1());
      dispatchSet.exMask.sync(this->handlerRep.maxHandleP1());
#endif /*ACE_WIN32*/
    } else if (numActiveHandles == -1) {
      dispatchSet.rdMask.reset ();
      dispatchSet.wrMask.reset ();
      dispatchSet.exMask.reset ();
    }
  }

  return numActiveHandles;
}

int SelectReactor::dispatch (int activeHandleCount, SelectReactorHandleSet &dispatchSet)
{
  int ioHandlersDispatched = 0;
  int otherHandlersDispatched = 0;

  do {
    //int initialHandleCount = activeHandleCount;

    this->stateChanged = false;

    if (activeHandleCount == -1)
      return -1;
    else if (this->dispatchTimerHandler (otherHandlersDispatched) == -1)
      break;
    else if (activeHandleCount == 0)
      return ioHandlersDispatched + otherHandlersDispatched;

    else if (this->dispatchIoHandlers (dispatchSet,activeHandleCount,ioHandlersDispatched) == -1)
      break;
  } while (activeHandleCount > 0);

  return ioHandlersDispatched + otherHandlersDispatched;
}

int SelectReactor::dispatchTimerHandler (int& numberDispatched)
{
  numberDispatched += this->timerQueue->expire ();
  
  return 0;
}
  
int SelectReactor::dispatchIoHandlers (SelectReactorHandleSet & dispatchSet,
									   int &numActiveHandles,
									   int &numHandlersDispatched)
{
	if (this->dispatchIoSet (numActiveHandles,
							 numHandlersDispatched,
							 EventHandler::WRITE_MASK,
							 dispatchSet.wrMask,
							 &EventHandler::handleOutput) == -1) {
		numActiveHandles -= numHandlersDispatched;
		return -1;
	}

	if (this->dispatchIoSet (numActiveHandles,
							 numHandlersDispatched,
							 EventHandler::EXCEPT_MASK,
							 dispatchSet.exMask,
							 &EventHandler::handleException) == -1) {
		numActiveHandles -= numHandlersDispatched;
		return -1;
	}

	if (this->dispatchIoSet (numActiveHandles,
							 numHandlersDispatched,
							 EventHandler::READ_MASK,
							 dispatchSet.rdMask,
							 &EventHandler::handleInput) == -1) {
		numActiveHandles -= numHandlersDispatched;
		return -1;
	}

	numActiveHandles -= numHandlersDispatched;
	return 0;
}

int SelectReactor::dispatchIoSet (int numActiveHandles,
								  int &numHandlersDispatched,
								  int mask,
								  HandleSet &dispatchMask,
								  EH_PTMF callback)
{
	ACE_HANDLE handle;

	HandleSetIterator handleIter (dispatchMask);

	while ((handle = handleIter ()) != ACE_INVALID_HANDLE &&
          numHandlersDispatched < numActiveHandles) {
		++numHandlersDispatched;

		this->notifyHandle(handle, mask, this->handlerRep.find (handle), callback);
		this->clearDispatchMask (handle, mask);
	}

	return 0;
}

void SelectReactor::notifyHandle (ACE_HANDLE handle,
								  ReactorMask mask,
								  EventHandler *eventHandler,
								  EH_PTMF ptmf)
{
	if (eventHandler == 0)
		return;

	int status = (eventHandler->*ptmf) (handle);

	if (status < 0)
		this->removeHandlerInternal(handle, mask);
	//else if (status > 0)
		//readyMask.setBit (handle);
}

void SelectReactor::clearDispatchMask (ACE_HANDLE handle, ReactorMask mask)
{
  if (ACE_BIT_ENABLED (mask, EventHandler::READ_MASK) ||
      ACE_BIT_ENABLED (mask, EventHandler::ACCEPT_MASK) )   {
        this->dispatchSet.rdMask.clrBit (handle);
  }

  if (ACE_BIT_ENABLED (mask, EventHandler::WRITE_MASK)) {
    this->dispatchSet.wrMask.clrBit(handle);
  }

  if (ACE_BIT_ENABLED (mask, EventHandler::EXCEPT_MASK)) {
    this->dispatchSet.exMask.clrBit(handle);
  }

  this->stateChanged = true;
}

int SelectReactor::bitOps (ACE_HANDLE handle,
                          ReactorMask mask,
                          SelectReactorHandleSet &handleSet,
                          int ops)
{
  //if (this->handlerRep.handleInRange (handle) == 0)
    //return -1;

  FDS_PTMF ptmf = &HandleSet::setBit;
  u_long omask = EventHandler::NULL_MASK;

  if (handleSet.rdMask.isSet(handle))
    ACE_SET_BITS (omask, EventHandler::READ_MASK);
  if (handleSet.wrMask.isSet(handle))
    ACE_SET_BITS (omask, EventHandler::WRITE_MASK);
  if (handleSet.exMask.isSet(handle))
    ACE_SET_BITS (omask, EventHandler::EXCEPT_MASK);

  switch (ops) {
  case Reactor::GET_MASK:
    break;
  case Reactor::CLR_MASK:
    ptmf = &HandleSet::clrBit;
    this->clearDispatchMask(handle, mask);
  case Reactor::SET_MASK:
  case Reactor::ADD_MASK:
    if (ACE_BIT_ENABLED (mask, EventHandler::READ_MASK)
        || ACE_BIT_ENABLED (mask, EventHandler::ACCEPT_MASK)
        || ACE_BIT_ENABLED (mask, EventHandler::CONNECT_MASK)) {
      (handleSet.rdMask.*ptmf) (handle);
    } else if (ops == Reactor::SET_MASK)
      handleSet.rdMask.clrBit (handle);

    if (ACE_BIT_ENABLED (mask, EventHandler::WRITE_MASK)
        || ACE_BIT_ENABLED (mask, EventHandler::CONNECT_MASK)) {
      (handleSet.wrMask.*ptmf) (handle);
    } else if (ops == Reactor::SET_MASK)
      handleSet.wrMask.clrBit(handle);

    if (ACE_BIT_ENABLED (mask, EventHandler::EXCEPT_MASK)) {
      (handleSet.exMask.*ptmf) (handle);
    } else if (ops == Reactor::SET_MASK)
      handleSet.exMask.clrBit(handle);
    break;

  default:
    return -1;
  }

  return omask;
}







