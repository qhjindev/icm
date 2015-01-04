// Proactor.cpp,v 4.111 2001/07/02 01:03:12 schmidt Exp


#include "icc/Proactor.h"
#include "icc/ProactorImpl.h"
//#include "icc/Object_Manager.h"
#include "icc/Task.h"
#include "icc/AutoEvent.h"

//#if ((defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS)))
// This only works on Win32 platforms and on Unix platforms with aio
// calls.
#include "icc/Task.h"
#include "icc/Log.h"

#if defined (ACE_HAS_AIO_CALLS)
//comment by qinghua
//#   include "icc/PosixProactor.h"
#else /* !ACE_HAS_AIO_CALLS */
#   include "icc/Win32Proactor.h"
#endif /* ACE_HAS_AIO_CALLS */

#if defined (ACE_HAS_SIG_C_FUNC)
extern "C" void
ACE_Proactor_cleanup (void *instance, void *arg)
{
  Proactor::cleanup (instance, arg);
}
#endif

void
Proactor::cleanup (void *, void *)
{
  Proactor::close_singleton ();
}

// Process-wide Proactor.
Proactor *Proactor::proactor_ = 0;

// Controls whether the Proactor is deleted when we shut down (we can
// only delete it safely if we created it!)
int Proactor::delete_proactor_ = 0;

// Terminate the eventloop.
sig_atomic_t Proactor::end_event_loop_ = 0;

// Number of threads in the event loop.
sig_atomic_t Proactor::event_loop_thread_count_ = 0;

class ACE_Export ACE_Proactor_Timer_Handler : public Task <NULL_SYNCH>
{
  // = TITLE
  //     A Handler for timer. It helps in the management of timers
  //     registered with the Proactor.
  //
  // = DESCRIPTION
  //     This object has a thread that will wait on the earliest time
  //     in a list of timers and an event. When a timer expires, the
  //     thread will post a completion event on the port and go back
  //     to waiting on the timer queue and event. If the event is
  //     signaled, the thread will refresh the time it is currently
  //     waiting on (in case the earliest time has changed).

  friend class Proactor;
  // Proactor has special privileges
  // Access needed to: timer_event_

public:
  ACE_Proactor_Timer_Handler (Proactor &proactor);
  // Constructor.

  virtual ~ACE_Proactor_Timer_Handler (void);
  // Destructor.

  int destroy (void);
  // Proactor calls this to shut down the timer handler
  // gracefully. Just calling the destructor alone doesnt do what
  // <destroy> does. <destroy> make sure the thread exits properly.

protected:
  virtual int svc (void);
  // Run by a daemon thread to handle deferred processing. In other
  // words, this method will do the waiting on the earliest timer and
  // event.

  ACE_Auto_Event timer_event_;
  // Event to wait on.

  Proactor &proactor_;
  // Proactor.

  int shutting_down_;
  // Flag used to indicate when we are shutting down.
};

ACE_Proactor_Timer_Handler::ACE_Proactor_Timer_Handler (Proactor &proactor)
  : Task <NULL_SYNCH> (&proactor.thr_mgr_),
    proactor_ (proactor),
    shutting_down_ (0)
{
}

ACE_Proactor_Timer_Handler::~ACE_Proactor_Timer_Handler (void)
{
  // Mark for closing down.
  this->shutting_down_ = 1;

  // Signal timer event.
  this->timer_event_.signal ();

  // Wait for the Timer Handler thread to exit.
  //this->thr_mgr ()->wait_grp (this->grp_id ());
}

