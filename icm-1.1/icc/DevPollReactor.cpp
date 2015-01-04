#include "icc/DevPollReactor.h"
#include "icc/Log.h"

DevPollReactor::EventTuple::EventTuple(EventHandler* eh, ReactorMask m, bool is_suspended, bool is_controlled) :
    event_handler(eh), mask(m), suspended(is_suspended), controlled(is_controlled) {
}

DevPollReactor::HandlerRepository::HandlerRepository(void) :
    size_(0), max_size_(0), handlers_(0) {
}

bool DevPollReactor::HandlerRepository::invalid_handle(ACE_HANDLE handle) const {
  if (handle < 0 || handle >= this->max_size_) {
    errno = EINVAL;
    return true;
  } else
    return false;
}

bool DevPollReactor::HandlerRepository::handle_in_range(ACE_HANDLE handle) const {
  if (handle >= 0 && handle < this->max_size_)
    return true;
  else {
    errno = EINVAL;
    return false;
  }
}

int DevPollReactor::HandlerRepository::open(size_t size) {
  this->max_size_ = size;
  this->handlers_ = new EventTuple[size];
  return 0;
}

int DevPollReactor::HandlerRepository::unbind_all(void) {
  // Unbind all of the event handlers; similar to remove_handler() on all.
  for (int handle = 0; handle < this->max_size_; ++handle) {
    EventTuple *entry = this->find(handle);
    if (entry == 0)
      continue;

    (void) entry->event_handler->handleClose(handle, entry->mask);
    this->unbind(handle, false);
  }

  return 0;
}

int DevPollReactor::HandlerRepository::close(void) {
  if (this->handlers_ != 0) {
    this->unbind_all();

    delete[] this->handlers_;
    this->handlers_ = 0;
  }

  return 0;
}

DevPollReactor::EventTuple *
DevPollReactor::HandlerRepository::find(ACE_HANDLE handle) {
  EventTuple *tuple = 0;

  // Only bother to search for the <handle> if it's in range.
  if (!this->handle_in_range(handle)) {
    errno = ERANGE;
    return 0;
  }

  tuple = &(this->handlers_[handle]);
  if (tuple->event_handler == 0) {
    errno = ENOENT;
    tuple = 0;
  }

  return tuple;
}

int DevPollReactor::HandlerRepository::bind(ACE_HANDLE handle, EventHandler *event_handler, ReactorMask mask) {
  if (event_handler == 0)
    return -1;

  if (handle == ACE_INVALID_HANDLE
  )
    handle = event_handler->getHandle();

  if (this->invalid_handle(handle))
    return -1;

  this->handlers_[handle].event_handler = event_handler;
  this->handlers_[handle].mask = mask;
  //event_handler->add_reference ();
  ++this->size_;

  return 0;
}

int DevPollReactor::HandlerRepository::unbind(ACE_HANDLE handle, bool decr_refcnt) {
  EventTuple *entry = this->find(handle);
  if (entry == 0)
    return -1;

  //if (decr_refcnt)
  //entry->event_handler->remove_reference ();

  entry->event_handler = 0;
  entry->mask = EventHandler::NULL_MASK;
  entry->suspended = false;
  entry->controlled = false;
  --this->size_;
  return 0;
}

DevPollReactor::DevPollReactor(void) :
    m_initialized(false), m_poll_fd(ACE_INVALID_HANDLE), epoll_wait_in_progress(false), timer_queue(0) {
  if (this->open(4096) == -1)
    ACE_ERROR((LM_ERROR, "%p\n", "DevPollReactor::open failed inside DevPollReactor::CTOR"));
}

int DevPollReactor::open(size_t size) {
  if (this->m_initialized)
    return -1;

  OS::memset(&this->m_event, 0, sizeof(this->m_event));
  this->m_event.data.fd = ACE_INVALID_HANDLE;

  int result = 0;
  if (result != -1 && this->timer_queue == 0) {
    this->timer_queue = new TimerHeap;
    if (this->timer_queue == 0)
      result = -1;
  }

  this->m_poll_fd = ::epoll_create(size);
  if (this->m_poll_fd == -1)
    result = -1;

  if (result != -1 && this->handler_rep.open(size) == -1)
    result = -1;

  if (result != -1)
    this->m_initialized = true;
  else
    (void) this->close();

  return result;
}

