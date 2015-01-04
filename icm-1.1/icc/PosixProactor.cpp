/* -*- C++ -*- */
// POSIX_Proactor.cpp,v 4.38 2001/08/16 18:07:12 schmidt Exp

#define ACE_BUILD_DLL
#include "icc/PosixProactor.h"

//#if defined (ACE_HAS_AIO_CALLS)

#include "os/ACE.h"
#include "os/Flag_Manip.h"
#include "icc/Task.h"
#include "icc/Log.h"
//#include "ace/Object_Manager.h"


# if defined (ACE_HAS_SYSINFO)
#   include  <sys/systeminfo.h>
# endif /* ACE_HAS_SYS_INFO */

class ACE_Export ACE_POSIX_Wakeup_Completion : public PosixAsynchResult
{
  // = TITLE
  //     This is result object is used by the <end_event_loop> of the
  //     ACE_Proactor interface to wake up all the threads blocking
  //     for completions.
public:
  ACE_POSIX_Wakeup_Completion (ACE_Handler &handler,
                               const void *act = 0,
                               ACE_HANDLE event = ACE_INVALID_HANDLE,
                               int priority = 0,
                               int signal_number = ACE_SIGRTMIN);
  // Constructor.

  virtual ~ACE_POSIX_Wakeup_Completion (void);
  // Destructor.


  virtual void complete (u_long bytes_transferred = 0,
                         int success = 1,
                         const void *completion_key = 0,
                         u_long error = 0);
  // This method calls the <handler>'s <handle_wakeup> method.
};

// *********************************************************************
PosixProactor::PosixProactor (void)
  :  os_id_ (OS_UNDEFINED)
{
#if defined(sun)

  os_id_ = OS_SUN; // set family

  char Buf [32];

  ::memset(Buf,0,sizeof(Buf));

  OS::sysinfo (SI_RELEASE , Buf, sizeof(Buf)-1);

  if (OS_String::strcasecmp (Buf , "5.6") == 0)
    os_id_ = OS_SUN_56;
  else if (OS_String::strcasecmp (Buf , "5.7") == 0)
    os_id_ = OS_SUN_57;
  else if (OS_String::strcasecmp (Buf , "5.8") == 0)
    os_id_ = OS_SUN_58;

#elif defined(HPUX)

  os_id_ = OS_HPUX;   // set family
  // do the same

//#else defined (LINUX, __FreeBSD__ ...)
//setup here os_id_
#endif
}

PosixProactor::~PosixProactor (void)
{
  this->close ();
}

int
PosixProactor::close (void)
{
  return 0;
}

int
PosixProactor::register_handle (ACE_HANDLE handle,
                                     const void *completion_key)
{
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (completion_key);
  return 0;
}

int
PosixProactor::wake_up_dispatch_threads (void)
{
  return 0;
}

int
PosixProactor::close_dispatch_threads (int)
{
  return 0;
}

size_t
PosixProactor::number_of_threads (void) const
{
  // @@ Implement it.
  ACE_NOTSUP_RETURN (0);
}

void
PosixProactor::number_of_threads (size_t threads)
{
  // @@ Implement it.
  ACE_UNUSED_ARG (threads);
}

ACE_HANDLE
PosixProactor::get_handle (void) const
{
  return ACE_INVALID_HANDLE;
}