int
ACE_Proactor_Timer_Handler::svc (void)
{
  TimeValue absolute_time;
  int empty_flag = 0;
  int result = 0;

  while (this->shutting_down_ == 0)
    {
      // Is the timer queue empty?
      empty_flag = this->proactor_.timer_queue ()->isEmpty ();

      if (!empty_flag)
        {
          // Get the earliest absolute time.
          absolute_time = this->proactor_.timer_queue ()->earliestTime ();

          // Block for absolute time.
          result = this->timer_event_.wait (&absolute_time);
        }
      else
        {
          // Wait for ever.
          result = this->timer_event_.wait ();
        }

      // Check for timer expiries.
      if (result == -1)
        {
          switch (errno)
            {
            case ETIME:
              // timeout: expire timers
              this->proactor_.timer_queue ()->expire ();
              break;
            default:
              // Error.
              ACE_ERROR_RETURN ((LM_ERROR,
                                 ACE_LIB_TEXT ("%N:%l:(%P | %t):%p\n"),
                                 ACE_LIB_TEXT ("ACE_Proactor_Timer_Handler::svc:wait failed")),
                                 -1);
            }
        }
    }
  return 0;
}

// *********************************************************************

ProactorHandleTimeoutUpcall::ProactorHandleTimeoutUpcall (void)
  : proactor_ (0)
{
}

int
ProactorHandleTimeoutUpcall::timeout (TIMER_QUEUE &timer_queue,
                                             Handler *handler,
                                             const void *act,
                                             int recurringTimer,
                                             const TimeValue &time)
{
  ACE_UNUSED_ARG (timer_queue);

  if (this->proactor_ == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("(%t) No Proactor set in ProactorHandleTimeoutUpcall,")
                       ACE_LIB_TEXT (" no completion port to post timeout to?!@\n")),
                      -1);

  // Create the Asynch_Timer.
  AsynchResultImpl *asynch_timer = this->proactor_->create_asynch_timer (*handler,
                                                                               act,
                                                                               time,
                                                                               ACE_INVALID_HANDLE,
                                                                               0,
                                                                               -1);
  if (asynch_timer == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l:(%P | %t):%p\n"),
                       ACE_LIB_TEXT ("ProactorHandleTimeoutUpcall::timeout:")
                       ACE_LIB_TEXT ("create_asynch_timer failed")),
                      -1);

  // Post a completion.
  if (asynch_timer->post_completion (this->proactor_->implementation ()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("Failure in dealing with timers: ")
                       ACE_LIB_TEXT ("PostQueuedCompletionStatus failed\n")),
                      -1);
  return 0;
}

int
ProactorHandleTimeoutUpcall::cancellation (TIMER_QUEUE &,
                                                  Handler *,
                                                  int )
{
//  ACE_UNUSED_ARG (timer_queue);
//  ACE_UNUSED_ARG (handler);

  // Do nothing
  return 0;
}

int
ProactorHandleTimeoutUpcall::deletion (TIMER_QUEUE &,
                                              Handler *,
                                              const void *)
{
//  ACE_UNUSED_ARG (timer_queue);
//  ACE_UNUSED_ARG (handler);
//  ACE_UNUSED_ARG (arg);

  // Do nothing
  return 0;
}

int
ProactorHandleTimeoutUpcall::proactor (Proactor &proactor)
{
  if (this->proactor_ == 0)
    {
      this->proactor_ = &proactor;
      return 0;
    }
  else
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("ProactorHandleTimeoutUpcall is only suppose")
                       ACE_LIB_TEXT (" to be used with ONE (and only one) Proactor\n")),
                      -1);
}

// *********************************************************************

Proactor::Proactor (ProactorImpl *implementation,
                            int delete_implementation,
                            TIMER_QUEUE *tq)
  : implementation_ (0),
    delete_implementation_ (delete_implementation),
    timer_handler_ (0),
    timer_queue_ (0),
    delete_timer_queue_ (0)
{
  this->implementation (implementation);

  if (this->implementation () == 0)
    {
#if defined (ACE_HAS_AIO_CALLS)
      // POSIX Proactor.

   //implementation = new ACE_POSIX_SIG_Proactor; comment by qinghua

  #if defined (ACE_POSIX_AIOCB_PROACTOR)
      // comment by robert , cannot compile on hpux
      //ACE_NEW (implementation,
               //ACE_POSIX_AIOCB_Proactor);
  #elif defined (ACE_POSIX_SIG_PROACTOR)
      //ACE_NEW (implementation,
        //       ACE_POSIX_SIG_Proactor);
  #else /* Default is to use the SIG one */
    // comment by robert , cannot compile on aix
//      ACE_NEW (implementation,
//               ACE_POSIX_SIG_Proactor);
  #endif
#elif (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE))
      // WIN_Proactor.
      ACE_NEW (implementation,
               Win32Proactor);
#endif /* ACE_HAS_AIO_CALLS */
      this->implementation (implementation);
      this->delete_implementation_ = 1;
    }

  // Set the timer queue.
  this->timer_queue (tq);

  // Create the timer handler
  ACE_NEW (this->timer_handler_,
           ACE_Proactor_Timer_Handler (*this));

  // Activate <timer_handler>.
  if (this->timer_handler_->activate () == -1)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%N:%l:(%P | %t):%p\n"),
                ACE_LIB_TEXT ("Task::activate:could not create thread\n")));
}

