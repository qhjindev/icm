
#ifndef ICC_PROACTOR_H
#define ICC_PROACTOR_H


#include "os/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

//#if ((defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS)))
// This only works on Win32 platforms and on Unix platforms supporting
// POSIX aio calls.

#include "icc/AsynchIo.h"
#include "icc/AsynchIoImpl.h"
#include "icc/ThreadManager.h"
#include "icc/TimerQueue.h"
//#include "ace/Timer_List.h"
#include "icc/TimerHeap.h"
//#include "ace/Timer_Wheel.h"


// Forward declarations.
class ProactorImpl;
class ACE_Proactor_Timer_Handler;

/**
 * @class ProactorHandleTimeoutUpcall
 *
 * @brief Functor for <ACE_Timer_Queue>.
 *
 * This class implements the functor required by the Timer
 * Queue to call <handle_timeout> on ACE_Handlers.
 */
class ACE_Export ProactorHandleTimeoutUpcall
{

  /// Type def for the timer queue.
  typedef TimerQueueT<Handler *,
                            ProactorHandleTimeoutUpcall,
                            SYNCH_RECURSIVE_MUTEX>
  TIMER_QUEUE;

  /// The main Proactor class has special permissions.
  friend class Proactor;

public:
  /// Constructor.
  ProactorHandleTimeoutUpcall (void);

  /// This method is called when the timer expires.
  int timeout (TIMER_QUEUE &timer_queue,
	       Handler *handler,
	       const void *arg,
         int recurringTimer,
	       const TimeValue &cur_time);

  /// This method is called when the timer is canceled.
  int cancellation (TIMER_QUEUE &timer_queue,
		    Handler *handler,
        int dont_call);

  /// This method is called when the timer queue is destroyed and the
  /// timer is still contained in it.
  int deletion (TIMER_QUEUE &timer_queue,
                Handler *handler,
                const void *arg);

protected:
  /// Set the proactor. This will fail, if one is already set!
  int proactor (Proactor &proactor);

  /// Handle to the proactor. This is needed for posting a timer result
  /// to the Proactor's completion queue.
  Proactor *proactor_;
};

/**
 * @class Proactor
 *
 * @brief A manager for asynchronous event demultiplexing.
 *
 * See the Proactor pattern description at
 * http://www.cs.wustl.edu/~schmidt/proactor.ps.gz for more
 * details.
 */
class ACE_Export Proactor
{

  // = Here are the private typedefs that the <Proactor> uses.

  typedef TimerQueueIteratorT<Handler *,
    ProactorHandleTimeoutUpcall,
    SYNCH_RECURSIVE_MUTEX>
  TIMER_QUEUE_ITERATOR;
  typedef TimerHeapT<Handler *,
    ProactorHandleTimeoutUpcall,
    SYNCH_RECURSIVE_MUTEX>
  TIMER_HEAP;
  typedef TimerHeapIteratorT<Handler *,
    ProactorHandleTimeoutUpcall,
    SYNCH_RECURSIVE_MUTEX>
  TIMER_HEAP_ITERATOR;

  // = Friendship.

  /// Timer handler runs a thread and manages the timers, on behalf of
  /// the Proactor.
  friend class ACE_Proactor_Timer_Handler;

public:
  /// Public type.
  typedef TimerQueueT<Handler *,
    ProactorHandleTimeoutUpcall,
    SYNCH_RECURSIVE_MUTEX>
  TIMER_QUEUE;

  /**
   * Constructor. If <implementation> is 0, the correct implementation
   * object will be created. <delete_implementation> flag determines
   * whether the implementation object should be deleted by the
   * Proactor or not. If <tq> is 0, a new TIMER_QUEUE is created.
   */
  Proactor (ProactorImpl *implementation = 0,
                int delete_implementation = 0,
                TIMER_QUEUE *tq = 0);

  /// Virtual destruction.
  virtual ~Proactor (void);

  /// Get pointer to a process-wide <Proactor>.  <threads> should
  /// be part of another method.
  static Proactor *instance (size_t threads = 0);

  /// Set pointer to a process-wide <Proactor> and return existing
  /// pointer.
  static Proactor *instance (Proactor *);

  /// Delete the dynamically allocated Singleton.
  static void close_singleton (void);

  /// Cleanup method, used by the <ACE_Object_Manager> to destroy the
  /// singleton.
  static void cleanup (void *instance, void *arg);

  // = Proactor event loop management methods.

  /// Run the event loop until the <Proactor::handle_events> method
  /// returns -1 or the <end_event_loop> method is invoked.
  static int run_event_loop (void);

