#ifndef _SELECT_REACTOR_H_
#define _SELECT_REACTOR_H_

#include <map>
#include "os/config.h"

#include "icc/TimerQueue.h"

#include "icc/HandleSet.h"

#include "icc/ReactorImpl.h"

typedef void (HandleSet::*FDS_PTMF) (ACE_HANDLE);
typedef int (EventHandler::*EH_PTMF) (ACE_HANDLE);

class SelectReactorHandleSet
{
public:
  HandleSet rdMask;
  HandleSet wrMask;
  HandleSet exMask;
};

class SelectReactor;

class SelectReactorHandlerRepository
{
public:
  typedef std::map<ACE_HANDLE,EventHandler *> mapType;

  SelectReactorHandlerRepository (SelectReactor &reactor)
    :selectReactor (reactor)
#ifndef ACE_WIN32
     ,m_maxHandleP1 (0)
#endif  /* !ACE_WIN32 */
  {
  }

  EventHandler * find (ACE_HANDLE handle);

  int bind (ACE_HANDLE,EventHandler *,ReactorMask);

  int unbind (ACE_HANDLE, ReactorMask);

  ACE_HANDLE maxHandleP1 (void) const;

  mapType eventHandlers;

private:
  mapType::iterator findEh (ACE_HANDLE handle);

  SelectReactor &selectReactor;

#ifndef ACE_WIN32
  /// The highest currently active handle, plus 1 (ranges between 0 and
  /// @c max_size_.
  ACE_HANDLE m_maxHandleP1;
#endif  /* !ACE_WIN32 */

};

class SelectReactor : public ReactorImpl
{
  friend class SelectReactorHandlerRepository;

public:
  SelectReactor ();

  virtual int handleEvents (TimeValue* maxWaitTime = 0) ;

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

  virtual int registerHandlerInternal (ACE_HANDLE handle, EventHandler *eventHandler, ReactorMask mask);

  virtual int removeHandlerInternal (ACE_HANDLE handle, ReactorMask mask);

  virtual int waitForMultipleEvents (SelectReactorHandleSet &, TimeValue* maxWaitTime);

  virtual int dispatch (int nFound, SelectReactorHandleSet &);

  virtual int dispatchTimerHandler (int &numberDispatched);

  virtual int dispatchIoHandlers (SelectReactorHandleSet & dispatchSet,
								  int &numActiveHandles,
								  int &numHandlersDispatched);

  virtual int dispatchIoSet (int numActiveHandles,
						     int &numHandlersDispatched,
						     int mask,
						     HandleSet &dispatchMask,
						     EH_PTMF callback);

  virtual void notifyHandle (ACE_HANDLE handle,
							 ReactorMask mask,
							 EventHandler *eh,
							 EH_PTMF callback);

  virtual int bitOps (ACE_HANDLE handle,
                      ReactorMask mask,
                      SelectReactorHandleSet &handleSet,
                      int ops);

  virtual void clearDispatchMask (ACE_HANDLE handle, ReactorMask mask);

  SelectReactorHandleSet dispatchSet;
  SelectReactorHandleSet waitSet;

  SelectReactorHandlerRepository handlerRep;

  TimerQueue* timerQueue;

  bool stateChanged;
};

#endif //_SELECT_REACTOR_H_