AsynchReadStreamResultImpl *
PosixProactor::create_asynch_read_stream_result (ACE_Handler &handler,
                                                      ACE_HANDLE handle,
                                                      MessageBlock &message_block,
                                                      u_long bytes_to_read,
                                                      const void* act,
                                                      ACE_HANDLE event,
                                                      int priority,
                                                      int signal_number)
{
  AsynchReadStreamResultImpl *implementation;
  ACE_NEW_RETURN (implementation,
                  PosixAsynchReadStreamResult (handler,
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
PosixProactor::create_asynch_write_stream_result (ACE_Handler &handler,
                                                       ACE_HANDLE handle,
                                                       MessageBlock &message_block,
                                                       u_long bytes_to_write,
                                                       const void* act,
                                                       ACE_HANDLE event,
                                                       int priority,
                                                       int signal_number)
{
  AsynchWriteStreamResultImpl *implementation;
  ACE_NEW_RETURN (implementation,
                  PosixAsynchWriteStreamResult (handler,
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
PosixProactor::create_asynch_read_file_result (ACE_Handler &handler,
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
  AsynchReadFileResultImpl *implementation;
  ACE_NEW_RETURN (implementation,
                  PosixAsynchReadFileResult (handler,
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
PosixProactor::create_asynch_write_file_result (ACE_Handler &handler,
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
  AsynchWriteFileResultImpl *implementation;
  ACE_NEW_RETURN (implementation,
                  PosixAsynchWriteFileResult (handler,
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
PosixProactor::create_asynch_timer (ACE_Handler &handler,
                                         const void *act,
                                         const TimeValue &tv,
                                         ACE_HANDLE event,
                                         int priority,
                                         int signal_number)
{
  AsynchResultImpl *implementation;
  ACE_NEW_RETURN (implementation,
                  PosixAsynchTimer (handler,
                                          act,
                                          tv,
                                          event,
                                          priority,
                                          signal_number),
                  0);
  return implementation;
}

#if 0
int
PosixProactor::handle_signal (int, siginfo_t *, ucontext_t *)
{
  // Perform a non-blocking "poll" for all the I/O events that have
  // completed in the I/O completion queue.

  TimeValue timeout (0, 0);
  int result = 0;

  for (;;)
    {
      result = this->handle_events (timeout);
      if (result != 0 || errno == ETIME)
        break;
    }

  // If our handle_events failed, we'll report a failure to the
  // Reactor.
  return result == -1 ? -1 : 0;
}

int
PosixProactor::handle_close (ACE_HANDLE handle,
                                  ACE_Reactor_Mask close_mask)
{
  ACE_UNUSED_ARG (close_mask);
  ACE_UNUSED_ARG (handle);

  return this->close ();
}
#endif /* 0 */

void
PosixProactor::application_specific_code (PosixAsynchResult *asynch_result,
                                               u_long bytes_transferred,
                                               int success,
                                               const void */* completion_key*/,
                                               u_long error)
{
  ACE_SEH_TRY
    {
      // Call completion hook
      asynch_result->complete (bytes_transferred,
                               success,
                               0, // No completion key.
                               error);
    }
  ACE_SEH_FINALLY
    {
      // This is crucial to prevent memory leaks
      delete asynch_result;
    }
}

int
PosixProactor::post_wakeup_completions (int how_many)
{
  ACE_POSIX_Wakeup_Completion *wakeup_completion = 0;

  for (ssize_t ci = 0; ci < how_many; ci++)
    {
      ACE_NEW_RETURN (wakeup_completion,
                      ACE_POSIX_Wakeup_Completion (this->wakeup_handler_),
                      -1);

      if (wakeup_completion->post_completion (this) == -1)
        return -1;
    }

  return 0;
}

ACE_INLINE
PosixProactor::Proactor_Type PosixProactor::get_impl_type (void)
{
  return PROACTOR_POSIX;
}

ACE_INLINE
PosixProactor::Proactor_Type PosixAiocbProactor::get_impl_type (void)
{
  return PROACTOR_AIOCB;
}

ACE_INLINE
PosixProactor::Proactor_Type PosixSigProactor::get_impl_type (void)
{
  return PROACTOR_SIG;
}

class ACE_Export ACE_AIOCB_Notify_Pipe_Manager : public ACE_Handler
{
  // = TITLE
  //     This class manages the notify pipe of the AIOCB
  //     Proactor. This class acts as the Handler for the
  //     <Asynch_Read> operations issued on the notify pipe. This
  //     class is very useful in implementing <Asynch_Accept> operation
  //     class for the <AIOCB_Proactor>. This is also useful for
  //     implementing <post_completion> for <AIOCB_Proactor>.
  //
  // = DESCRIPTION
  //     <AIOCB_Proactor> class issues a <Asynch_Read> on
  //     the pipe, using this class as the
  //     Handler. <POSIX_Asynch_Result *>'s are sent through the
  //     notify pipe. When <POSIX_Asynch_Result *>'s show up on the
  //     notify pipe, the <POSIX_AIOCB_Proactor> dispatches the
  //     completion of the <Asynch_Read_Stream> and calls the
  //     <handle_read_stream> of this class. This class calls
  //     <complete> on the <POSIX_Asynch_Result *> and thus calls the
  //     application handler.
  //     Handling the MessageBlock:
  //     We give this message block to read the result pointer through
  //     the notify pipe. We expect that to read 4 bytes from the
  //     notify pipe, for each <accept> call. Before giving this
  //     message block to another <accept>, we update <wr_ptr> and put
  //     it in its initial position.
public:
  ACE_AIOCB_Notify_Pipe_Manager (PosixAiocbProactor *posix_aiocb_proactor);
  // Constructor. You need the posix proactor because you need to call
  // <application_specific_code>

  virtual ~ACE_AIOCB_Notify_Pipe_Manager (void);
  // Destructor.

  int notify ();
  // Send the result pointer through the notification pipe.

  virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);
  // This is the call back method when <Asynch_Read> from the pipe is
  // complete.

private:
  PosixAiocbProactor  *posix_aiocb_proactor_;
  // The implementation proactor class.

  MessageBlock message_block_;
  // Message block to get PosixAsynchResult pointer from the
  // pipe.

  ACE_Pipe pipe_;
  // Pipe for the communication between Proactor and the
  // Asynch_Accept.

  PosixAiocbAsynchReadStream read_stream_;
  // To do asynch_read on the pipe.

  ACE_AIOCB_Notify_Pipe_Manager (void);
  // Default constructor. Shouldnt be called.
};

ACE_AIOCB_Notify_Pipe_Manager::ACE_AIOCB_Notify_Pipe_Manager (PosixAiocbProactor *posix_aiocb_proactor)
  : posix_aiocb_proactor_ (posix_aiocb_proactor),
    message_block_ (sizeof (PosixAsynchReadFileResult *)),
    read_stream_ (posix_aiocb_proactor)
{
  // Open the pipe.
  this->pipe_.open ();

  // Set write side in NONBLOCK mode
  ACE::set_flags (this->pipe_.write_handle (), ACE_NONBLOCK);

  // Let AIOCB_Proactor know about our handle
  posix_aiocb_proactor_->set_notify_handle (this->pipe_.read_handle ());

  // Open the read stream.
  if (this->read_stream_.open (*this,
                               this->pipe_.read_handle (),
                               0, // Completion Key
                               0) // Proactor
      == -1)
    ACE_ERROR ((LM_ERROR,
                "%s:%s\n",
                "ACE_AIOCB_Notify_Pipe_Manager::ACE_AIOCB_Notify_Pipe_Manager:"
                "Open on Read Stream failed"));

  // Issue an asynch_read on the read_stream of the notify pipe.
  if (this->read_stream_.read (this->message_block_,
                               1, // enough to read 1 byte
                               0, // ACT
                               0) // Priority
      == -1)
    ACE_ERROR ((LM_ERROR,
                "%s:%s\n",
                "ACE_AIOCB_Notify_Pipe_Manager::ACE_AIOCB_Notify_Pipe_Manager:"
                "Read from pipe failed"));
}

ACE_AIOCB_Notify_Pipe_Manager::~ACE_AIOCB_Notify_Pipe_Manager (void)
{
}

int
ACE_AIOCB_Notify_Pipe_Manager::notify ()
{
  // Send the result pointer through the pipe.
  char char_send = 0;
  int  ret_val = ::send (this->pipe_.write_handle (),&char_send, sizeof (char_send), 0);

  if (ret_val < 0 && errno != EWOULDBLOCK)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "(%s):%s\n",
                       "ACE_AIOCB_Notify_Pipe_Manager::notify"
                       "Error:Writing on to notify pipe failed"),
                      -1);
  return 0;
}

void
ACE_AIOCB_Notify_Pipe_Manager::handle_read_stream
  (const ACE_Asynch_Read_Stream::Result & /*result*/)
{
  // 1. Start new read to avoid pipe overflow

  // Set the message block properly. Put the <wr_ptr> back in the
  // initial position.
  if (this->message_block_.length () > 0)
      this->message_block_.wrPtr (this->message_block_.rdPtr ());

  // One accept has completed. Issue a read to handle any
  // <post_completion>s in the future.
  if (this->read_stream_.read (this->message_block_,
                               1,  // enough to read 1 byte
                               0,  // ACT
                               0)  // Priority
      == -1)
    ACE_ERROR ((LM_ERROR,
                "%s:%s\n",
                "ACE_AIOCB_Notify_Pipe_Manager::handle_read_stream:"
                "Read from pipe failed"));


  // 2. Do the upcalls
  // this->posix_aiocb_proactor_->process_result_queue ();
}

// Public constructor for common use.
PosixAiocbProactor::PosixAiocbProactor (size_t max_aio_operations)
  : aiocb_notify_pipe_manager_ (0),
    aiocb_list_ (0),
    result_list_ (0),
    aiocb_list_max_size_ (max_aio_operations),
    aiocb_list_cur_size_ (0),
    notify_pipe_read_handle_ (ACE_INVALID_HANDLE),
    num_deferred_aiocb_ (0),
    num_started_aio_ (0)
{
  //check for correct value for max_aio_operations
  check_max_aio_num ();

  ACE_NEW (aiocb_list_,
           aiocb *[aiocb_list_max_size_]);
  ACE_NEW (result_list_,
           PosixAsynchResult *[aiocb_list_max_size_]);

  // Initialize the array.
  for (size_t ai = 0; ai < this->aiocb_list_max_size_; ai++)
    {
      aiocb_list_[ai] = 0;
      result_list_[ai] = 0;
    }

  create_notify_manager ();
}

// Special protected constructor for ACE_SUN_Proactor
PosixAiocbProactor::PosixAiocbProactor (size_t max_aio_operations,
                                                    PosixProactor::Proactor_Type ptype)
  : aiocb_notify_pipe_manager_ (0),
    aiocb_list_ (0),
    result_list_ (0),
    aiocb_list_max_size_ (max_aio_operations),
    aiocb_list_cur_size_ (0),
    notify_pipe_read_handle_ (ACE_INVALID_HANDLE),
    num_deferred_aiocb_ (0),
    num_started_aio_ (0)
{
  ACE_UNUSED_ARG (ptype);

  //check for correct value for max_aio_operations
  check_max_aio_num ();

  ACE_NEW (aiocb_list_,
           aiocb *[aiocb_list_max_size_]);
  ACE_NEW (result_list_,
           PosixAsynchResult *[aiocb_list_max_size_]);

  // Initialize the array.
  for (size_t ai = 0; ai < this->aiocb_list_max_size_; ai++)
    {
      aiocb_list_[ai] = 0;
      result_list_[ai] = 0;
    }

  // @@ We should create Notify_Pipe_Manager in the derived class to
  // provide correct calls for virtual functions !!!
}

// Destructor.
PosixAiocbProactor::~PosixAiocbProactor (void)
{
  delete_notify_manager ();

  // delete all uncomlpeted operarion
  // as nobody will notify client since now
  for (size_t ai = 0; ai < aiocb_list_max_size_; ai++)
    {
      delete result_list_[ai];
      result_list_[ai] = 0;
      aiocb_list_[ai] = 0;
    }

  delete [] aiocb_list_;
  aiocb_list_ = 0;

  delete [] result_list_;
  result_list_ = 0;

  clear_result_queue ();
}

void PosixAiocbProactor::set_notify_handle (ACE_HANDLE h)
{
  notify_pipe_read_handle_ = h;
}

void PosixAiocbProactor::check_max_aio_num ()
{
  long max_os_aio_num = OS ::sysconf (_SC_AIO_MAX);

  // Define max limit AIO's for concrete OS
  // -1 means that there is no limit, but it is not true
  // (example, SunOS 5.6)

  if (max_os_aio_num > 0
     && aiocb_list_max_size_ > (unsigned long) max_os_aio_num
    )
     aiocb_list_max_size_ = max_os_aio_num;

#if defined (HPUX)
  // Although HPUX 11.00 allows to start 2048 AIO's
  // for all process in system
  // it has a limit 256 max elements for aio_suspend ()
  // It is a pity, but ...

  long max_os_listio_num = OS ::sysconf (_SC_AIO_LISTIO_MAX);
  if (max_os_listio_num > 0
      && aiocb_list_max_size_ > (unsigned long) max_os_listio_num)
    aiocb_list_max_size_ = max_os_listio_num;
#endif /* HPUX */

  // check for user-defined value
  // ACE_AIO_MAX_SIZE if defined in POSIX_Proactor.h

  if (aiocb_list_max_size_ <= 0
     || aiocb_list_max_size_ > ACE_AIO_MAX_SIZE)
     aiocb_list_max_size_ = ACE_AIO_MAX_SIZE;

  // check for max number files to open

  int max_num_files = ACE::max_handles ();

  if (max_num_files > 0
      && aiocb_list_max_size_ > (unsigned long) max_num_files)
    {
      ACE::set_handle_limit (aiocb_list_max_size_);

      max_num_files = ACE::max_handles ();
    }

  if (max_num_files > 0
      && aiocb_list_max_size_ > (unsigned long) max_num_files)
    aiocb_list_max_size_ = (unsigned long) max_num_files;

  ACE_DEBUG ((LM_DEBUG,
             "PosixAiocbProactor::Max Number of AIOs=%d\n",
              aiocb_list_max_size_));

}

void
PosixAiocbProactor::create_notify_manager (void)
{
  // Accept Handler for aio_accept. Remember! this issues a Asynch_Read
  // on the notify pipe for doing the Asynch_Accept.

  if (aiocb_notify_pipe_manager_ == 0)
    ACE_NEW (aiocb_notify_pipe_manager_,
             ACE_AIOCB_Notify_Pipe_Manager (this));
}

void
PosixAiocbProactor::delete_notify_manager (void)
{
  // We are responsible for delete as all pointers set to 0 after
  // delete, it is save to delete twice

  delete aiocb_notify_pipe_manager_;
  aiocb_notify_pipe_manager_ = 0;
}

int
PosixAiocbProactor::handle_events (TimeValue &wait_time)
{
  // Decrement <wait_time> with the amount of time spent in the method
  //ACE_Countdown_Time countdown (&wait_time);
  return this->handle_events (wait_time.msec ());
}

int
PosixAiocbProactor::handle_events (void)
{
  return this->handle_events (ACE_INFINITE);
}

int
PosixAiocbProactor::notify_completion(int  sig_num)
{
  ACE_UNUSED_ARG (sig_num);

  return this->aiocb_notify_pipe_manager_->notify ();
}

int
PosixAiocbProactor::post_completion (PosixAsynchResult *result)
{
  ACE_MT (ACE_GUARD_RETURN (SYNCH_MUTEX, ace_mon, this->mutex_, -1));

  int ret_val = this->putq_result (result);

  return ret_val;
}

int
PosixAiocbProactor::putq_result (PosixAsynchResult *result)
{
  // this protected method should be called with locked mutex_
  // we can't use GUARD as Proactor uses non-recursive mutex

  if (!result)
    return -1;

  int sig_num = result->signal_number ();
  int ret_val = this->result_queue_.enqueue_tail (result);

  if (ret_val == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "PosixAiocbProactor::putq_result failed\n"),
                      -1);

  this->notify_completion (sig_num);

  return 0;
}

PosixAsynchResult * PosixAiocbProactor::getq_result (void)
{
  ACE_MT (ACE_GUARD_RETURN (SYNCH_MUTEX, ace_mon, this->mutex_, 0));


  PosixAsynchResult* result = 0;

  if (this->result_queue_.dequeue_head (result) != 0)
    return 0;

//  don;t waste time if queue is empty - it is normal
//  or check queue size before dequeue_head
//    ACE_ERROR_RETURN ((LM_ERROR,
//                       "%N:%l:(%P | %t):%p\n",
//                       "PosixAiocbProactor::getq_result failed"),
//                      0);

  return result;
}

int PosixAiocbProactor::clear_result_queue (void)
{
  int ret_val = 0;
  PosixAsynchResult* result = 0;

  while ((result = this->getq_result ()) != 0)
    {
      delete result;
      ret_val++;
    }

  return ret_val;
}

int PosixAiocbProactor::process_result_queue (void)
{
  int ret_val = 0;
  PosixAsynchResult* result = 0;

  while ((result = this->getq_result ()) != 0)
    {
      this->application_specific_code
            (result,
             result->bytes_transferred(), // 0, No bytes transferred.
             1,  // Result : True.
             0,  // No completion key.
             result->error());   //0, No error.

      ret_val++;
    }

  return ret_val;
}

AsynchReadStreamImpl *
PosixAiocbProactor::create_asynch_read_stream (void)
{
  AsynchReadStreamImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  PosixAiocbAsynchReadStream (this),
                  0);
  return implementation;
}

AsynchWriteStreamImpl *
PosixAiocbProactor::create_asynch_write_stream (void)
{
  AsynchWriteStreamImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  PosixAiocbAsynchWriteStream (this),
                  0);
  return implementation;
}

AsynchReadFileImpl *
PosixAiocbProactor::create_asynch_read_file (void)
{
  AsynchReadFileImpl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  PosixAiocbAsynchReadFile (this),
                  0);
  return  implementation;
}

ACE_Asynch_Write_File_Impl *
PosixAiocbProactor::create_asynch_write_file (void)
{
  ACE_Asynch_Write_File_Impl *implementation = 0;
  ACE_NEW_RETURN (implementation,
                  PosixAiocbAsynchWriteFile (this),
                  0);
  return  implementation;
}

int
PosixAiocbProactor::handle_events (u_long milli_seconds)
{
  int result_suspend = 0;
  int retval= 0;

  if (milli_seconds == ACE_INFINITE)
    // Indefinite blocking.
    result_suspend = aio_suspend (aiocb_list_,
                                  aiocb_list_max_size_,
                                  0);
  else
    {
      // Block on <aio_suspend> for <milli_seconds>
      timespec timeout;
      timeout.tv_sec = milli_seconds / 1000;
      timeout.tv_nsec = (milli_seconds - (timeout.tv_sec * 1000)) * 1000;
      result_suspend = aio_suspend (aiocb_list_,
                                    aiocb_list_max_size_,
                                    &timeout);
    }

  // Check for errors
  if (result_suspend == -1)
    {
      if (errno != EAGAIN &&   // Timeout
          errno != EINTR )    // Interrupted call
          ACE_ERROR ((LM_ERROR,
                      "%s::%s\n",
                      "PosixAiocbProactor::handle_events:"
                      "aio_suspend failed\n"));

      // let continue work
      // we should check "post_completed" queue
    }
  else
    {
      size_t index = 0;
      size_t count = aiocb_list_max_size_;  // max number to iterate
      int error_status = 0;
      int return_status = 0;

      for (;; retval++)
        {
          PosixAsynchResult *asynch_result =
            find_completed_aio (error_status,
                                return_status,
                                index,
                                count);

          if (asynch_result == 0)
            break;

          // Call the application code.
          this->application_specific_code (asynch_result,
                                         return_status, // Bytes transferred.
                                         1,             // Success
                                         0,             // No completion key.
                                         error_status); // Error
        }
    }

  // process post_completed results
  retval += this->process_result_queue ();

  return retval > 0 ? 1 : 0;
}

PosixAsynchResult *
PosixAiocbProactor::find_completed_aio (int &error_status,
                                              int &return_status,
                                              size_t &index,
                                              size_t &count)
{
  // parameter index defines initial slot to scan
  // parameter count tells us how many slots should we scan

  ACE_MT (ACE_GUARD_RETURN (ThreadMutex, ace_mon, this->mutex_, 0));

  PosixAsynchResult *asynch_result = 0;

  error_status = 0;
  return_status= 0;

  if (num_started_aio_ == 0)  // save time
    return asynch_result;

  for (; count > 0; index++ , count--)
    {
      if (index >= aiocb_list_max_size_) // like a wheel
          index = 0;

      if (aiocb_list_[index] == 0) // Dont process null blocks.
        continue;

      // Get the error status of the aio_ operation.
      error_status = aio_error (aiocb_list_[index]);

      if (error_status == -1)   // <aio_error> itself has failed.
        {
          ACE_ERROR ((LM_ERROR,
                      "%s::%s\n",
                      "PosixAiocbProactor::find_completed_aio:"
                      "<aio_error> has failed\n"));

          break;

          // we should notify user, otherwise :
          // memory leak for result and "hanging" user
          // what was before skip this operation

          //aiocb_list_[index] = 0;
          //result_list_[index] = 0;
          //aiocb_list_cur_size_--;
          //continue;
        }

      // Continue the loop if <aio_> operation is still in progress.
      if (error_status != EINPROGRESS)
        break;

    } // end for

  if (count == 0) // all processed , nothing found
    return asynch_result;

  if (error_status == ECANCELED)
    return_status = 0;
  else if (error_status == -1)
    return_status = 0;
  else
    return_status = aio_return (aiocb_list_[index]);

  if (return_status == -1)
    {
      return_status = 0; // zero bytes transferred

      if (error_status == 0)  // nonsense
        ACE_ERROR ((LM_ERROR,
                  "%s::%s\n",
                  "PosixAiocbProactor::find_completed_aio:"
                  "<aio_return> failed\n"));
    }


  asynch_result = result_list_[index];

  aiocb_list_[index] = 0;
  result_list_[index] = 0;
  aiocb_list_cur_size_--;

  num_started_aio_--;  // decrement count active aios
  index++;            // for next iteration
  count--;             // for next iteration

  this->start_deferred_aio ();
  //make attempt to start deferred AIO
  //It is safe as we are protected by mutex_

  return asynch_result;
}

void
PosixAiocbProactor::application_specific_code (PosixAsynchResult *asynch_result,
                                                     u_long bytes_transferred,
                                                     int success,
                                                     const void *completion_key,
                                                     u_long error)
{
  PosixProactor::application_specific_code (asynch_result,
                                                 bytes_transferred,
                                                 success,
                                                 completion_key,
                                                 error);
}

int
PosixAiocbProactor::register_and_start_aio (PosixAsynchResult *result,
                                                  int op)
{
  ACE_TRACE ("PosixAiocbProactor::register_and_start_aio");

  ACE_MT (ACE_GUARD_RETURN (ThreadMutex, ace_mon, this->mutex_, -1));

  int ret_val = (aiocb_list_cur_size_ >= aiocb_list_max_size_) ? -1 : 0;

  if (result == 0) // Just check the status of the list
    return ret_val;

  // Save operation code in the aiocb
  switch (op)
    {
    case 0 :
      result->aio_lio_opcode = LIO_READ;
      break;

    case 1 :
      result->aio_lio_opcode = LIO_WRITE;
      break;

    default:
      ACE_ERROR_RETURN ((LM_ERROR,
                         "%s%s)::\n"
                         "register_and_start_aio: Invalid operation code\n"),
                        -1);
    }

  if (ret_val != 0)   // No free slot
    {
      errno   = EAGAIN;
      ACE_ERROR_RETURN ((LM_ERROR,
                         "%s| %s)::\n"
                         "register_and_start_aio: "
                         "No space to store the <aio>info\n"),
                        -1);
    }

  // Find a free slot and store.

  ret_val = allocate_aio_slot (result);

  if (ret_val < 0)
    return -1;

  size_t index = ACE_static_cast (size_t, ret_val);

  result_list_[index] = result;   //Store result ptr anyway
  aiocb_list_cur_size_++;

  ret_val = start_aio (result);

  switch (ret_val)
    {
    case 0 :     // started OK
      aiocb_list_[index] = result;
      return 0;

    case 1 :     //OS AIO queue overflow
      num_deferred_aiocb_ ++;
      return 0;

    default:    //Invalid request, there is no point
      break;    // to start it later
    }

  result_list_[index] = 0;
  aiocb_list_cur_size_--;

  ACE_ERROR ((LM_ERROR,
              "%s::%s\n",
              "register_and_start_aio: Invalid request to start <aio>\n"));
  return -1;
}

int
PosixAiocbProactor::allocate_aio_slot (PosixAsynchResult *result)
{
  size_t i = 0;

  // we reserve zero slot for ACE_AIOCB_Notify_Pipe_Manager
  // so make check for ACE_AIOCB_Notify_Pipe_Manager request

  if (notify_pipe_read_handle_ == result->aio_fildes) // Notify_Pipe ?
    {                                       // should be free,
      if (result_list_[i] != 0)           // only 1 request
        {                                   // is allowed
          errno   = EAGAIN;
          ACE_ERROR_RETURN ((LM_ERROR,
                     "%N:%l:(%P | %t)::\n"
                     "PosixAiocbProactor::allocate_aio_slot:"
                     "internal Proactor error 0\n"),
                     -1);
        }
    }
  else  //try to find free slot as usual, but starting from 1
    {
      for (i= 1; i < this->aiocb_list_max_size_; i++)
        if (result_list_[i] == 0)
          break;
    }

  if (i >= this->aiocb_list_max_size_)
    ACE_ERROR_RETURN ((LM_ERROR,
              "%N:%l:(%P | %t)::\n"
              "PosixAiocbProactor::allocate_aio_slot:"
              "internal Proactor error 1\n"),
              -1);


  //setup OS notification methods for this aio
  result->aio_sigevent.sigev_notify = SIGEV_NONE;

  return ACE_static_cast (int, i);
}

// start_aio  has new return codes
//     0    AIO was started successfully
//     1    AIO was not started, OS AIO queue overflow
//     -1   AIO was not started, other errors

int
PosixAiocbProactor::start_aio (PosixAsynchResult *result)
{
  ACE_TRACE ("PosixAiocbProactor::start_aio");

  int ret_val;
  const ACE_TCHAR *ptype;

  // Start IO

  switch (result->aio_lio_opcode )
    {
    case LIO_READ :
      ptype = "read ";
      ret_val = aio_read (result);
      break;
    case LIO_WRITE :
      ptype = "write";
      ret_val = aio_write (result);
      break;
    default:
      ptype = "?????";
      ret_val = -1;
      break;
    }

  if (ret_val == 0)
    num_started_aio_ ++;
  else // if (ret_val == -1)
    {
      if (errno == EAGAIN)  //Ok, it will be deferred AIO
         ret_val = 1;
      else
        ACE_ERROR ((LM_ERROR,
                "%N:%l:(%P | %t)::start_aio: aio_%s %p\n",
                ptype,
                "queueing failed\n"));
    }

  return ret_val;
}


int
PosixAiocbProactor::start_deferred_aio ()
{
  ACE_TRACE ("PosixAiocbProactor::start_deferred_aio");

  // This protected method is called from
  // find_completed_aio after any AIO completion
  // We should call this method always with locked
  // PosixAiocbProactor::mutex_
  //
  // It tries to start the first deferred AIO
  // if such exists

  if (num_deferred_aiocb_ == 0)
    return 0;  //  nothing to do

  size_t i = 0;

  for (i= 0; i < this->aiocb_list_max_size_; i++)
    if (result_list_[i] !=0       // check for
       && aiocb_list_[i]  ==0)     // deferred AIO
      break;

  if (i >= this->aiocb_list_max_size_)
    ACE_ERROR_RETURN ((LM_ERROR,
                 "%N:%l:(%P | %t)::\n"
                 "start_deferred_aio:"
                 "internal Proactor error 3\n"),
                 -1);

  PosixAsynchResult *result = result_list_[i];

  int ret_val = start_aio (result);

  switch (ret_val)
    {
    case 0 :    //started OK , decrement count of deferred AIOs
      aiocb_list_[i] = result;
      num_deferred_aiocb_ --;
      return 0;

    case 1 :
      return 0;  //try again later

    default :     // Invalid Parameters , should never be
      break;
    }

  //AL notify  user

  result_list_[i] = 0;
  aiocb_list_cur_size_--;

  num_deferred_aiocb_ --;

  result->set_error (errno);
  result->set_bytes_transferred (0);
  this->putq_result (result);  // we are with locked mutex_ here !

  return -1;
}

int
PosixAiocbProactor::cancel_aio (ACE_HANDLE handle)
{
  // This new method should be called from
  // PosixAsynchOperation instead of usual ::aio_cancel
  // It scans the result_list_ and defines all AIO requests
  // that were issued for handle "handle"
  //
  // For all deferred AIO requests with handle "handle"
  // it removes its from the lists and notifies user
  //
  // For all running AIO requests with handle "handle"
  // it calls ::aio_cancel. According to the POSIX standards
  // we will receive ECANCELED  for all ::aio_canceled AIO requests
  // later on return from ::aio_suspend

  ACE_TRACE ("PosixAiocbProactor::cancel_aio");

  int    num_total     = 0;
  int    num_cancelled = 0;

  {
    ACE_MT (ACE_GUARD_RETURN (ThreadMutex, ace_mon, this->mutex_, -1));

    size_t ai = 0;

    for (ai = 0; ai < aiocb_list_max_size_; ai++)
      {
        if (result_list_[ai] == 0)    //skip empty slot
           continue;

        if (result_list_[ai]->aio_fildes != handle)  //skip not our slot
           continue;

        num_total++;

        PosixAsynchResult *asynch_result = result_list_[ai];

        if (aiocb_list_ [ai] == 0)  //deferred aio
          {
            num_cancelled ++;
            num_deferred_aiocb_ --;

            aiocb_list_[ai] = 0;
            result_list_[ai] = 0;
            aiocb_list_cur_size_--;

            asynch_result->set_error (ECANCELED);
            asynch_result->set_bytes_transferred (0);
            this->putq_result (asynch_result);
            // we are with locked mutex_ here !
          }
        else      //cancel started aio
          {
            int rc_cancel = this->cancel_aiocb (asynch_result);

            if (rc_cancel == 0)    //notification in the future
              num_cancelled ++;     //it is OS responsiblity
          }
      }

  } // release mutex_

  if (num_total == 0)
    return 1;  // ALLDONE

  if (num_cancelled == num_total)
    return 0;  // CANCELLED

  return 2; // NOT CANCELLED
}

int
PosixAiocbProactor::cancel_aiocb (PosixAsynchResult * result)
{
  // This new method is called from cancel_aio
  // to cancel concrete running AIO request
  int rc = ::aio_cancel (0, result);

  // Check the return value and return 0/1/2 appropriately.
  if (rc == AIO_CANCELED)
    return 0;
  else if (rc == AIO_ALLDONE)
    return 1;
  else if (rc == AIO_NOTCANCELED)
    return 2;

  ACE_ERROR_RETURN ((LM_ERROR,
                       "%N:%l:(%P | %t)::%p\n",
                       "cancel_aiocb:"
                       "Unexpected result from <aio_cancel>"),
                      -1);

}


// *********************************************************************

PosixSigProactor::PosixSigProactor (size_t max_aio_operations)
  :  PosixAiocbProactor (max_aio_operations,
                               PosixProactor::PROACTOR_SIG)
{
  // = Set up the mask we'll use to block waiting for SIGRTMIN. Use that
  // to add it to the signal mask for this thread, and also set the process
  // signal action to pass signal information when we want it.

  // Clear the signal set.
  if (sigemptyset (&this->RT_completion_signals_) == -1)
    ACE_ERROR ((LM_ERROR,
                "Error:%p\n",
                "Couldn't init the RT completion signal set"));

  // Add the signal number to the signal set.
  if (sigaddset (&this->RT_completion_signals_, ACE_SIGRTMIN) == -1)
    ACE_ERROR ((LM_ERROR,
                "Error:%p\n",
                "Couldnt init the RT completion signal set"));
  this->mask_signals (&this->RT_completion_signals_);
  // Set up the signal action for SIGRTMIN.
  this->setup_signal_handler (ACE_SIGRTMIN);

  // we do not have to create notify manager
  return;
}

PosixSigProactor::PosixSigProactor (const sigset_t signal_set,
                                                size_t max_aio_operations)
  :  PosixAiocbProactor (max_aio_operations,
                               PosixProactor::PROACTOR_SIG)
{
  // = Keep <Signal_set> with the Proactor, mask all the signals and
  //   setup signal actions for the signals in the <signal_set>.

  // = Keep <signal_set> with the Proactor.

  // Empty the signal set first.
  if (sigemptyset (&this->RT_completion_signals_) == -1)
    ACE_ERROR ((LM_ERROR,
                "Error:(%P | %t):%p\n",
                "sigemptyset failed"));

  // For each signal number present in the <signal_set>, add it to
  // the signal set we use, and also set up its process signal action
  // to allow signal info to be passed into sigwait/sigtimedwait.
  int member = 0;
  for (int si = ACE_SIGRTMIN; si <= ACE_SIGRTMAX; si++)
    {
      member = sigismember (&signal_set,
                            si);
      if (member == -1)
      {
        ACE_ERROR ((LM_ERROR,
                    "%N:%l:(%P | %t)::%p\n",
                    "PosixSigProactor::PosixSigProactor:"
                    "sigismember failed"));
      }
      else if (member == 1)
        {
          sigaddset (&this->RT_completion_signals_, si);
          this->setup_signal_handler (si);
        }
    }

  // Mask all the signals.
  this->mask_signals (&this->RT_completion_signals_);

  // we do not have to create notify manager
  return;
}

PosixSigProactor::~PosixSigProactor (void)
{
  // @@ Enable the masked signals again.
}

int
PosixSigProactor::handle_events (TimeValue &wait_time)
{
  // Decrement <wait_time> with the amount of time spent in the method
  //ACE_Countdown_Time countdown (&wait_time);
  return this->handle_events (wait_time.msec ());
}

int
PosixSigProactor::handle_events (void)
{
  return this->handle_events (ACE_INFINITE);
}

int
PosixSigProactor::notify_completion (int sig_num)
{
  // Get this process id.
  pid_t pid = OS::getpid ();
  if (pid == (pid_t) -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error:%N:%l(%P | %t):%p",
                       "<getpid> failed"),
                      -1);

  // Set the signal information.
  sigval value;
#if defined (__FreeBSD__)
  value.sigval_int = -1;
#else
  value.sival_int = -1;
#endif /* __FreeBSD__ */

  // Solaris 8 can "forget" to delivery
  // two or more signals queued immediately.
  // Just comment the following "if" statement
  // and try this->post_completion(2)

  if (os_id_ == OS_SUN_58 && result_queue_.size() > 1)
    return 0;

  // Queue the signal.
  if (sigqueue (pid, sig_num, value) == 0)
    return 0;

  if (errno != EAGAIN)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error:%N:%l:(%P | %t):%p\n",
                       "<sigqueue> failed"),
                      -1);
  return -1;
}

AsynchResultImpl *
PosixSigProactor::create_asynch_timer (ACE_Handler &handler,
                                             const void *act,
                                             const TimeValue &tv,
                                             ACE_HANDLE event,
                                             int priority,
                                             int signal_number)
{
  int is_member = 0;

  // Fix the signal number.
  if (signal_number == -1)
    {
      int si;
      for (si = ACE_SIGRTMAX;
           (is_member == 0) && (si >= ACE_SIGRTMIN);
           si--)
        {
          is_member = sigismember (&this->RT_completion_signals_,
                                   si);
          if (is_member == -1)
            ACE_ERROR_RETURN ((LM_ERROR,
                               "%N:%l:(%P | %t)::%s\n",
                               "PosixSigProactor::create_asynch_timer:"
                               "sigismember failed"),
                              0);
        }

      if (is_member == 0)
      {
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Error:%N:%l:(%P | %t)::%s\n",
                           "PosixSigProactor::PosixSigProactor:"
                           "Signal mask set empty"),
                          0);
      }
      else
        // + 1 to nullify loop increment.
        signal_number = si + 1;
    }

  AsynchResultImpl *implementation;
  ACE_NEW_RETURN (implementation,
                  PosixAsynchTimer (handler,
                                          act,
                                          tv,
                                          event,
                                          priority,
                                          signal_number),
                  0);
  return implementation;
}