Proactor::~Proactor (void)
{
  this->close ();
}

Proactor *
Proactor::instance (size_t /* threads */)
{
  ACE_TRACE ("Proactor::instance");

  if (Proactor::proactor_ == 0)
    {
      // Perform Double-Checked Locking Optimization.
/*      ACE_MT (ACE_GUARD_RETURN (RecursiveThread_Mutex, ace_mon,
                                *ACE_Static_Object_Lock::instance (),
                                0));
*/
      if (Proactor::proactor_ == 0)
        {
          ACE_NEW_RETURN (Proactor::proactor_,
                          Proactor,
                          0);

          Proactor::delete_proactor_ = 1;

          // Register with the Object_Manager so that the wrapper to
          // delete the proactor will be called when Object_Manager is
          // being terminated.
/*
#if defined ACE_HAS_SIG_C_FUNC
          ACE_Object_Manager::at_exit (Proactor::proactor_,
                                       ACE_Proactor_cleanup,
                                       0);
#else
          ACE_Object_Manager::at_exit (Proactor::proactor_,
                                       Proactor::cleanup,
                                       0);
#endif */

        }
    }
  return Proactor::proactor_;
}

Proactor *
Proactor::instance (Proactor *r)
{
  ACE_TRACE ("Proactor::instance");

  //ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon,
    //                        *ACE_Static_Object_Lock::instance (), 0));

  Proactor *t = Proactor::proactor_;

  // We can't safely delete it since we don't know who created it!
  Proactor::delete_proactor_ = 0;

  // Register with the Object_Manager so that the wrapper to
  // delete the proactor will be called when Object_Manager is
  // being terminated.
/*
#if defined ACE_HAS_SIG_C_FUNC
  ACE_Object_Manager::at_exit (Proactor::proactor_,
                               ACE_Proactor_cleanup,
                               0);
#else
  ACE_Object_Manager::at_exit (Proactor::proactor_,
                               Proactor::cleanup,
                               0);
#endif */

  Proactor::proactor_ = r;
  return t;
}

void
Proactor::close_singleton (void)
{
  ACE_TRACE ("Proactor::close_singleton");

  //ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon,
  //                   *ACE_Static_Object_Lock::instance ()));

  if (Proactor::delete_proactor_)
    {
      delete Proactor::proactor_;
      Proactor::proactor_ = 0;
      Proactor::delete_proactor_ = 0;
    }
}