int DevPollReactor::close(void) {
  int result = 0;
  if (this->m_poll_fd != ACE_INVALID_HANDLE) {
    result = OS::close(this->m_poll_fd);
  }

  OS::memset(&this->m_event, 0, sizeof(this->m_event));
  this->m_event.data.fd = ACE_INVALID_HANDLE;

  this->m_poll_fd = ACE_INVALID_HANDLE;
  this->m_initialized = false;

  return result;
}

int DevPollReactor::workPending(TimeValue* maxWaitTime) {
  if (this->m_event.data.fd != ACE_INVALID_HANDLE
  )
    return 1;

  TimeValue timerBuf(0);
  TimeValue *thisTimeout = this->timer_queue->calculateTimeout(maxWaitTime, &timerBuf);

  // Check if we have timers to fire.
  int const timersPending = ((thisTimeout != 0 && maxWaitTime == 0) || (thisTimeout != 0 && maxWaitTime != 0 && *thisTimeout != *maxWaitTime) ? 1 : 0);

  long const timeout = (thisTimeout == 0 ? -1 /* Infinity */: static_cast<long>(thisTimeout->msec()));

  {
    ACE_GUARD_RETURN(SYNCH_MUTEX, grd, this->m_to_be_resumed_lock, -1);
    this->epoll_wait_in_progress = true;
    for (ResumeMap::iterator i = this->to_be_resumed.begin(); i != this->to_be_resumed.end(); ++i) {
      // Make sure that 1) the handle is still registered,
      // 2) the registered handler is the one we're waiting to resume.
      EventTuple *info = this->handler_rep.find(i->first);
      if (info != 0 && info->event_handler == i->second) {
        this->resume_handler_i(i->first);
      }
    }
    this->to_be_resumed.clear();
  }

  // Wait for an event.
  int const nfds = ::epoll_wait(this->m_poll_fd, &this->m_event, 1, static_cast<int>(timeout));
  // Count on this being an atomic update; at worst, we may get an
  // extraneous notify() from dispatch_io_event.
  this->epoll_wait_in_progress = false;

  // If timers are pending, override any timeout from the poll.
  return (nfds == 0 && timersPending != 0 ? 1 : nfds);
}

int DevPollReactor::handleEvents(TimeValue *max_wait_time) {
  ACE_TRACE ("ACE_Dev_Poll_Reactor::handle_events_i");

  int result = 0;

  // Poll for events
  //
  // If the underlying event wait call was interrupted via the interrupt
  // signal (i.e. returned -1 with errno == EINTR) then the loop will
  // be restarted if so desired.
  do {
    result = this->workPending(max_wait_time);
    if (result == -1 && (this->restart_ == 0 || errno != EINTR))
      ACE_ERROR((LM_ERROR, ACE_TEXT("%t: %p\n"), ACE_TEXT("work_pending_i")));
  } while (result == -1 && this->restart_ != 0 && errno == EINTR);

  if (result == 0 || (result == -1 && errno == ETIME))
    return 0;
  else if (result == -1) {
    if (errno != EINTR
    )
      return -1;

    return 1;
  }

  // Dispatch an event.
  return this->dispatch();
}

// Dispatch an event. On entry, the token is held by the caller. If an
// event is found to dispatch, the token is released before dispatching it.
int DevPollReactor::dispatch() {
  // Perform the Template Method for dispatching the first located event.
  // We dispatch only one to effectively dispatch events concurrently.
  // As soon as an event is located, the token is released, allowing the
  // next waiter to begin getting an event while we dispatch one here.
  int result = 0;

  // Handle timers early since they may have higher latency
  // constraints than I/O handlers.  Ideally, the order of
  // dispatching should be a strategy...
  if ((result = this->dispatch_timer_handler()) != 0)
    return result;

  // If no timer dispatched, check for an I/O event.
  result = this->dispatchIoEvent();

  return result;
}

int DevPollReactor::dispatch_timer_handler() {
  if (this->timer_queue->isEmpty())
    return 0; // Empty timer queue so cannot have any expired timers.

  this->timer_queue->expire();

  return 0;
}