void sig_handler (int sig_num, siginfo_t *, ucontext_t *)
{
  // Should never be called
  ACE_DEBUG ((LM_DEBUG,
              "%N:%l:(%P | %t)::sig_handler received signal: %d\n",
               sig_num));
  return;
}

int
PosixSigProactor::setup_signal_handler (int signal_number) const
{
  // Set up the specified signal so that signal information will be
  // passed to sigwaitinfo/sigtimedwait. Don't change the default
  // signal handler - having a handler and waiting for the signal can
  // produce undefined behavior.

  // But can not use SIG_DFL
  // With SIG_DFL after delivering the first signal
  // SIG_DFL handler resets  SA_SIGINFO flags
  // and we will lose all information sig_info
  // At least all SunOS have such behavior

  struct sigaction reaction;
  sigemptyset (&reaction.sa_mask);   // Nothing else to mask.
  reaction.sa_flags = SA_SIGINFO;    // Realtime flag.
  //reaction.sa_sigaction = ACE_SIGNAL_C_FUNC(sig_handler); // (SIG_DFL);
  int sigaction_return = ::sigaction (signal_number,
                                            &reaction,
                                            0);
  if (sigaction_return == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error:%p\n",
                       "Proactor couldnt do sigaction for the RT SIGNAL"),
                      -1);
  return 0;
}


