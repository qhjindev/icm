// WIN32_Proactor.cpp,v 4.19 2001/07/13 22:00:08 othman Exp

// ACE_RCSID(ace, Proactor, "WIN32_Proactor.cpp,v 4.19 2001/07/13 22:00:08 othman Exp")

#include "icc/Win32Proactor.h"

#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE))
// WIN implemenatation of the Proactor.

#include "icc/Log.h"
//#include "ace/Object_Manager.h"
#include "icc/AutoEvent.h"

class ACE_Export ACE_WIN32_Wakeup_Completion : public Win32AsynchResult
{
  // = TITLE
  //     This is result object is used by the <end_event_loop> of the
  //     Proactor interface to wake up all the threads blocking
  //     for completions.

public:
  ACE_WIN32_Wakeup_Completion (Handler &handler,
                               const void *act = 0,
                               ACE_HANDLE event = ACE_INVALID_HANDLE,
                               int priority = 0,
                               int signal_number = ACE_SIGRTMIN);
  // Constructor.

  virtual ~ACE_WIN32_Wakeup_Completion (void);
  // Destructor.


  virtual void complete (u_long bytes_transferred = 0,
                         int success = 1,
                         const void *completion_key = 0,
                         u_long error = 0);
  // This method calls the <handler>'s <handle_wakeup> method.
};

Win32Proactor::Win32Proactor (size_t number_of_threads,
                                        int used_with_reactor_event_loop)
  : completion_port_ (0),
    // This *MUST* be 0, *NOT* ACE_INVALID_HANDLE !!!
    number_of_threads_ (number_of_threads),
    used_with_reactor_event_loop_ (used_with_reactor_event_loop)
{
  // Create the completion port.
  this->completion_port_ = ::CreateIoCompletionPort (INVALID_HANDLE_VALUE,
                                                     0,
                                                     0,
                                                     this->number_of_threads_);
  if (this->completion_port_ == 0)
    ACE_ERROR ((LM_ERROR,
                ACE_LIB_TEXT ("%p\n"),
                ACE_LIB_TEXT ("CreateIoCompletionPort")));
}

Win32Proactor::~Win32Proactor (void)
{
  this->close ();
}

int
Win32Proactor::close (void)
{
  // Close the completion port
  if (this->completion_port_ != 0)
    {
      int result = OS::close (this->completion_port_);
      this->completion_port_ = 0;
      return result;
    }

  return 0;
}

int
Win32Proactor::register_handle (ACE_HANDLE handle,
                                     const void *completion_key)
{
  // No locking is needed here as no state changes.
  ACE_HANDLE cp = ::CreateIoCompletionPort (handle,
                                            this->completion_port_,
                                            (u_long) completion_key,
                                            this->number_of_threads_);
  if (cp == 0)
    {
      OS::set_errno_to_last_error ();
      // If errno == ERROR_INVALID_PARAMETER, then this handle was
      // already registered.
      if (errno != ERROR_INVALID_PARAMETER)
        {
//          if (ACE::debug ())
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_LIB_TEXT ("%p\n"),
                          ACE_LIB_TEXT ("CreateIoCompletionPort")));
            }
          return -1;
        }
    }
  return 0;
}

AsynchReadStreamImpl *
Win32Proactor::create_asynch_read_stream (void)
{
  AsynchReadStreamImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchReadStream (this),
                  0);
  return implementation;
}

AsynchWriteStreamImpl *
Win32Proactor::create_asynch_write_stream (void)
{
  AsynchWriteStreamImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchWriteStream (this),
                  0);
  return implementation;
}

AsynchReadFileImpl *
Win32Proactor::create_asynch_read_file (void)
{
  AsynchReadFileImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchReadFile (this),
                  0);
  return  implementation;
}

AsynchWriteFileImpl *
Win32Proactor::create_asynch_write_file (void)
{
  AsynchWriteFileImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchWriteFile (this),
                  0);
  return  implementation;
}

AsynchReadStreamResultImpl *
Win32Proactor::create_asynch_read_stream_result (Handler &handler,
                                                      ACE_HANDLE handle,
                                                      MessageBlock &message_block,
                                                      u_long bytes_to_read,
                                                      const void* act,
                                                      ACE_HANDLE event,
                                                      int priority,
                                                      int signal_number)
{
  AsynchReadStreamResultImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchReadStreamResult (handler,
                                                       handle,
                                                       message_block,
                                                       bytes_to_read,
                                                       act,
                                                       event,
                                                       priority,
                                                       signal_number),
                  0);
  return implementation;
}