int
Proactor::run_event_loop (void)
{
  int result = 0;
/*
  // Declaring the lock variable.
#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
  ACE_Thread_Mutex *lock =
    ACE_Managed_Object<ACE_Thread_Mutex>::get_preallocated_object
    (ACE_Object_Manager::ACE_PROACTOR_EVENT_LOOP_LOCK);
#endif  ACE_MT_SAFE */

  // Early check. It is ok to do this without lock, since we care just
  // whether it is zero or non-zero.
  if (Proactor::end_event_loop_ != 0)
    return 0;

  // First time you are in. Increment the thread count.
  {
    // Obtain the lock in the MT environments.
/*#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
    ACE_GUARD_RETURN (ACE_Thread_Mutex, ace_mon, *lock, -1);
#endif  ACE_MT_SAFE */

    // Increment the thread count.
    Proactor::event_loop_thread_count_ ++;
  }

  // Run the event loop.
  for (;;)
    {
      // Check the end loop flag. It is ok to do this without lock,
      // since we care just whether it is zero or non-zero.
      if (Proactor::end_event_loop_ != 0)
        break;

      // <end_event_loop> is not set. Ready to do <handle_events>.
      result = Proactor::instance ()->handle_events ();

      //if (ACE_Service_Config::reconfig_occurred ())
      //  ACE_Service_Config::reconfigure ();

      //else
        if (result == -1)
        break;
    }

  // Leaving the event loop. Decrement the thread count.

  // Obtain the lock in the MT environments.
//#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
 // ACE_GUARD_RETURN (ACE_Thread_Mutex, ace_mon, *lock, -1);
//#endif ACE_MT_SAFE */

  // Decrement the thread count.
  Proactor::event_loop_thread_count_ --;

  if (Proactor::event_loop_thread_count_ > 0
      && Proactor::end_event_loop_ != 0)
    Proactor::post_wakeup_completions (1);

  return result;
}

// Handle events for -tv- time.  handle_events updates -tv- to reflect
// time elapsed, so do not return until -tv- == 0, or an error occurs.
int
Proactor::run_event_loop (TimeValue &tv)
{
  ACE_TRACE ("Proactor::run_event_loop");

  int result = 0;

  // Declaring the lock variable.
/*#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
  ACE_Thread_Mutex *lock =
    ACE_Managed_Object<ACE_Thread_Mutex>::get_preallocated_object
    (ACE_Object_Manager::ACE_PROACTOR_EVENT_LOOP_LOCK);
#endif  ACE_MT_SAFE */

  // Early check. It is ok to do this without lock, since we care just
  // whether it is zero or non-zero.
  if (Proactor::end_event_loop_ != 0
      || tv == TimeValue::zero)
    return 0;

  // First time you are in. Increment the thread count.
  {
    // Obtain the lock in the MT environments.
/*#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
    ACE_GUARD_RETURN (ACE_Thread_Mutex, ace_mon, *lock, -1);
#endif ACE_MT_SAFE */

    // Increment the thread count.
    Proactor::event_loop_thread_count_ ++;
  }

  // Run the event loop.
  for (;;)
    {
      // Check for end of loop. It is ok to do this without lock,
      // since we care just whether it is zero or non-zero.
      if (Proactor::end_event_loop_ != 0
          || tv == TimeValue::zero)
        break;

      // <end_event_loop> is not set. Ready to do <handle_events>.
      result = Proactor::instance ()->handle_events (tv);

      //if (ACE_Service_Config::reconfig_occurred ())
        //ACE_Service_Config::reconfigure ();

      // An error has occurred.
      //else
      if (result == -1)
        break;
    }

  // Leaving the event loop. Decrement the thread count.

  // Obtain the lock in the MT environments.
/*#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
  ACE_GUARD_RETURN (ACE_Thread_Mutex, ace_mon, *lock, -1);
#endif ACE_MT_SAFE */

  // Decrement the thread count.
  Proactor::event_loop_thread_count_ --;

  if (Proactor::event_loop_thread_count_  > 0
      && Proactor::end_event_loop_ != 0)
    Proactor::post_wakeup_completions (1);

  return result;
}

int
Proactor::reset_event_loop(void)
{
  ACE_TRACE ("Proactor::reset_event_loop");

  // Obtain the lock in MT environments.
/*#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE !=0)
  ACE_Thread_Mutex *lock =
	  ACE_Managed_Object<ACE_Thread_Mutex>::get_preallocated_object
	  (ACE_Object_Manager::ACE_PROACTOR_EVENT_LOOP_LOCK);
  ACE_GUARD_RETURN (ACE_Thread_Mutex, ace_mon, *lock, -1);
#endif
  */
  Proactor::end_event_loop_ = 0;
  return 0;
}