int
PosixSigProactor::mask_signals (const sigset_t *signals) const
{
  if (::pthread_sigmask (SIG_BLOCK, signals, 0) != 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error:(%P | %t):%p\n",
                       "pthread_sigmask failed"),
                      -1);
  return 0;
}

int
PosixSigProactor::allocate_aio_slot (PosixAsynchResult *result)
{
  size_t i = 0;

  //try to find free slot as usual, starting from 0
  for (i = 0; i < this->aiocb_list_max_size_; i++)
    if (result_list_[i] == 0)
      break;

  if (i >= this->aiocb_list_max_size_)
    ACE_ERROR_RETURN ((LM_ERROR,
              "%N:%l:(%P | %t)::\n"
              "PosixSigProactor::allocate_aio_slot "
              "internal Proactor error 1\n"),
              -1);

  int retval = ACE_static_cast (int, i);

  // setup OS notification methods for this aio
  // store index!!, not pointer in signal info
  result->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  result->aio_sigevent.sigev_signo = result->signal_number ();
#if defined (__FreeBSD__)
  result->aio_sigevent.sigev_value.sigval_int = retval;
#else
  result->aio_sigevent.sigev_value.sival_int = retval;
#endif /* __FreeBSD__ */

  return retval;
}

int
PosixSigProactor::handle_events (unsigned long milli_seconds)
{
  int result_sigwait = 0;
  siginfo_t sig_info;

  // Mask all the signals.
  if (this->mask_signals (&this->RT_completion_signals_) != 0)
    return -1;

  // Wait for the signals.
  if (milli_seconds == ACE_INFINITE)
    {
      result_sigwait = sigwaitinfo (&this->RT_completion_signals_,
                                    &sig_info);
    }
  else
    {
      // Wait for <milli_seconds> amount of time.
      timespec timeout;
      timeout.tv_sec = milli_seconds / 1000;
      timeout.tv_nsec = (milli_seconds - (timeout.tv_sec * 1000)) * 1000;
      result_sigwait = sigtimedwait (&this->RT_completion_signals_,
                                     &sig_info,
                                     &timeout);
    }

  size_t index = 0;          // start index to scan aiocb list
  size_t count = aiocb_list_max_size_;  // max number to iterate
  int error_status = 0;
  int return_status = 0;
  int flg_aio = 0;          // 1 if AIO Completion possible
  int flg_que = 0;          // 1 if SIGQUEUE possible

  // define index to start
  // nothing will happen if it contains garbage
#if defined (__FreeBSD__)
  index = ACE_static_cast (size_t, sig_info.si_value.sigval_int);
#else
  index = ACE_static_cast (size_t, sig_info.si_value.sival_int);
#endif

  // Check for errors
  // but let continue work in case of errors
  // we should check "post_completed" queue
  if (result_sigwait == -1)
    {
      if (errno != EAGAIN &&   // timeout
          errno != EINTR )    // interrupted system call
        ACE_ERROR ((LM_ERROR,
                    "%N:%l:(%P | %t)::%p\n",
                    "PosixSigProactor::handle_events:"
                    "sigtimedwait/sigwaitinfo failed"
                  ));
    }
  else if (sig_info.si_signo != result_sigwait)
    {
      // No errors, RT compleion signal is received.
      // Is the signo returned consistent with the sig info?
      ACE_ERROR ((LM_ERROR,
                  "Error:%N:%l:(%P | %t):"
                  "PosixSigProactor::handle_events:"
                  "Inconsistent signal number (%d) in the signal info block",
                   sig_info.si_signo
                ));
    }
  else if (sig_info.si_code == SI_ASYNCIO)
    flg_aio = 1;  // AIO signal received
  else if (sig_info.si_code == SI_QUEUE)
    flg_que = 1;  // SIGQUEUE received
  else
    {
      // Unknown signal code.
      // may some other third-party libraries could send it
      // or message queue could also generate it !
      // So print the message and check our completions
      ACE_ERROR ((LM_DEBUG,
                "%N:%l:(%P | %t):"
                "PosixSigProactor::handle_events:\n"
                "Unexpected signal code (%d) returned on completion querying\n",
                sig_info.si_code));
    }

   // extra actions for different systems
   if (os_id_ == OS_SUN_58)  // Solaris 8
     {
       // Solaris 8 never loses any AIO completion It can store more
       // than 40000 notifications!  So don't waste time to scan all
       // aiocb list We know exactly what finished in case SI_ASYNCHIO

       // But we can easy have lost SI_QUEUE

       if (flg_aio)   // AIO - correct behavior
         count = 1;
       flg_que=1;       // not to miss "post_completed" results
     }
   else if (os_id_ == OS_SUN_56) // Solaris 6
     {
       // 1. Solaris 6 always loses any RT signal,
       //    if it has more SIGQUEMAX=32 pending signals
       //    so we should scan the whole aiocb list
       // 2. Moreover,it has one more bad habit
       //    to notify aio completion
       //    with  SI_QUEUE code instead of SI_ASYNCIO.

       // this is reliable solution
       flg_aio =1;  // always find_completed_aio
       flg_que =1;  // always scan queue
       count = aiocb_list_max_size_;
     }
   else // insert here specific for other systems
     {
       // this is reliable solution
       flg_aio =1;  // always find_completed_aio
       flg_que =1;  // always scan queue
       count = aiocb_list_max_size_;
     }

  // At this point we have
  // if (flg_aio)
  //    scan aiocb list starting with "index" slot
  //    no more "count" times
  //    till we have no more AIO completed
  // if (flg_que)
  //    check "post_completed" queue

  int ret_aio = 0;
  int ret_que = 0;

  if (flg_aio)
    for (;; ret_aio++)
      {
        PosixAsynchResult *asynch_result =
          find_completed_aio (error_status,
                              return_status,
                              index,
                              count);

        if (asynch_result == 0)
          break;

        // Call the application code.
        this->application_specific_code (asynch_result,
                                     return_status, // Bytes transferred.
                                     1,             // Success
                                     0,             // No completion key.
                                     error_status); // Error
      }

  // process post_completed results
  if (flg_que)
    ret_que = this->process_result_queue ();

  // Uncomment this  if you want to test
  // and research the behavior of you system
  // ACE_DEBUG ((LM_DEBUG,
  //            "(%t) NumAIO=%d NumQueue=%d\n",
  //             ret_aio, ret_que));

  return ret_aio + ret_que > 0 ? 1 : 0;
}

