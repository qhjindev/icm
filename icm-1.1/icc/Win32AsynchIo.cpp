// WIN32_Asynch_IO.cpp,v 4.20 2001/08/08 14:24:27 schmidt Exp

#include "icc/Win32AsynchIo.h"

// ACE_RCSID(ace, Asynch_IO, "WIN32_Asynch_IO.cpp,v 4.20 2001/08/08 14:24:27 schmidt Exp")

#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE))

#include "icc/Win32Proactor.h"
#include "icc/MessageBlock.h"
//#include "ace/Service_Config.h"
#include "icc/InetAddr.h"
#include "icc/Task.h"

u_long
Win32AsynchResult::bytes_transferred (void) const
{
  return this->bytes_transferred_;
}

const void *
Win32AsynchResult::act (void) const
{
  return this->act_;
}

int
Win32AsynchResult::success (void) const
{
  return this->success_;
}

const void *
Win32AsynchResult::completion_key (void) const
{
  return this->completion_key_;
}

u_long
Win32AsynchResult::error (void) const
{
  return this->error_;
}

ACE_HANDLE
Win32AsynchResult::event (void) const
{
  return this->hEvent;
}

u_long
Win32AsynchResult::offset (void) const
{
  return this->Offset;
}

u_long
Win32AsynchResult::offset_high (void) const
{
  return this->OffsetHigh;
}

int
Win32AsynchResult::priority (void) const
{
  ACE_NOTSUP_RETURN (0);
}

int
Win32AsynchResult::signal_number (void) const
{
  ACE_NOTSUP_RETURN (0);
}

int
Win32AsynchResult::post_completion (ProactorImpl *proactor)
{
  // Get to the platform specific implementation.
  Win32Proactor *win32_proactor = dynamic_cast <Win32Proactor *> (proactor);

  if (win32_proactor == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_LIB_TEXT ("Dynamic cast to WIN32 Proactor failed\n")),
                       -1);

  // Post myself.
  return win32_proactor->post_completion (this);
}

void
Win32AsynchResult::set_bytes_transferred (u_long nbytes)
{
  this->bytes_transferred_ = nbytes;
}

void
Win32AsynchResult::set_error (u_long errcode)
{
  this->error_ = errcode;
}

Win32AsynchResult::~Win32AsynchResult (void)
{
}

Win32AsynchResult::Win32AsynchResult (Handler &handler,
                                                  const void* act,
                                                  ACE_HANDLE event,
                                                  u_long offset,
                                                  u_long offset_high,
                                                  int priority,
                                                  int signal_number)
  : AsynchResultImpl (),
    OVERLAPPED (),
    handler_ (handler),
    act_ (act),
    bytes_transferred_ (0),
    success_ (0),
    completion_key_ (0),
    error_ (0)
{
  // Set the ACE_OVERLAPPED structure
  this->Internal = 0;
  this->InternalHigh = 0;
  this->Offset = offset;
  this->OffsetHigh = offset_high;
  this->hEvent = event;

  ACE_UNUSED_ARG (priority);
  ACE_UNUSED_ARG (signal_number);
}

int
Win32AsynchOperation::open (Handler &handler,
                                  ACE_HANDLE handle,
                                  const void *completion_key,
                                  Proactor *proactor)
{
  this->proactor_ = proactor;
  this->handler_ = &handler;
  this->handle_ = handle;

  // Grab the handle from the <handler> if <handle> is invalid
  if (this->handle_ == ACE_INVALID_HANDLE)
    this->handle_ = this->handler_->handle ();
  if (this->handle_ == ACE_INVALID_HANDLE)
    return -1;

  // Register with the <proactor>.
  return this->win32_proactor_->register_handle (this->handle_,
                                                 completion_key);
}