int DevPollReactor::dispatchIoEvent() {
  const __uint32_t out_event = EPOLLOUT;
  const __uint32_t exc_event = EPOLLPRI;
  const __uint32_t in_event = EPOLLIN;
  const __uint32_t err_event = EPOLLHUP | EPOLLERR;

  // epoll_wait() pulls one event which is stored in event_. If the handle
  // is invalid, there's no event there. Else process it. In any event, we
  // have the event, so clear event_ for the next thread.
  const ACE_HANDLE handle = this->m_event.data.fd;
  __uint32_t revents = this->m_event.events;
  this->m_event.data.fd = ACE_INVALID_HANDLE;
  this->m_event.events = 0;
  if (handle != ACE_INVALID_HANDLE)
  {
    /* When using sys_epoll, we can attach arbitrary user
     data to the descriptor, so it can be delivered when
     activity is detected. Perhaps we should store event
     handler together with descriptor, instead of looking
     it up in a repository ? Could it boost performance ?
     */
    EventTuple *info = this->handler_rep.find(handle);
    if (info == 0) // No registered handler any longer
        {
      this->m_event.data.fd = ACE_INVALID_HANDLE; // Dump the event
      return 0;
    }

    // Figure out what to do first in order to make it easier to manage
    // the bit twiddling and possible pfds increment before releasing
    // the token for dispatch.
    // Note that if there's an error (such as the handle was closed
    // without being removed from the event set) the EPOLLHUP and/or
    // EPOLLERR bits will be set in revents.
    ReactorMask disp_mask = 0;
    EventHandler *eh = info->event_handler;
    int (EventHandler::*callback)(ACE_HANDLE) = 0;
    if (ACE_BIT_ENABLED (revents, out_event)) {
      disp_mask = EventHandler::WRITE_MASK;
      callback = &EventHandler::handleOutput;
      ACE_CLR_BITS(revents, out_event);
    } else if (ACE_BIT_ENABLED (revents, exc_event)) {
      disp_mask = EventHandler::EXCEPT_MASK;
      callback = &EventHandler::handleException;
      ACE_CLR_BITS(revents, exc_event);
    } else if (ACE_BIT_ENABLED (revents, in_event)) {
      disp_mask = EventHandler::READ_MASK;
      callback = &EventHandler::handleInput;
      ACE_CLR_BITS(revents, in_event);
    } else if (ACE_BIT_ENABLED (revents, err_event)) {
      this->removeHandlerI(handle, EventHandler::ALL_EVENTS_MASK, info->event_handler);
      return 1;
    } else {
      ACE_ERROR((LM_ERROR, ACE_TEXT ("(%t) dispatch_io h %d unknown events 0x%x\n"), handle, revents));
    }

    // With epoll, events are registered with oneshot, so the handle is
    // effectively suspended; future calls to epoll_wait() will select
    // the next event, so they're not managed here.
    // The hitch to this is that the notify handler is always registered
    // WITHOUT oneshot and is never suspended/resumed. This avoids endless
    // notify loops caused by the notify handler requiring a resumption
    // which requires the token, which requires a notify, etc. described
    // in Bugzilla 3714. So, never suspend the notify handler.
    bool reactor_resumes_eh = false;
    //if (eh != this->notify_handler_)
    {
      info->suspended = true;

      reactor_resumes_eh = eh->resumeHandler() == EventHandler::ICC_REACTOR_RESUMES_HANDLER;
    }

    int status = 0; // gets callback status, below.
    {
      // Modify the reference count in an exception-safe way.
      // Note that eh could be the notify handler. It's not strictly
      // necessary to manage its refcount, but since we don't enable
      // the counting policy, it won't do much. Management of the
      // notified handlers themselves is done in the notify handler.
      //ACE_Dev_Poll_Handler_Guard eh_guard (eh);

      // Release the reactor token before upcall.
      //guard.release_token ();

      // Dispatch the detected event; will do the repeated upcalls
      // if callback returns > 0, unless it's the notify handler (which
      // returns the number of notfies dispatched, not an indication of
      // re-callback requested). If anything other than the notify, come
      // back with either 0 or < 0.
      status = this->upcall(eh, callback, handle);

      // If the callback returned 0, epoll-based needs to resume the
      // suspended handler but dev/poll doesn't.
      // The epoll case is optimized to not acquire the token in order
      // to resume the handler; the handler is added to a list of those
      // that need to be resumed and is handled by the next leader
      // that does an epoll_wait().
      // In both epoll and dev/poll cases, if the callback returns <0,
      // the token needs to be acquired and the handler checked and
      // removed if it hasn't already been.
      if (status == 0) {
        // epoll-based effectively suspends handlers around the upcall.
        // If the handler must be resumed, add it to the list.
        if (reactor_resumes_eh) {
          ACE_GUARD_RETURN(SYNCH_MUTEX, grd, this->m_to_be_resumed_lock, -1);
          bool map_was_empty = this->to_be_resumed.empty();
          this->to_be_resumed.insert(ResumeMap::value_type(handle, eh));
        }

        return 1;
      }

      // All state in the handler repository may have changed during the
      // upcall while other threads had the token. Thus, reacquire the
      // token and evaluate what's needed. If the upcalled handler is still
      // the handler of record for handle, continue with checking whether
      // or not to remove or resume the handler.
      //guard.acquire ();
      info = this->handler_rep.find(handle);
      if (info != 0 && info->event_handler == eh) {
        if (status < 0)
          this->removeHandlerI(handle, disp_mask);
      }
    }
    // Scope close handles eh ref count decrement, if needed.

    return 1;
  }

  return 0;
}