AsynchWriteStreamResultImpl *
Win32Proactor::create_asynch_write_stream_result (Handler &handler,
                                                       ACE_HANDLE handle,
                                                       MessageBlock &message_block,
                                                       u_long bytes_to_write,
                                                       const void* act,
                                                       ACE_HANDLE event,
                                                       int priority,
                                                       int signal_number)
{
  AsynchWriteStreamResultImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchWriteStreamResult (handler,
                                                        handle,
                                                        message_block,
                                                        bytes_to_write,
                                                        act,
                                                        event,
                                                        priority,
                                                        signal_number),
                  0);
  return implementation;
}

AsynchReadFileResultImpl *
Win32Proactor::create_asynch_read_file_result (Handler &handler,
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
  AsynchReadFileResultImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchReadFileResult (handler,
                                                     handle,
                                                     message_block,
                                                     bytes_to_read,
                                                     act,
                                                     offset,
                                                     offset_high,
                                                     event,
                                                     priority,
                                                     signal_number),
                  0);
  return implementation;
}

AsynchWriteFileResultImpl *
Win32Proactor::create_asynch_write_file_result (Handler &handler,
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
  AsynchWriteFileResultImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchWriteFileResult (handler,
                                                      handle,
                                                      message_block,
                                                      bytes_to_write,
                                                      act,
                                                      offset,
                                                      offset_high,
                                                      event,
                                                      priority,
                                                      signal_number),
                  0);
  return implementation;
}

AsynchResultImpl *
Win32Proactor::create_asynch_timer (Handler &handler,
                                         const void *act,
                                         const TimeValue &tv,
                                         ACE_HANDLE event,
                                         int priority,
                                         int signal_number)
{
  AsynchResultImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  Win32AsynchTimer (handler,
                                          act,
                                          tv,
                                          event,
                                          priority,
                                          signal_number),
                  0);
  return implementation;
}

/*
int
Win32Proactor::handle_signal (int, siginfo_t *, ucontext_t *)
{
  // Perform a non-blocking "poll" for all the I/O events that have
  // completed in the I/O completion queue.

  int result = 0;

  for (TimeValue timeout (0, 0);
       ;
       )
    {
      result = this->handle_events (timeout);

      if (result != 0 || errno == ETIME)
        break;
    }

  // If our handle_events failed, we'll report a failure to the
  // Reactor.
  return result == -1 ? -1 : 0;
}
*/

int
Win32Proactor::handleClose (ACE_HANDLE handle,
                                  ReactorMask close_mask)
{
  ACE_UNUSED_ARG (close_mask);
  ACE_UNUSED_ARG (handle);

  return this->close ();
}

ACE_HANDLE
Win32Proactor::get_handle (void) const
{
  if (this->used_with_reactor_event_loop_)
    return this->event_.handle ();
  else
    return 0;
}

int
Win32Proactor::handle_events (TimeValue &wait_time)
{
  // Decrement <wait_time> with the amount of time spent in the method
//  ACE_Countdown_Time countdown (&wait_time);
  return this->handle_events (wait_time.msec ());
}

int
Win32Proactor::handle_events (void)
{
  return this->handle_events (INFINITE);
}

int
Win32Proactor::handle_events (unsigned long milli_seconds)
{
  ACE_OVERLAPPED *overlapped = 0;
  u_long bytes_transferred = 0;
  u_long completion_key = 0;

  // Get the next asynchronous operation that completes
  BOOL result = ::GetQueuedCompletionStatus (this->completion_port_,
                                             &bytes_transferred,
                                             &completion_key,
                                             &overlapped,
                                             milli_seconds);
  if (result == FALSE && overlapped == 0)
    {
      OS::set_errno_to_last_error ();

      if (errno == WAIT_TIMEOUT)
        {
          errno = ETIME;
          return 0;
        }
      else
        {
//          if (ACE::debug ())
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_LIB_TEXT ("%p\n"),
                          ACE_LIB_TEXT ("GetQueuedCompletionStatus")));
            }
          return -1;
        }
    }
  else
    {
      // Narrow the result.
      Win32AsynchResult *asynch_result = (Win32AsynchResult *) overlapped;

      // If errors happen, grab the error.
      if (result == FALSE)
        OS::set_errno_to_last_error ();
      else
        errno = 0;

      u_long result_err = asynch_result->error ();

      // if "result_err" is 0 than
      //     It is normal OS/WIN32 AIO completion.
      //     We have cleared asynch_result->error_
      //     during shared_read/shared_write.
      //     The real error code is already stored in "errno",
      //     so copy "errno" value to the "result_err"
      //     and pass this "result_err" code 
      //     to the application_specific_code ()
      // else 
      //    "result_err" non zero
      //     it means we have "post_completed" result
      //     so pass this "result_err" code 
      //     to the application_specific_code ()

      if ( result_err == 0 ) 
        result_err = errno ;

      this->application_specific_code (asynch_result,
                                       bytes_transferred,
                                       result,
                                       (void *) completion_key,
                                       result_err);
    }
  return 1;
}