int
Win32AsynchOperation::cancel (void)
{
#if (defined (ACE_HAS_WINNT4) && (ACE_HAS_WINNT4 != 0)) \
    && (   (defined (_MSC_VER) && (_MSC_VER > 1020)) \
        || (defined (__BORLANDC__) && (__BORLANDC__ >= 0x530)))
  // All I/O operations that are canceled will complete with the error
  // ERROR_OPERATION_ABORTED. All completion notifications for the I/O
  // operations will occur normally.

  // @@ This API returns 0 on failure. So, I am returning -1 in that
  //    case. Is that right? (Alex).

  int result = (int) ::CancelIo (this->handle_);

  if (result == 0)
    // Couldnt cancel the operations.
    return 2;

  // result is non-zero. All the operations are cancelled then.
  return 0;

#else /* Not ACE_HAS_WINNT4 && ACE_HAS_WINNT4!=0 && _MSC... */
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_AIO_CALLS */
}

Proactor *
Win32AsynchOperation::proactor (void) const
{
  return this->proactor_;
}

Win32AsynchOperation::Win32AsynchOperation (Win32Proactor *win32_proactor)
  : AsynchOperationImpl (),
    win32_proactor_ (win32_proactor),
    proactor_ (0),
    handler_ (0),
    handle_ (ACE_INVALID_HANDLE)
{
}

Win32AsynchOperation::~Win32AsynchOperation (void)
{
}

// ************************************************************

u_long
Win32AsynchReadStreamResult::bytes_to_read (void) const
{
  return this->bytes_to_read_;
}

MessageBlock &
Win32AsynchReadStreamResult::message_block (void) const
{
  return this->message_block_;
}

ACE_HANDLE
Win32AsynchReadStreamResult::handle (void) const
{
  return this->handle_;
}

Win32AsynchReadStreamResult::Win32AsynchReadStreamResult (Handler &handler,
                                                                          ACE_HANDLE handle,
                                                                          MessageBlock &message_block,
                                                                          u_long bytes_to_read,
                                                                          const void* act,
                                                                          ACE_HANDLE event,
                                                                          int priority,
                                                                          int signal_number)
  : AsynchResultImpl (),
    AsynchReadStreamResultImpl (),
    Win32AsynchResult (handler, act, event, 0, 0, priority, signal_number),
    bytes_to_read_ (bytes_to_read),
    message_block_ (message_block),
    handle_ (handle)
{
}

void
Win32AsynchReadStreamResult::complete (u_long bytes_transferred,
                                               int success,
                                               const void *completion_key,
                                               u_long error)
{
  // Copy the data which was returned by GetQueuedCompletionStatus
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // Appropriately move the pointers in the message block.
  this->message_block_.wrPtr (bytes_transferred);

  // Create the interface result class.
  AsynchReadStream::Result result (this);

  // Call the application handler.
  this->handler_.handle_read_stream (result);
}