int DevPollReactor::upcall(EventHandler* eventHandler, int(EventHandler::*callback)(ACE_HANDLE), ACE_HANDLE handle) {
  // If the handler returns positive value (requesting a reactor
  // callback) just call back as many times as the handler requests
  // it.  The handler is suspended internally and other threads are off
  // handling other things.
  int status = 0;

do {
  status = (eventHandler->*callback)(handle);
}while(status > 0)

return status;
}

int DevPollReactor::registerHandler(EventHandler *eventHandler, ReactorMask mask) {
  return this->register_handler_i(eventHandler->getHandle(), eventHandler, mask);
}

int DevPollReactor::register_handler_i(ACE_HANDLE handle, EventHandler *event_handler, ReactorMask mask) {
  if (handle == ACE_INVALID_HANDLE || mask == EventHandler::NULL_MASK) {
    errno = EINVAL;
    return -1;
  }

  if (this->handler_rep.find(handle) == 0) {
    // Handler not present in the repository.  Bind it.
    if (this->handler_rep.bind(handle, event_handler, mask) != 0)
      return -1;

    EventTuple *info = this->handler_rep.find(handle);

    struct epoll_event epev;
    OS::memset(&epev, 0, sizeof(epev));
    static const int op = EPOLL_CTL_ADD;

    epev.data.fd = handle;
    epev.events = this->reactor_mask_to_poll_event(mask);
    // All but the notify handler get registered with oneshot to facilitate
    // auto suspend before the upcall. See dispatch_io_event for more
    // information.
    epev.events |= EPOLLONESHOT;

    if (::epoll_ctl(this->m_poll_fd, op, handle, &epev) == -1) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("epoll_ctl")));
      (void) this->handler_rep.unbind(handle);
      return -1;
    }
    info->controlled = true;

  } else {
    // Handler is already present in the repository, so register it
    // again, possibly for different event.  Add new mask to the
    // current one.
    if (this->mask_ops_i(handle, mask, Reactor::ADD_MASK) == -1)
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("mask_ops_i")), -1);
  }

  // Note the fact that we've changed the state of the wait_set_,
  // which is used by the dispatching loop to determine whether it can
  // keep going or if it needs to reconsult select ().
  // this->state_changed_ = 1;

  return 0;
}

int DevPollReactor::removeHandler(EventHandler *handler, ReactorMask mask) {
  return this->removeHandlerI(handler->getHandle(), mask);
}

int DevPollReactor::removeHandler(ACE_HANDLE handle, ReactorMask mask) {
  return this->removeHandlerI(handle, mask);
}

int DevPollReactor::removeHandlerI(ACE_HANDLE handle, ReactorMask mask, EventHandler *eh) {

  // If registered event handler not the same as eh, don't mess with
  // the mask, but do the proper callback and refcount when needed.
  bool handle_reg_changed = true;
  EventTuple *info = this->handler_rep.find(handle);
  if (info == 0 && eh == 0) // Nothing to work with
    return -1;
  if (info != 0 && (eh == 0 || info->event_handler == eh)) {
    if (this->mask_ops_i(handle, mask, Reactor::CLR_MASK) == -1)
      return -1;
    handle_reg_changed = false;
    eh = info->event_handler;
  }

  // Check for ref counting now - handle_close () may delete eh.
  //bool const requires_reference_counting = eh->reference_counting_policy().value() == ACE_Event_Handler::Reference_Counting_Policy::ENABLED;

  if (ACE_BIT_DISABLED (mask, EventHandler::DONT_CALL))
    (void) eh->handleClose(handle, mask);

  // If there are no longer any outstanding events on the given handle
  // then remove it from the handler repository.
  if (!handle_reg_changed && info->mask == EventHandler::NULL_MASK)
    this->handler_rep.unbind(handle, false);

  return 0;
}