int
Proactor::end_event_loop (void)
{
  ACE_TRACE ("Proactor::end_event_loop");

  // Obtain the lock, set the end flag and post the wakeup
  // completions.

  // Obtain the lock in the MT environments.
/*#if defined (ACE_MT_SAFE) && (ACE_MT_SAFE != 0)
  ACE_Thread_Mutex *lock =
    ACE_Managed_Object<ACE_Thread_Mutex>::get_preallocated_object
    (ACE_Object_Manager::ACE_PROACTOR_EVENT_LOOP_LOCK);
  ACE_GUARD_RETURN (ACE_Thread_Mutex, ace_mon, *lock, -1);
#endif  ACE_MT_SAFE */

  // Set the end flag.
  Proactor::end_event_loop_ = 1;

  // Number of completions to post.
  int how_many = Proactor::event_loop_thread_count_;

  if (how_many == 0)
    return 0;

  // Reset the thread count.
  // Proactor::event_loop_thread_count_ = 0;

  // Post completions to all the threads so that they will all wake
  // up.
  return Proactor::post_wakeup_completions (how_many);
}

int
Proactor::event_loop_done (void)
{
  ACE_TRACE ("Proactor::event_loop_done");
  return Proactor::end_event_loop_ != 0;
}

int
Proactor::close (void)
{
  // Close the implementation.
  if (this->implementation ()->close () == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("%N:%l:(%P | %t):%p\n"),
                       ACE_LIB_TEXT ("Proactor::close:implementation couldnt be closed")),
                      -1);

  // Delete the implementation.
  if (this->delete_implementation_)
    {
      delete this->implementation ();
      this->implementation_ = 0;
    }

  // Delete the timer handler.
  if (this->timer_handler_)
    {
      delete this->timer_handler_;
      this->timer_handler_ = 0;
    }

  // Delete the timer queue.
  if (this->delete_timer_queue_)
    {
      delete this->timer_queue_;
      this->timer_queue_ = 0;
      this->delete_timer_queue_ = 0;
    }

  return 0;
}

int
Proactor::register_handle (ACE_HANDLE handle,
                               const void *completion_key)
{
  return this->implementation ()->register_handle (handle,
                                                   completion_key);
}

long
Proactor::schedule_timer (Handler &handler,
                              const void *act,
                              const TimeValue &time)
{
  return this->schedule_timer (handler,
                               act,
                               time,
                               TimeValue::zero);
}

long
Proactor::schedule_repeating_timer (Handler &handler,
                                        const void *act,
                                        const TimeValue &interval)
{
  return this->schedule_timer (handler,
                               act,
                               interval,
                               interval);
}

long
Proactor::schedule_timer (Handler &handler,
                              const void *act,
                              const TimeValue &time,
                              const TimeValue &interval)
{
  // absolute time.
  TimeValue absolute_time =
    OS::gettimeofday () + time;

  // Only one guy goes in here at a time
  ACE_GUARD_RETURN (SYNCH_RECURSIVE_MUTEX,
                    ace_mon,
                    this->timer_queue_->mutex (),
                    -1);

  // Schedule the timer
  long result = this->timer_queue_->schedule (&handler,
                                              act,
                                              absolute_time,
                                              interval);
  if (result != -1)
    {
      // no failures: check to see if we are the earliest time
      if (this->timer_queue_->earliestTime () == absolute_time)

        // wake up the timer thread
        if (this->timer_handler_->timer_event_.signal () == -1)
          {
            // Cancel timer
            this->timer_queue_->cancel (result);
            result = -1;
          }
    }
  return result;
}

int
Proactor::cancel_timer (long timer_id,
                            const void **arg,
                            int dont_call_handle_close)
{
  // No need to singal timer event here. Even if the cancel timer was
  // the earliest, we will have an extra wakeup.
  return this->timer_queue_->cancel (timer_id,
                                     arg,
                                     dont_call_handle_close);
}

int
Proactor::cancel_timer (Handler &handler,
                                  int dont_call_handle_close)
{
  // No need to signal timer event here. Even if the cancel timer was
  // the earliest, we will have an extra wakeup.
  return this->timer_queue_->cancel (&handler,
                                     dont_call_handle_close);
}