  /**
   * Run the event loop until the <Proactor::handle_events> method
   * returns -1, the <end_event_loop> method is invoked, or the
   * <TimeValue> expires.
   */
  static int run_event_loop (TimeValue &tv);

  /**
   * Instruct the <Proactor::instance> to terminate its event
   * loop.
   * This method wakes up all the threads blocked on waiting for
   * completions and end the event loop.
   */
  static int end_event_loop (void);

  /**
   * Resets the <Proactor::end_event_loop_> static so that the
   * <run_event_loop> method can be restarted.
   */
  static int reset_event_loop (void);

  /// Report if the <Proactor::instance> event loop is finished.
  static int event_loop_done (void);

  /// Close the IO completion port.
  virtual int close (void);

  /// This method adds the <handle> to the I/O completion port. This
  /// function is a no-op function for Unix systems and returns 0;
  virtual int register_handle (ACE_HANDLE handle,
			       const void *completion_key);

  // = Timer management.
  /**
   * Schedule a <handler> that will expire after <time>.  If it
   * expires then <act> is passed in as the value to the <handler>'s
   * <handle_timeout> callback method.  This method returns a
   * <timer_id>. This <timer_id> can be used to cancel a timer before
   * it expires.  The cancellation ensures that <timer_ids> are unique
   * up to values of greater than 2 billion timers.  As long as timers
   * don't stay around longer than this there should be no problems
   * with accidentally deleting the wrong timer.  Returns -1 on
   * failure (which is guaranteed never to be a valid <timer_id>).
   */
  virtual long schedule_timer (Handler &handler,
			       const void *act,
			       const TimeValue &time);

  virtual long schedule_repeating_timer (Handler &handler,
					 const void *act,
					 const TimeValue &interval);

  // Same as above except <interval> it is used to reschedule the
  // <handler> automatically.

  /// This combines the above two methods into one. Mostly for backward
  /// compatibility.
  virtual long schedule_timer (Handler &handler,
			       const void *act,
			       const TimeValue &time,
			       const TimeValue &interval);

  /// Cancel all timers associated with this <handler>.  Returns number
  /// of timers cancelled.
  virtual int cancel_timer (Handler &handler,
			    int dont_call_handle_close = 1);

  /**
   * Cancel the single <Handler> that matches the <timer_id> value
   * (which was returned from the <schedule> method).  If <act> is
   * non-NULL then it will be set to point to the ``magic cookie''
   * argument passed in when the <Handler> was registered.  This makes
   * it possible to free up the memory and avoid memory leaks.
   * Returns 1 if cancellation succeeded and 0 if the <timer_id>
   * wasn't found.
   */
  virtual int cancel_timer (long timer_id,
			    const void **act = 0,
			    int dont_call_handle_close = 1);

  /**
   * Dispatch a single set of events, waiting up to a specified time limit
   * if necessary.
   * @param wait_time the time to wait for an event to occur. This is
   * a relative time. On successful return, the time is updated to
   * reflect the amount of time spent waiting for event(s) to occur.
   * @return Returns 0 if no events occur before the wait_time expires.
   * Returns 1 when a completion is dispatched. On error, returns -1
   * and sets errno accordingly.
   */
  virtual int handle_events (TimeValue &wait_time);

  /**
   * Block indefinitely until at least one event is dispatched.
   * @return Returns 1 when a completion is dispatched. On error, returns -1
   * and sets errno accordingly.
   */
  virtual int handle_events (void);

  /// Add wakeup dispatch threads (reinit).
  int wake_up_dispatch_threads (void);

  /// Close all dispatch threads.
  int close_dispatch_threads (int wait);

  /// Number of thread used as a parameter to CreatIoCompletionPort.
  size_t number_of_threads (void) const;
  void number_of_threads (size_t threads);

  /// Get/Set timer queue.
  TIMER_QUEUE *timer_queue (void) const;
  void timer_queue (TIMER_QUEUE *timer_queue);

  /**
   * Get the event handle.
   * It is a no-op in POSIX platforms and it returns
   * ACE_INVALID_HANDLE.
   */
  virtual ACE_HANDLE get_handle (void) const;

  /// Get the implementation class.
  virtual ProactorImpl *implementation (void) const;

  // = Factory methods for the operations

  // Note that the user does not have to use or know about these
  // methods.

  /// Create the correct implementation class for doing
  /// Asynch_Read_Stream.
  virtual AsynchReadStreamImpl *create_asynch_read_stream (void);