int DevPollReactor::suspendHandler(EventHandler *event_handler) {
  if (event_handler == 0) {
    errno = EINVAL;
    return -1;
  }

  ACE_HANDLE handle = event_handler->getHandle();

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return this->suspendHandlerI(handle);
}

int DevPollReactor::suspendHandler(ACE_HANDLE handle) {
  //ACE_TRACE ("ACE_Dev_Poll_Reactor::suspend_handler");

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return this->suspendHandlerI(handle);
}

int DevPollReactor::suspendHandlerI(ACE_HANDLE handle) {
  EventTuple *info = this->handler_rep.find(handle);
  if (info == 0)
    return -1;

  if (info->suspended)
    return 0; // Already suspended.  @@ Should this be an error?

  // Remove the handle from the "interest set."
  //
  // Note that the associated event handler is still in the handler
  // repository, but no events will be polled on the given handle thus
  // no event will be dispatched to the event handler.

  struct epoll_event epev;
  OS::memset(&epev, 0, sizeof(epev));
  static const int op = EPOLL_CTL_DEL;

  epev.events = 0;
  epev.data.fd = handle;

  if (::epoll_ctl(this->m_poll_fd, op, handle, &epev) == -1)
    return -1;

  info->controlled = false;

  info->suspended = true;

  return 0;
}

int DevPollReactor::resume_handler(EventHandler *event_handler) {
  if (event_handler == 0) {
    errno = EINVAL;
    return -1;
  }

  ACE_HANDLE handle = event_handler->getHandle();

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return this->resume_handler_i(handle);
}

int DevPollReactor::resume_handler_i(ACE_HANDLE handle) {
  ACE_TRACE ("ACE_Dev_Poll_Reactor::resume_handler_i");

  EventTuple *info = this->handler_rep.find(handle);
  if (info == 0)
    return -1;

  if (!info->suspended)
    return 0;

  ReactorMask mask = info->mask;
  if (mask == EventHandler::NULL_MASK) {
    info->suspended = false;
    return 0;
  }

  // Place the handle back in to the "interest set."
  //
  // Events for the given handle will once again be polled.

  struct epoll_event epev;
  OS::memset(&epev, 0, sizeof(epev));
  int op = EPOLL_CTL_ADD;
  if (info->controlled)
    op = EPOLL_CTL_MOD;
  epev.events = this->reactor_mask_to_poll_event(mask) | EPOLLONESHOT;
  epev.data.fd = handle;

  if (::epoll_ctl(this->m_poll_fd, op, handle, &epev) == -1)
    return -1;
  info->controlled = true;

  info->suspended = false;

  return 0;
}

long DevPollReactor::scheduleTimer(EventHandler *event_handler, const void *arg, const TimeValue &delay, const TimeValue &interval) {
  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  if (0 != this->timer_queue)
    return this->timer_queue->schedule(event_handler, arg, OS::gettimeofday() + delay, interval);

  errno = ESHUTDOWN;
  return -1;
}

int DevPollReactor::cancelTimer(EventHandler *event_handler, int dont_call_handle_close) {
  //ACE_TRACE ("ACE_Dev_Poll_Reactor::cancel_timer");

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return (this->timer_queue == 0 ? 0 : this->timer_queue->cancel(event_handler, dont_call_handle_close));
}

int DevPollReactor::cancelTimer(long timer_id, const void **arg, int dont_call_handle_close) {
  //ACE_TRACE ("ACE_Dev_Poll_Reactor::cancel_timer");

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return (this->timer_queue == 0 ? 0 : this->timer_queue->cancel(timer_id, arg, dont_call_handle_close));
}

int DevPollReactor::mask_ops(EventHandler *event_handler, ReactorMask mask, int ops) {
  //ACE_TRACE ("ACE_Dev_Poll_Reactor::mask_ops");

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return this->mask_ops_i(event_handler->getHandle(), mask, ops);
}