Win32AsynchReadStreamResult::~Win32AsynchReadStreamResult (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
Win32AsynchReadStreamResult::bytes_transferred (void) const
{
  return Win32AsynchResult::bytes_transferred ();
}

const void *
Win32AsynchReadStreamResult::act (void) const
{
  return Win32AsynchResult::act ();
}

int
Win32AsynchReadStreamResult::success (void) const
{
  return Win32AsynchResult::success ();
}

const void *
Win32AsynchReadStreamResult::completion_key (void) const
{
  return Win32AsynchResult::completion_key ();
}

u_long
Win32AsynchReadStreamResult::error (void) const
{
  return Win32AsynchResult::error ();
}

ACE_HANDLE
Win32AsynchReadStreamResult::event (void) const
{
  return Win32AsynchResult::event ();
}

u_long
Win32AsynchReadStreamResult::offset (void) const
{
  return Win32AsynchResult::offset ();
}

u_long
Win32AsynchReadStreamResult::offset_high (void) const
{
  return Win32AsynchResult::offset_high ();
}

int
Win32AsynchReadStreamResult::priority (void) const
{
  return Win32AsynchResult::priority ();
}

int
Win32AsynchReadStreamResult::signal_number (void) const
{
  return Win32AsynchResult::signal_number ();
}

int
Win32AsynchReadStreamResult::post_completion (ProactorImpl *proactor)
{
  return Win32AsynchResult::post_completion (proactor);
}

Win32AsynchReadStream::Win32AsynchReadStream (Win32Proactor *win32_proactor)
  : AsynchOperationImpl (),
    AsynchReadStreamImpl (),
    Win32AsynchOperation (win32_proactor)
{
}

int
Win32AsynchReadStream::read (MessageBlock &message_block,
                                    u_long bytes_to_read,
                                    const void *act,
                                    int priority,
                                    int signal_number)
{
  // Create the Asynch_Result.
  Win32AsynchReadStreamResult *result = 0;
  ACE_NEW_RETURN (result,
                  Win32AsynchReadStreamResult (*this->handler_,
                                                       this->handle_,
                                                       message_block,
                                                       bytes_to_read,
                                                       act,
                                                       this->win32_proactor_->get_handle (),
                                                       priority,
                                                       signal_number),
                  -1);

  // Shared read
  ssize_t return_val = this->shared_read (result);

  // Upon errors
  if (return_val == -1)
    delete result;

  return return_val;
}

Win32AsynchReadStream::~Win32AsynchReadStream (void)
{
}

int
Win32AsynchReadStream::shared_read (Win32AsynchReadStreamResult *result)
{
  u_long bytes_read;

  result->set_error (0); // Clear error before starting IO.

  // Initiate the read
  int initiate_result = ::ReadFile (result->handle (),
                                    result->message_block ().wrPtr (),
                                    result->bytes_to_read (),
                                    &bytes_read,
                                    result);
  if (initiate_result == 1)
    // Immediate success: the OVERLAPPED will still get queued.
    return 1;

  // If initiate failed, check for a bad error.
  OS::set_errno_to_last_error ();
  switch (errno)
    {
    case ERROR_IO_PENDING:
      // The IO will complete proactively: the OVERLAPPED will still
      // get queued.
      return 0;

    default:
      // Something else went wrong: the OVERLAPPED will not get
      // queued.

      //if (ACE::debug ())
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_LIB_TEXT ("%p\n"),
                      ACE_LIB_TEXT ("ReadFile")));
        }

      return -1;
    }
}

// Methods belong to Win32AsynchOperation base class. These
// methods are defined here to avoid VC++ warnings. They route the
// call to the Win32AsynchOperation base class.