  /// Create the correct implementation class for doing
  /// Asynch_Write_Stream.
  virtual AsynchWriteStreamImpl *create_asynch_write_stream (void);

  /// Create the correct implementation class for doing
  /// Asynch_Read_File.
  virtual AsynchReadFileImpl *create_asynch_read_file (void);

  /// Create the correct implementation class for doing
  /// Asynch_Write_File.
  virtual AsynchWriteFileImpl *create_asynch_write_file (void);

  // = Factory methods for the results

  // Note that the user does not have to use or know about these
  // methods unless they want to "fake" results.

  /// Create the correct implementation class for AsynchReadStream::Result class.
  virtual AsynchReadStreamResultImpl *create_asynch_read_stream_result (Handler &handler,
                                                                                ACE_HANDLE handle,
                                                                                MessageBlock &message_block,
                                                                                u_long bytes_to_read,
                                                                                const void* act,
                                                                                ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                                int priority = 0,
                                                                                int signal_number = ACE_SIGRTMIN);

  /// Create the correct implementation class for AsynchWriteStream::Result.
  virtual AsynchWriteStreamResultImpl *create_asynch_write_stream_result (Handler &handler,
                                                                                  ACE_HANDLE handle,
                                                                                  MessageBlock &message_block,
                                                                                  u_long bytes_to_write,
                                                                                  const void* act,
                                                                                  ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                                  int priority = 0,
                                                                                  int signal_number = ACE_SIGRTMIN);

  /// Create the correct implementation class for AsynchReadFile::Result.
  virtual AsynchReadFileResultImpl *create_asynch_read_file_result (Handler &handler,
                                                                            ACE_HANDLE handle,
                                                                            MessageBlock &message_block,
                                                                            u_long bytes_to_read,
                                                                            const void* act,
                                                                            u_long offset,
                                                                            u_long offset_high,
                                                                            ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                            int priority = 0,
                                                                            int signal_number = ACE_SIGRTMIN);

  /// Create the correct implementation class for AsynchWriteFile::Result.
  virtual AsynchWriteFileResultImpl *create_asynch_write_file_result (Handler &handler,
                                                                              ACE_HANDLE handle,
                                                                              MessageBlock &message_block,
                                                                              u_long bytes_to_write,
                                                                              const void* act,
                                                                              u_long offset,
                                                                              u_long offset_high,
                                                                              ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                              int priority = 0,
                                                                              int signal_number = ACE_SIGRTMIN);


  /**
   * Create a timer result object which can be used with the Timer
   * mechanism of the Proactor.
   * If <signal_number> is -1, <POSIX_SIG_Proactor> will create a
   * Timer object with a meaningful signal number, choosing the
   * largest signal number from the signal mask of the Proactor.
   */
  virtual AsynchResultImpl *create_asynch_timer (Handler &handler,
                                                       const void *act,
                                                       const TimeValue &tv,
                                                       ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                       int priority = 0,
                                                       int signal_number = ACE_SIGRTMIN);

protected:

  /**
   * Post <how_many> completions to the completion port so that all
   * threads can wake up. This is used in conjunction with the
   * <run_event_loop>.
   */
  static int post_wakeup_completions (int how_many);

  /// Set the implementation class.
  virtual void implementation (ProactorImpl *implementation);

  /// Delegation/implementation class that all methods will be
  /// forwarded to.
  ProactorImpl *implementation_;

  /// Flag used to indicate whether we are responsible for cleaning up
  /// the implementation instance.
  int delete_implementation_;

  /// Pointer to a process-wide <Proactor>.
  static Proactor *proactor_;

  /// Must delete the <proactor_> if non-0.
  static int delete_proactor_;

  /// Handles timeout events.
  ACE_Proactor_Timer_Handler *timer_handler_;

  /// This will manage the thread in the Timer_Handler.
  ThreadManager thr_mgr_;

  /// Timer Queue.
  TIMER_QUEUE *timer_queue_;

  /// Flag on whether to delete the timer queue.
  int delete_timer_queue_;

  /// Terminate the proactor event loop.
  static sig_atomic_t end_event_loop_;

  /// Number of threads in the event loop.
  static sig_atomic_t event_loop_thread_count_;

private:
  /// Deny access since member-wise won't work...
  Proactor (const Proactor &);
  Proactor &operator= (const Proactor &);
};


//#endif /* ACE_WIN32 && !ACE_HAS_WINCE || ACE_HAS_AIO_CALLS*/

#endif /* ICC_PROACTOR_H */
