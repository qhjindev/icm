#ifndef ICC_DEV_POLL_REACTOR_H
#define ICC_DEV_POLL_REACTOR_H

#include <sys/epoll.h>
#include "icc/ReactorImpl.h"
#include "icc/Guard.h"
#include "icc/SynchTraits.h"

class DevPollReactor: public ReactorImpl {

  struct EventTuple {
    EventTuple(EventHandler* eh = 0, ReactorMask m = EventHandler::NULL_MASK, bool is_suspended = false, bool is_controlled = false);

    /// The event handler.
    EventHandler* event_handler;

    /// The event mask for the above event handler.
    ReactorMask mask;

    /// Flag that states whether or not the event handler is suspended.
    bool suspended;

    /// Flag to say whether or not this handle is registered with epoll.
    bool controlled;
  };

  class HandlerRepository {
  public:
    HandlerRepository(void);

    int open(size_t size);

    int close(void);

    EventTuple* find(ACE_HANDLE handle);

    int bind(ACE_HANDLE handle, EventHandler* handler, ReactorMask mask);

    int unbind(ACE_HANDLE handle, bool decr_refcnt = true);

    int unbind_all(void);

    bool invalid_handle(ACE_HANDLE handle) const;

    bool handle_in_range(ACE_HANDLE) const;

    size_t size(void) const;

    size_t max_size(void) const;

    void dump(void) const;

  private:
    /// Current number of handles.
    int size_;

    /// Maximum number of handles.
    int max_size_;

    /// The underlying array of event handlers.
    EventTuple* handlers_;
  };

public:

  DevPollReactor();

  virtual ~DevPollReactor(void) {
  }

  /// Initialization
  int open(size_t size);

  int close(void);

  // = Event loop drivers
  int workPending(TimeValue* maxWaitTime = TimeValue::zero);

  virtual int handleEvents(TimeValue* maxWaitTime = 0);

  // = Register and remove Handlers.

  virtual int registerHandler(EventHandler *eventHandler, ReactorMask mask);

  virtual int removeHandler(EventHandler *eventHandler, ReactorMask mask);

  virtual int removeHandler(ACE_HANDLE handle, ReactorMask mask);

  // = Suspend and resume Handlers.
  virtual int suspendHandler(EventHandler* eventHandler);

  virtual int suspendHandler(ACE_HANDLE handle);

  virtual int resume_handler(EventHandler* eventHandler);

  // = Timer management
  virtual long scheduleTimer(EventHandler *eventHandler, const void* arg, const TimeValue& delay, const TimeValue& interval = TimeValue::zero);

  virtual int resetTimerInterval(long timerId, const TimeValue& interval);

  virtual int cancelTimer(long timerId, const void** arg = 0, int dontCallHandleClose = 1);

  virtual int cancelTimer(EventHandler *eventHandler, int dontCallHandleClose = 1);

  /// GET/SET/ADD/CLR the dispatch mask "bit" bound with the
  /// event_handler and mask.
  /**
   * @return Old mask on success, -1 on error.
   */
  virtual int mask_ops(EventHandler *event_handler, ReactorMask mask, int ops);

  /// GET/SET/ADD/CLR the dispatch MASK "bit" bound with the handle
  /// and mask.
  /**
   * @return Old mask on success, -1 on error.
   */
  virtual int mask_ops(ACE_HANDLE handle, ReactorMask mask, int ops);

protected:
  /**
   * Returns non-zero if there are I/O events "ready" for dispatching,
   * but does not actually dispatch the event handlers.  By default,
   * don't block while checking this, i.e., "poll".
   */
  //int workPending(TimeValue* maxWaitTime);

  int dispatch();

  int dispatch_timer_handler();

  int dispatchIoEvent();

  int upcall(EventHandler* eventHandler, int(EventHandler::*callback)(ACE_HANDLE), ACE_HANDLE handle);

  int register_handler_i(ACE_HANDLE handle, EventHandler *event_handler, ReactorMask mask);

  int removeHandlerI(ACE_HANDLE handle, ReactorMask mask, EventHandler *eh = 0);

  int suspendHandlerI(ACE_HANDLE handle);

  int resume_handler_i(ACE_HANDLE handle);

  /// GET/SET/ADD/CLR the dispatch MASK "bit" bound with the handle
  /// and mask.  This internal helper method acquires no lock.
  /**
   * @return Old mask on success, -1 on error.
   */
  int mask_ops_i(ACE_HANDLE handle, ReactorMask mask, int ops);

  /// Convert a reactor mask to its corresponding poll() event mask.
  short reactor_mask_to_poll_event(ReactorMask mask);

protected:

  bool m_initialized;

  ACE_HANDLE m_poll_fd;

  struct epoll_event m_event;

  typedef std::map<ACE_HANDLE, EventHandler*> ResumeMap;
  ResumeMap to_be_resumed;
  volatile bool epoll_wait_in_progress;SYNCH_MUTEX m_to_be_resumed_lock;

  /// Restart the handle_events event loop method automatically when
  /// polling function in use (ioctl() in this case) is interrupted
  /// via an EINTR signal.
  bool restart_;

  HandlerRepository handler_rep;

  TimerQueue* timer_queue;
};

#endif //ICC_DEV_POLL_REACTOR_H