int
Win32AsynchReadStream::open (Handler &handler,
                                    ACE_HANDLE handle,
                                    const void *completion_key,
                                    Proactor *proactor)
{
  return Win32AsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
Win32AsynchReadStream::cancel (void)
{
  return Win32AsynchOperation::cancel ();
}

Proactor *
Win32AsynchReadStream::proactor (void) const
{
  return Win32AsynchOperation::proactor ();
}

u_long
Win32AsynchWriteStreamResult::bytes_to_write (void) const
{
  return this->bytes_to_write_;
}

MessageBlock &
Win32AsynchWriteStreamResult::message_block (void) const
{
  return this->message_block_;
}

ACE_HANDLE
Win32AsynchWriteStreamResult::handle (void) const
{
  return this->handle_;
}

Win32AsynchWriteStreamResult::Win32AsynchWriteStreamResult (Handler &handler,
                                                                            ACE_HANDLE handle,
                                                                            MessageBlock &message_block,
                                                                            u_long bytes_to_write,
                                                                            const void* act,
                                                                            ACE_HANDLE event,
                                                                            int priority,
                                                                            int signal_number)
  : AsynchResultImpl (),
    AsynchWriteStreamResultImpl (),
    Win32AsynchResult (handler, act, event, 0, 0, priority, signal_number),
    bytes_to_write_ (bytes_to_write),
    message_block_ (message_block),
    handle_ (handle)
{
}

void
Win32AsynchWriteStreamResult::complete (u_long bytes_transferred,
                                                int success,
                                                const void *completion_key,
                                                u_long error)
{
  // Copy the data which was returned by <GetQueuedCompletionStatus>.
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // Appropriately move the pointers in the message block.
  this->message_block_.rdPtr (bytes_transferred);

  // Create the interface result class.
  AsynchWriteStream::Result result (this);

  // Call the application handler.
  this->handler_.handle_write_stream (result);
}

Win32AsynchWriteStreamResult::~Win32AsynchWriteStreamResult (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
Win32AsynchWriteStreamResult::bytes_transferred (void) const
{
  return Win32AsynchResult::bytes_transferred ();
}

const void *
Win32AsynchWriteStreamResult::act (void) const
{
  return Win32AsynchResult::act ();
}

int
Win32AsynchWriteStreamResult::success (void) const
{
  return Win32AsynchResult::success ();
}

const void *
Win32AsynchWriteStreamResult::completion_key (void) const
{
  return Win32AsynchResult::completion_key ();
}

u_long
Win32AsynchWriteStreamResult::error (void) const
{
  return Win32AsynchResult::error ();
}

ACE_HANDLE
Win32AsynchWriteStreamResult::event (void) const
{
  return Win32AsynchResult::event ();
}

u_long
Win32AsynchWriteStreamResult::offset (void) const
{
  return Win32AsynchResult::offset ();
}

u_long
Win32AsynchWriteStreamResult::offset_high (void) const
{
  return Win32AsynchResult::offset_high ();
}

int
Win32AsynchWriteStreamResult::priority (void) const
{
  return Win32AsynchResult::priority ();
}

int
Win32AsynchWriteStreamResult::signal_number (void) const
{
  return Win32AsynchResult::signal_number ();
}

int
Win32AsynchWriteStreamResult::post_completion (ProactorImpl *proactor)
{
  return Win32AsynchResult::post_completion (proactor);
}

Win32AsynchWriteStream::Win32AsynchWriteStream (Win32Proactor *win32_proactor)
  : AsynchOperationImpl (),
    AsynchWriteStreamImpl (),
    Win32AsynchOperation (win32_proactor)
{
}

int
Win32AsynchWriteStream::write (MessageBlock &message_block,
                                      u_long bytes_to_write,
                                      const void *act,
                                      int priority,
                                      int signal_number)
{
  Win32AsynchWriteStreamResult *result = 0;
  ACE_NEW_RETURN (result,
                  Win32AsynchWriteStreamResult (*this->handler_,
                                                        this->handle_,
                                                        message_block,
                                                        bytes_to_write,
                                                        act,
                                                        this->win32_proactor_->get_handle (),
                                                        priority,
                                                        signal_number),
                  -1);

  // Shared write
  ssize_t return_val = this->shared_write (result);

  // Upon errors
  if (return_val == -1)
    delete result;

  return return_val;
}

Win32AsynchWriteStream::~Win32AsynchWriteStream (void)
{
}

int
Win32AsynchWriteStream::shared_write (Win32AsynchWriteStreamResult *result)
{
  u_long bytes_written;

  result->set_error (0); // Clear error before starting IO.

  // Initiate the write
  int initiate_result = ::WriteFile (result->handle (),
                                     result->message_block ().rdPtr (),
                                     result->bytes_to_write (),
                                     &bytes_written,
                                     result);
  if (initiate_result == 1)
    // Immediate success: the OVERLAPPED will still get queued.
    return 1;

  // If initiate failed, check for a bad error.
  OS::set_errno_to_last_error ();
  switch (errno)
    {
    case ERROR_IO_PENDING:
      // The IO will complete proactively: the OVERLAPPED will still
      // get queued.
      return 0;

    default:
      // Something else went wrong: the OVERLAPPED will not get
      // queued.

//      if (ACE::debug ())
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_LIB_TEXT ("%p\n"),
                      ACE_LIB_TEXT ("WriteFile")));
        }
      return -1;
    }
}