int DevPollReactor::mask_ops(ACE_HANDLE handle, ReactorMask mask, int ops) {
  //ACE_TRACE ("ACE_Dev_Poll_Reactor::mask_ops");

  //ACE_MT (ACE_GUARD_RETURN (ACE_Dev_Poll_Reactor_Token, mon, this->token_, -1));

  return this->mask_ops_i(handle, mask, ops);
}

int DevPollReactor::mask_ops_i(ACE_HANDLE handle, ReactorMask mask, int ops) {
  EventTuple *info = this->handler_rep.find(handle);
  if (info == 0)
    return -1;

  // Block out all signals until method returns.
  //ACE_Sig_Guard sb;

  ReactorMask const old_mask = info->mask;
  ReactorMask new_mask = old_mask;

  // Perform GET, CLR, SET, and ADD operations on the interest/wait
  // set and the suspend set (if necessary).
  //
  // GET = 1, Retrieve current value
  // SET = 2, Set value of bits to new mask (changes the entire mask)
  // ADD = 3, Bitwise "or" the value into the mask (only changes
  //          enabled bits)
  // CLR = 4  Bitwise "and" the negation of the value out of the mask
  // (only changes enabled bits)
  //
  // Returns the original mask.

  switch (ops) {
  case Reactor::GET_MASK:
    // The work for this operation is done in all cases at the
    // begining of the function.
    return old_mask;

  case Reactor::CLR_MASK:
    ACE_CLR_BITS(new_mask, mask);
    break;

  case Reactor::SET_MASK:
    new_mask = mask;
    break;

  case Reactor::ADD_MASK:
    ACE_SET_BITS(new_mask, mask);
    break;

  default:
    return -1;
  }

  /// Reset the mask for the given handle.
  info->mask = new_mask;

  // Only attempt to alter events for the handle from the
  // "interest set" if it hasn't been suspended. If it has been
  // suspended, the revised mask will take affect when the
  // handle is resumed. The exception is if all the mask bits are
  // cleared, we can un-control the fd now.
  if (!info->suspended || (info->controlled && new_mask == 0)) {

    short const events = this->reactor_mask_to_poll_event(new_mask);

    struct epoll_event epev;
    OS::memset(&epev, 0, sizeof(epev));
    int op;

    // ACE_Event_Handler::NULL_MASK ???
    if (new_mask == 0) {
      op = EPOLL_CTL_DEL;
      epev.events = 0;
    } else {
      op = EPOLL_CTL_MOD;
      epev.events = events | EPOLLONESHOT;
    }

    epev.data.fd = handle;

    if (::epoll_ctl(this->m_poll_fd, op, handle, &epev) == -1) {
      // If a handle is closed, epoll removes it from the poll set
      // automatically - we may not know about it yet. If that's the
      // case, a mod operation will fail with ENOENT. Retry it as
      // an add.
      if (op == EPOLL_CTL_MOD && errno == ENOENT && ::epoll_ctl(this->m_poll_fd, EPOLL_CTL_ADD, handle, &epev) == -1)
        return -1;
    }
    info->controlled = (op != EPOLL_CTL_DEL);

  }

  return old_mask;
}

short DevPollReactor::reactor_mask_to_poll_event(ReactorMask mask) {
  //ACE_TRACE ("ACE_Dev_Poll_Reactor::reactor_mask_to_poll_event");

  if (mask == EventHandler::NULL_MASK)
    // No event.  Remove from interest set.
    return EPOLL_CTL_DEL;

  short events = 0;

  // READ, ACCEPT, and CONNECT flag will place the handle in the
  // read set.
  if (ACE_BIT_ENABLED (mask, EventHandler::READ_MASK) || ACE_BIT_ENABLED (mask, EventHandler::ACCEPT_MASK) || ACE_BIT_ENABLED (mask, EventHandler::CONNECT_MASK))
  {
    ACE_SET_BITS(events, EPOLLIN);
  }

  // WRITE and CONNECT flag will place the handle in the write set.
  if (ACE_BIT_ENABLED (mask, EventHandler::WRITE_MASK) || ACE_BIT_ENABLED (mask, EventHandler::CONNECT_MASK))
  {
    ACE_SET_BITS(events, EPOLLOUT);
  }

  // EXCEPT flag will place the handle in the except set.
  if (ACE_BIT_ENABLED (mask, EventHandler::EXCEPT_MASK)) {
    ACE_SET_BITS(events, EPOLLPRI);
  }

  return events;
}