int
Proactor::handle_events (TimeValue &wait_time)
{
  return implementation ()->handle_events (wait_time);
}

int
Proactor::handle_events (void)
{
  return this->implementation ()->handle_events ();
}

int
Proactor::wake_up_dispatch_threads (void)
{
  return 0;
}

int
Proactor::close_dispatch_threads (int)
{
  return 0;
}

size_t
Proactor::number_of_threads (void) const
{
  return this->implementation ()->number_of_threads ();
}

void
Proactor::number_of_threads (size_t threads)
{
  this->implementation ()->number_of_threads (threads);
}

Proactor::TIMER_QUEUE *
Proactor::timer_queue (void) const
{
  return this->timer_queue_;
}

void
Proactor::timer_queue (TIMER_QUEUE *tq)
{
  // Cleanup old timer queue.
  if (this->delete_timer_queue_)
    {
      delete this->timer_queue_;
      this->delete_timer_queue_ = 0;
    }

  // New timer queue.
  if (tq == 0)
    {
      ACE_NEW (this->timer_queue_,
               TIMER_HEAP);
      this->delete_timer_queue_ = 1;
    }
  else
    {
      this->timer_queue_ = tq;
      this->delete_timer_queue_ = 0;
    }

  // Set the proactor in the timer queue's functor
  this->timer_queue_->upcallFunctor ().proactor (*this);
}

ACE_HANDLE
Proactor::get_handle (void) const
{
  return this->implementation ()->get_handle ();
}

ProactorImpl *
Proactor::implementation (void) const
{
  return this->implementation_;
}


AsynchReadStreamImpl *
Proactor::create_asynch_read_stream (void)
{
  return this->implementation ()->create_asynch_read_stream ();
}

AsynchWriteStreamImpl *
Proactor::create_asynch_write_stream (void)
{
  return this->implementation ()->create_asynch_write_stream ();
}

AsynchReadFileImpl *
Proactor::create_asynch_read_file (void)
{
  return this->implementation ()->create_asynch_read_file ();
}

AsynchWriteFileImpl *
Proactor::create_asynch_write_file (void)
{
  return this->implementation ()->create_asynch_write_file ();
}

AsynchReadStreamResultImpl *
Proactor::create_asynch_read_stream_result (Handler &handler,
                                                ACE_HANDLE handle,
                                                MessageBlock &message_block,
                                                u_long bytes_to_read,
                                                const void* act,
                                                ACE_HANDLE event,
                                                int priority,
                                                int signal_number)
{
  return this->implementation ()->create_asynch_read_stream_result (handler,
                                                                    handle,
                                                                    message_block,
                                                                    bytes_to_read,
                                                                    act,
                                                                    event,
                                                                    priority,
                                                                    signal_number);
}


AsynchWriteStreamResultImpl *
Proactor::create_asynch_write_stream_result (Handler &handler,
                                                 ACE_HANDLE handle,
                                                 MessageBlock &message_block,
                                                 u_long bytes_to_write,
                                                 const void* act,
                                                 ACE_HANDLE event,
                                                 int priority,
                                                 int signal_number)

{
  return this->implementation ()->create_asynch_write_stream_result (handler,
                                                                     handle,
                                                                     message_block,
                                                                     bytes_to_write,
                                                                     act,
                                                                     event,
                                                                     priority,
                                                                     signal_number);
}

AsynchReadFileResultImpl *
Proactor::create_asynch_read_file_result (Handler &handler,
                                              ACE_HANDLE handle,
                                              MessageBlock &message_block,
                                              u_long bytes_to_read,
                                              const void* act,
                                              u_long offset,
                                              u_long offset_high,
                                              ACE_HANDLE event,
                                              int priority,
                                              int signal_number)

{
  return this->implementation ()->create_asynch_read_file_result (handler,
                                                                  handle,
                                                                  message_block,
                                                                  bytes_to_read,
                                                                  act,
                                                                  offset,
                                                                  offset_high,
                                                                  event,
                                                                  priority,
                                                                  signal_number);
}