// Methods belong to Win32AsynchOperation base class. These
// methods are defined here to avoid VC++ warnings. They route the
// call to the Win32AsynchOperation base class.

int
Win32AsynchWriteStream::open (Handler &handler,
                                     ACE_HANDLE handle,
                                     const void *completion_key,
                                     Proactor *proactor)
{
  return Win32AsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
Win32AsynchWriteStream::cancel (void)
{
  return Win32AsynchOperation::cancel ();
}

Proactor *
Win32AsynchWriteStream::proactor (void) const
{
  return Win32AsynchOperation::proactor ();
}

Win32AsynchReadFileResult::Win32AsynchReadFileResult (Handler &handler,
                                                                      ACE_HANDLE handle,
                                                                      MessageBlock &message_block,
                                                                      u_long bytes_to_read,
                                                                      const void* act,
                                                                      u_long offset,
                                                                      u_long offset_high,
                                                                      ACE_HANDLE event,
                                                                      int priority,
                                                                      int signal_number)
  : AsynchResultImpl (),
    AsynchReadStreamResultImpl (),
    AsynchReadFileResultImpl (),
    Win32AsynchReadStreamResult (handler,
                                         handle,
                                         message_block,
                                         bytes_to_read,
                                         act,
                                         event,
                                         priority,
                                         signal_number)
{
  this->Offset = offset;
  this->OffsetHigh = offset_high;
}

void
Win32AsynchReadFileResult::complete (u_long bytes_transferred,
                                             int success,
                                             const void *completion_key,
                                             u_long error)
{
  // Copy the data which was returned by GetQueuedCompletionStatus.
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // Appropriately move the pointers in the message block.
  this->message_block_.wrPtr (bytes_transferred);

  // Create the interface result class.
  AsynchReadFile::Result result (this);

  // Call the application handler.
  this->handler_.handle_read_file (result);
}

Win32AsynchReadFileResult::~Win32AsynchReadFileResult (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
Win32AsynchReadFileResult::bytes_transferred (void) const
{
  return Win32AsynchResult::bytes_transferred ();
}

const void *
Win32AsynchReadFileResult::act (void) const
{
  return Win32AsynchResult::act ();
}

int
Win32AsynchReadFileResult::success (void) const
{
  return Win32AsynchResult::success ();
}

const void *
Win32AsynchReadFileResult::completion_key (void) const
{
  return Win32AsynchResult::completion_key ();
}

u_long
Win32AsynchReadFileResult::error (void) const
{
  return Win32AsynchResult::error ();
}

ACE_HANDLE
Win32AsynchReadFileResult::event (void) const
{
  return Win32AsynchResult::event ();
}

u_long
Win32AsynchReadFileResult::offset (void) const
{
  return Win32AsynchResult::offset ();
}

u_long
Win32AsynchReadFileResult::offset_high (void) const
{
  return Win32AsynchResult::offset_high ();
}

int
Win32AsynchReadFileResult::priority (void) const
{
  return Win32AsynchResult::priority ();
}

int
Win32AsynchReadFileResult::signal_number (void) const
{
  return Win32AsynchResult::signal_number ();
}

// The following methods belong to
// Win32AsynchReadStreamResult. They are here to avoid VC++
// warnings. These methods route their call to the
// Win32AsynchReadStreamResult base class.

u_long
Win32AsynchReadFileResult::bytes_to_read (void) const
{
  return Win32AsynchReadStreamResult::bytes_to_read ();
}

MessageBlock &
Win32AsynchReadFileResult::message_block (void) const
{
  return Win32AsynchReadStreamResult::message_block ();
}

ACE_HANDLE
Win32AsynchReadFileResult::handle (void) const
{
  return Win32AsynchReadStreamResult::handle ();
}

int
Win32AsynchReadFileResult::post_completion (ProactorImpl *proactor)
{
  return Win32AsynchResult::post_completion (proactor);
}

// ************************************************************

Win32AsynchReadFile::Win32AsynchReadFile (Win32Proactor *win32_proactor)
  : AsynchOperationImpl (),
    AsynchReadStreamImpl (),
    AsynchReadFileImpl (),
    Win32AsynchReadStream (win32_proactor)
{
}

int
Win32AsynchReadFile::read (MessageBlock &message_block,
                                  u_long bytes_to_read,
                                  u_long offset,
                                  u_long offset_high,
                                  const void *act,
                                  int priority,
                                  int signal_number)
{
  Win32AsynchReadFileResult *result = 0;
  ACE_NEW_RETURN (result,
                  Win32AsynchReadFileResult (*this->handler_,
                                                     this->handle_,
                                                     message_block,
                                                     bytes_to_read,
                                                     act,
                                                     offset,
                                                     offset_high,
                                                     this->win32_proactor_->get_handle (),
                                                     priority,
                                                     signal_number),
                  -1);

  // Shared read
  ssize_t return_val = this->shared_read (result);

  // Upon errors
  if (return_val == -1)
    delete result;

  return return_val;
}

Win32AsynchReadFile::~Win32AsynchReadFile (void)
{
}

int
Win32AsynchReadFile::read (MessageBlock &message_block,
                                  u_long bytes_to_read,
                                  const void *act,
                                  int priority,
                                  int signal_number)
{
  return Win32AsynchReadStream::read (message_block,
                                             bytes_to_read,
                                             act,
                                             priority,
                                             signal_number);
}

// Methods belong to Win32AsynchOperation base class. These
// methods are defined here to avoid VC++ warnings. They route the
// call to the Win32AsynchOperation base class.

int
Win32AsynchReadFile::open (Handler &handler,
                                  ACE_HANDLE handle,
                                  const void *completion_key,
                                  Proactor *proactor)
{
  return Win32AsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
Win32AsynchReadFile::cancel (void)
{
  return Win32AsynchOperation::cancel ();
}

Proactor *
Win32AsynchReadFile::proactor (void) const
{
  return Win32AsynchOperation::proactor ();
}

Win32AsynchWriteFileResult::Win32AsynchWriteFileResult (Handler &handler,
                                                                        ACE_HANDLE handle,
                                                                        MessageBlock &message_block,
                                                                        u_long bytes_to_write,
                                                                        const void* act,
                                                                        u_long offset,
                                                                        u_long offset_high,
                                                                        ACE_HANDLE event,
                                                                        int priority,
                                                                        int signal_number)
  : AsynchResultImpl (),
    AsynchWriteStreamResultImpl (),
    AsynchWriteFileResultImpl (),
    Win32AsynchWriteStreamResult (handler,
                                          handle,
                                          message_block,
                                          bytes_to_write,
                                          act,
                                          event,
                                          priority,
                                          signal_number)
{
  this->Offset = offset;
  this->OffsetHigh = offset_high;
}

void
Win32AsynchWriteFileResult::complete (u_long bytes_transferred,
                                              int success,
                                              const void *completion_key,
                                              u_long error)
{
  // Copy the data which was returned by GetQueuedCompletionStatus
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // Appropriately move the pointers in the message block.
  this->message_block_.rdPtr (bytes_transferred);

  // Create the interface result class.
  AsynchWriteFile::Result result (this);

  // Call the application handler.
  this->handler_.handle_write_file (result);
}

Win32AsynchWriteFileResult::~Win32AsynchWriteFileResult  (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
Win32AsynchWriteFileResult::bytes_transferred (void) const
{
  return Win32AsynchResult::bytes_transferred ();
}

const void *
Win32AsynchWriteFileResult::act (void) const
{
  return Win32AsynchResult::act ();
}

int
Win32AsynchWriteFileResult::success (void) const
{
  return Win32AsynchResult::success ();
}

const void *
Win32AsynchWriteFileResult::completion_key (void) const
{
  return Win32AsynchResult::completion_key ();
}

u_long
Win32AsynchWriteFileResult::error (void) const
{
  return Win32AsynchResult::error ();
}

ACE_HANDLE
Win32AsynchWriteFileResult::event (void) const
{
  return Win32AsynchResult::event ();
}

u_long
Win32AsynchWriteFileResult::offset (void) const
{
  return Win32AsynchResult::offset ();
}

u_long
Win32AsynchWriteFileResult::offset_high (void) const
{
  return Win32AsynchResult::offset_high ();
}

int
Win32AsynchWriteFileResult::priority (void) const
{
  return Win32AsynchResult::priority ();
}

int
Win32AsynchWriteFileResult::signal_number (void) const
{
  return Win32AsynchResult::signal_number ();
}

// The following methods belong to
// Win32AsynchWriteStreamResult. They are here to avoid VC++
// warnings. These methods route their call to the
// Win32AsynchWriteStreamResult base class.

u_long
Win32AsynchWriteFileResult::bytes_to_write (void) const
{
  return Win32AsynchWriteStreamResult::bytes_to_write ();
}

MessageBlock &
Win32AsynchWriteFileResult::message_block (void) const
{
  return Win32AsynchWriteStreamResult::message_block ();
}

ACE_HANDLE
Win32AsynchWriteFileResult::handle (void) const
{
  return Win32AsynchWriteStreamResult::handle ();
}

int
Win32AsynchWriteFileResult::post_completion (ProactorImpl *proactor)
{
  return Win32AsynchResult::post_completion (proactor);
}

Win32AsynchWriteFile::Win32AsynchWriteFile (Win32Proactor *win32_proactor)
  : AsynchOperationImpl (),
    AsynchWriteStreamImpl (),
    AsynchWriteFileImpl (),
    Win32AsynchWriteStream (win32_proactor)
{
}

int
Win32AsynchWriteFile::write (MessageBlock &message_block,
                                    u_long bytes_to_write,
                                    u_long offset,
                                    u_long offset_high,
                                    const void *act,
                                    int priority,
                                    int signal_number)
{
  Win32AsynchWriteFileResult *result = 0;
  ACE_NEW_RETURN (result,
                  Win32AsynchWriteFileResult (*this->handler_,
                                                      this->handle_,
                                                      message_block,
                                                      bytes_to_write,
                                                      act,
                                                      offset,
                                                      offset_high,
                                                      this->win32_proactor_->get_handle (),
                                                      priority,
                                                      signal_number),
                  -1);

  // Shared write
  ssize_t return_val = this->shared_write (result);

  // Upon errors
  if (return_val == -1)
    delete result;

  return return_val;
}

Win32AsynchWriteFile::~Win32AsynchWriteFile (void)
{
}

int
Win32AsynchWriteFile::write (MessageBlock &message_block,
                                    u_long bytes_to_write,
                                    const void *act,
                                    int priority,
                                    int signal_number)
{
  return Win32AsynchWriteStream::write (message_block,
                                               bytes_to_write,
                                               act,
                                               priority,
                                               signal_number);
}

// Methods belong to Win32AsynchOperation base class. These
// methods are defined here to avoid VC++ warnings. They route the
// call to the Win32AsynchOperation base class.

int
Win32AsynchWriteFile::open (Handler &handler,
                                   ACE_HANDLE handle,
                                   const void *completion_key,
                                   Proactor *proactor)
{
  return Win32AsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
Win32AsynchWriteFile::cancel (void)
{
  return Win32AsynchOperation::cancel ();
}

Proactor *
Win32AsynchWriteFile::proactor (void) const
{
  return Win32AsynchOperation::proactor ();
}

#endif /* ACE_WIN32 || ACE_HAS_WINCE */