// *********************************************************************

PosixAsynchTimer::PosixAsynchTimer (ACE_Handler &handler,
                                                const void *act,
                                                const TimeValue &tv,
                                                ACE_HANDLE event,
                                                int priority,
                                                int signal_number)
  : AsynchResultImpl (),
    PosixAsynchResult (handler, act, event, 0, 0, priority, signal_number),
    time_ (tv)
{
}

void
PosixAsynchTimer::complete (u_long       /* bytes_transferred */,
                                  int          /* success */,
                                  const void * /* completion_key */,
                                  u_long       /* error */)
{
  this->handler_.handle_time_out (this->time_, this->act ());
}

// *********************************************************************

ACE_POSIX_Wakeup_Completion::ACE_POSIX_Wakeup_Completion (ACE_Handler &handler,
                                                          const void *act,
                                                          ACE_HANDLE event,
                                                          int priority,
                                                          int signal_number)
  : AsynchResultImpl (),
    PosixAsynchResult (handler, act, event, 0, 0, priority, signal_number)
{
}

ACE_POSIX_Wakeup_Completion::~ACE_POSIX_Wakeup_Completion (void)
{
}

void
ACE_POSIX_Wakeup_Completion::complete (u_long       /* bytes_transferred */,
                                       int          /* success */,
                                       const void * /* completion_key */,
                                       u_long       /*  error */)
{

  this->handler_.handle_wakeup ();
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Unbounded_Queue<PosixAsynchResult *>;
template class ACE_Node<PosixAsynchResult *>;
template class ACE_Unbounded_Queue_Iterator<PosixAsynchResult *>;

#elif  defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Unbounded_Queue<PosixAsynchResult *>
#pragma instantiate ACE_Node<PosixAsynchResult *>
#pragma instantiate ACE_Unbounded_Queue_Iterator<PosixAsynchResult *>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

//#endif /* ACE_HAS_AIO_CALLS */