AsynchWriteFileResultImpl *
Proactor::create_asynch_write_file_result (Handler &handler,
                                               ACE_HANDLE handle,
                                               MessageBlock &message_block,
                                               u_long bytes_to_write,
                                               const void* act,
                                               u_long offset,
                                               u_long offset_high,
                                               ACE_HANDLE event,
                                               int priority,
                                               int signal_number)

{
  return this->implementation ()->create_asynch_write_file_result (handler,
                                                                   handle,
                                                                   message_block,
                                                                   bytes_to_write,
                                                                   act,
                                                                   offset,
                                                                   offset_high,
                                                                   event,
                                                                   priority,
                                                                   signal_number);
}

AsynchResultImpl *
Proactor::create_asynch_timer (Handler &handler,
                                   const void *act,
                                   const TimeValue &tv,
                                   ACE_HANDLE event,
                                   int priority,
                                   int signal_number)
{
  return this->implementation ()->create_asynch_timer (handler,
                                                       act,
                                                       tv,
                                                       event,
                                                       priority,
                                                       signal_number);
}

int
Proactor::post_wakeup_completions (int how_many)
{
  return Proactor::instance ()->implementation ()->post_wakeup_completions (how_many);
}

void
Proactor::implementation (ProactorImpl *implementation)
{
  this->implementation_ = implementation;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Timer_Queue_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_Queue_Iterator_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_List_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_List_Iterator_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_Node_T<Handler *>;
template class ACE_Unbounded_Set<ACE_Timer_Node_T<Handler *> *>;
template class ACE_Unbounded_Set_Iterator<ACE_Timer_Node_T<Handler *> *>;
template class ACE_Node <ACE_Timer_Node_T<Handler *> *>;
template class ACE_Free_List<ACE_Timer_Node_T<Handler *> >;
template class ACE_Locked_Free_List<ACE_Timer_Node_T<Handler *>, ACE_Null_Mutex>;
template class ACE_Timer_Heap_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_Heap_Iterator_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_Wheel_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
template class ACE_Timer_Wheel_Iterator_T<Handler *,
  ProactorHandleTimeoutUpcall,
  SYNCH_RECURSIVE_MUTEX>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Timer_Queue_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_Queue_Iterator_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_List_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_List_Iterator_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_Node_T<Handler *>
#pragma instantiate ACE_Unbounded_Set<ACE_Timer_Node_T<Handler *> *>
#pragma instantiate ACE_Unbounded_Set_Iterator<ACE_Timer_Node_T<Handler *> *>
#pragma instantiate ACE_Node <ACE_Timer_Node_T<Handler *> *>
#pragma instantiate ACE_Free_List<ACE_Timer_Node_T<Handler *> >
#pragma instantiate ACE_Locked_Free_List<ACE_Timer_Node_T<Handler *>,\
   ACE_Null_Mutex>
#pragma instantiate ACE_Timer_Heap_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_Heap_Iterator_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_Wheel_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Timer_Wheel_Iterator_T<Handler *,\
  ProactorHandleTimeoutUpcall,\
  SYNCH_RECURSIVE_MUTEX>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

//#else /* !ACE_WIN32 || !ACE_HAS_AIO_CALLS */

/*
Proactor *
Proactor::instance (size_t threads)
{
  ACE_UNUSED_ARG (threads);
  return 0;
}

Proactor *
Proactor::instance (Proactor *)
{
  return 0;
}

void
Proactor::close_singleton (void)
{
}

int
Proactor::run_event_loop (void)
{
  // not implemented
  return -1;
}

int
Proactor::run_event_loop (TimeValue &tv)
{
  // not implemented
  ACE_UNUSED_ARG (tv);
  return -1;
}

int
Proactor::end_event_loop (void)
{
  // not implemented
  return -1;
}

sig_atomic_t
Proactor::event_loop_done (void)
{
  return sig_atomic_t (1);
}
*/

//#endif /* ACE_WIN32 || ACE_HAS_AIO_CALLS*/