void
Win32Proactor::application_specific_code (Win32AsynchResult *asynch_result,
                                               u_long bytes_transferred,
                                               int success,
                                               const void *completion_key,
                                               u_long error)
{
  ACE_SEH_TRY
    {
      // Call completion hook
      asynch_result->complete (bytes_transferred,
                               success,
                               (void *) completion_key,
                               error);
    }
  ACE_SEH_FINALLY
    {
      // This is crucial to prevent memory leaks
      delete asynch_result;
    }
}

int
Win32Proactor::post_completion (Win32AsynchResult *result)
{
  // Grab the event associated with the Proactor
  HANDLE handle = this->get_handle ();

  // If Proactor event is valid, signal it
  if (handle != ACE_INVALID_HANDLE &&
      handle != 0)
    OS::event_signal (&handle);

  // pass 
  //   bytes_transferred
  //   completion_key 
  // to the ::PostQueuedCompletionStatus()
  //   error will be extracted later in handle_events()

  u_long bytes_transferred = 0;
  const void * completion_key = 0 ;

  if ( result != 0 )
    {
      bytes_transferred = result->bytes_transferred ();
      completion_key = result->completion_key();
    }

  // Post a completion
  if (::PostQueuedCompletionStatus (this->completion_port_, // completion port
                                    bytes_transferred,      // number of bytes transferred
                                    (ULONG) completion_key, // completion key
                                    result                  // overlapped
                                    ) == FALSE)
    {
      delete result;

//      if (ACE::debug ())
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_LIB_TEXT ("%p\n"),
                      ACE_LIB_TEXT ("PostQueuedCompletionStatus failed")));
        }
      return -1;
    }

  return 0;
}

int
Win32Proactor::post_wakeup_completions (int how_many)
{
  ACE_WIN32_Wakeup_Completion *wakeup_completion = 0;

  for (ssize_t ci = 0; ci < how_many; ci++)
    {
      ACE_NEW_RETURN (wakeup_completion,
                      ACE_WIN32_Wakeup_Completion (this->wakeup_handler_),
                      -1);

      if (wakeup_completion->post_completion (this) == -1)
        return -1;
    }

  return 0;
}

int
Win32Proactor::wake_up_dispatch_threads (void)
{
  return 0;
}

int
Win32Proactor::close_dispatch_threads (int)
{
  return 0;
}

size_t
Win32Proactor::number_of_threads (void) const
{
  return this->number_of_threads_;
}

void
Win32Proactor::number_of_threads (size_t threads)
{
  this->number_of_threads_ = threads;
}

Win32AsynchTimer::Win32AsynchTimer (Handler &handler,
                                                const void *act,
                                                const TimeValue &tv,
                                                ACE_HANDLE event,
                                                int priority,
                                                int signal_number)
  : AsynchResultImpl (),
    Win32AsynchResult (handler, act, event, 0, 0, priority,
                             signal_number),
    time_ (tv)
{
}

void
Win32AsynchTimer::complete (u_long bytes_transferred,
                                  int success,
                                  const void *completion_key,
                                  u_long error)
{
  ACE_UNUSED_ARG (error);
  ACE_UNUSED_ARG (completion_key);
  ACE_UNUSED_ARG (success);
  ACE_UNUSED_ARG (bytes_transferred);

  this->handler_.handle_time_out (this->time_, this->act ());
}

ACE_WIN32_Wakeup_Completion::ACE_WIN32_Wakeup_Completion (Handler &handler,
                                                          const void *act,
                                                          ACE_HANDLE event,
                                                          int priority,
                                                          int signal_number)
  : AsynchResultImpl (),
    Win32AsynchResult (handler, act, event, 0, 0, priority, signal_number)
{
}

ACE_WIN32_Wakeup_Completion::~ACE_WIN32_Wakeup_Completion (void)
{
}

void
ACE_WIN32_Wakeup_Completion::complete (u_long       /* bytes_transferred */,
                                       int          /* success */,
                                       const void * /* completion_key */,
                                       u_long       /*  error */)
{
  this->handler_.handle_wakeup ();
}

#endif /* ACE_WIN32 */
