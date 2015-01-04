/* -*- C++ -*- */
// POSIX_Asynch_IO.cpp,v 4.36 2001/07/02 01:03:11 schmidt Exp

#include "icc/PosixAsynchIo.h"

//#if defined (ACE_HAS_AIO_CALLS)

#include "icc/Proactor.h"
#include "icc/MessageBlock.h"
#include "icc/InetAddr.h"
#include "icc/Task.h"
#include "icc/PosixProactor.h"

u_long
PosixAsynchResult::bytes_transferred (void) const
{
  return this->bytes_transferred_;
}

void
PosixAsynchResult::set_bytes_transferred (u_long nbytes)
{
  this->bytes_transferred_= nbytes;
}

const void *
PosixAsynchResult::act (void) const
{
  return this->act_;
}

int
PosixAsynchResult::success (void) const
{
  return this->success_;
}

const void *
PosixAsynchResult::completion_key (void) const
{
  return this->completion_key_;
}

u_long
PosixAsynchResult::error (void) const
{
  return this->error_;
}

void
PosixAsynchResult::set_error (u_long errcode)
{
  this->error_=errcode;
}
ACE_HANDLE
PosixAsynchResult::event (void) const
{
  return ACE_INVALID_HANDLE;
}

u_long
PosixAsynchResult::offset (void) const
{
  return this->aio_offset;
}

u_long
PosixAsynchResult::offset_high (void) const
{
  //
  // @@ Support aiocb64??
  //
  ACE_NOTSUP_RETURN (0);
}

int
PosixAsynchResult::priority (void) const
{
  return this->aio_reqprio;
}

int
PosixAsynchResult::signal_number (void) const
{
  return this->aio_sigevent.sigev_signo;
}

int
PosixAsynchResult::post_completion (ProactorImpl *proactor_impl)
{
  // Get to the platform specific implementation.
  PosixProactor *posix_proactor = ACE_dynamic_cast (PosixProactor *,
                                                         proactor_impl);

  if (posix_proactor == 0)
    ACE_ERROR_RETURN ((LM_ERROR, "Dynamic cast to POSIX Proactor failed\n"), -1);

  // Post myself.
  return posix_proactor->post_completion (this);
}

PosixAsynchResult::~PosixAsynchResult (void)
{
}

PosixAsynchResult::PosixAsynchResult (ACE_Handler &handler,
                                                  const void* act,
                                                  ACE_HANDLE event,
                                                  u_long offset,
                                                  u_long offset_high,
                                                  int priority,
                                                  int signal_number)
  : AsynchResultImpl (),
    aiocb (),
    handler_ (handler),
    act_ (act),
    bytes_transferred_ (0),
    success_ (0),
    completion_key_ (0),
    error_ (0)
{
  aio_offset = offset;
  aio_reqprio = priority;
  aio_sigevent.sigev_signo = signal_number;

  // Event is not used on POSIX.
  ACE_UNUSED_ARG (event);

  //
  // @@ Support offset_high with aiocb64.
  //
  ACE_UNUSED_ARG (offset_high);

  // Other fields in the <aiocb> will be initialized by the
  // subclasses.
}

// ****************************************************************

int
PosixAsynchOperation::open (ACE_Handler &handler,
                                  ACE_HANDLE handle,
                                  const void *completion_key,
                                  ACE_Proactor *proactor)
{
  this->proactor_ = proactor;
  this->handler_ = &handler;
  this->handle_ = handle;

  // Grab the handle from the <handler> if <handle> is invalid
  if (this->handle_ == ACE_INVALID_HANDLE)
    this->handle_ = this->handler_->handle ();
  if (this->handle_ == ACE_INVALID_HANDLE)
    return -1;

#if 0
  // @@ If <proactor> is 0, let us not bother about getting this
  // Proactor, we have already got the specific implementation
  // Proactor.

  // If no proactor was passed
  if (this->proactor_ == 0)
    {
      // Grab the proactor from the <Service_Config> if
      // <handler->proactor> is zero
      this->proactor_ = this->handler_->proactor ();
      if (this->proactor_ == 0)
        this->proactor_ = ACE_Proactor::instance();
    }
#endif /* 0 */

  // AIO stuff is present. So no registering.
  ACE_UNUSED_ARG (completion_key);
  return 0;
}

int
PosixAsynchOperation::cancel (void)
{
  ACE_Proactor *p = this->proactor () ;

  if (!p)
    return -1;

  PosixProactor * p_impl = ACE_dynamic_cast
    (PosixProactor *,
     p->implementation ());
  if (!p_impl)
    return -1;

  // For ACE_SUN_Proactor and PosixAiocbProactor
  // and PosixSigProactor now !
  // we should call  cancel_aio (this->handle_)
  // method to cancel correctly all deferred AIOs

  switch (p_impl->get_impl_type ())
  {
    case PosixProactor::PROACTOR_SUN:
    case PosixProactor::PROACTOR_AIOCB:
    case PosixProactor::PROACTOR_SIG:
      {
        PosixAiocbProactor * p_impl_aiocb = ACE_dynamic_cast
          (PosixAiocbProactor *,
           p_impl);

        if (! p_impl_aiocb)
           return -1;

        return p_impl_aiocb->cancel_aio (this->handle_);
      }
    default:
      break;
  }

  return -1;
}

ACE_Proactor *
PosixAsynchOperation::proactor (void) const
{
  return this->proactor_;
}

PosixAsynchOperation::~PosixAsynchOperation (void)
{
}

PosixAsynchOperation::PosixAsynchOperation (void)
  : AsynchOperationImpl (),
    handler_ (0),
    handle_ (ACE_INVALID_HANDLE)
{
}

// *********************************************************************

PosixAiocbProactor *
PosixAiocbAsynchOperation::posix_proactor (void) const
{
  return this->posix_aiocb_proactor_;
}

PosixAiocbAsynchOperation::PosixAiocbAsynchOperation (PosixAiocbProactor *posix_aiocb_proactor)
  : AsynchOperationImpl (),
    PosixAsynchOperation (),
    posix_aiocb_proactor_ (posix_aiocb_proactor)
{
}

PosixAiocbAsynchOperation::~PosixAiocbAsynchOperation (void)
{
}

int
PosixAiocbAsynchOperation::register_and_start_aio (PosixAsynchResult *result,
                                                          int op)
{
  return this->posix_proactor ()->register_and_start_aio (result, op);
}

// *********************************************************************

u_long
PosixAsynchReadStreamResult::bytes_to_read (void) const
{
  return this->aio_nbytes;
}

MessageBlock &
PosixAsynchReadStreamResult::message_block (void) const
{
  return this->message_block_;
}

ACE_HANDLE
PosixAsynchReadStreamResult::handle (void) const
{
  return this->aio_fildes;
}

PosixAsynchReadStreamResult::PosixAsynchReadStreamResult (ACE_Handler &handler,
                                                                          ACE_HANDLE handle,
                                                                          MessageBlock &message_block,
                                                                          u_long bytes_to_read,
                                                                          const void* act,
                                                                          ACE_HANDLE event,
                                                                          int priority,
                                                                          int signal_number)
  : AsynchResultImpl (),
    AsynchReadStreamResultImpl (),
    PosixAsynchResult (handler, act, event, 0, 0, priority, signal_number),
    message_block_ (message_block)
{
  this->aio_fildes = handle;
  this->aio_buf = message_block.wrPtr ();
  this->aio_nbytes = bytes_to_read;
  ACE_UNUSED_ARG (event);
}

void
PosixAsynchReadStreamResult::complete (u_long bytes_transferred,
                                               int success,
                                               const void *completion_key,
                                               u_long error)
{
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // <errno> is available in the aiocb.
  ACE_UNUSED_ARG (error);

  // Appropriately move the pointers in the message block.
  this->message_block_.wrPtr (bytes_transferred);

  // Create the interface result class.
  ACE_Asynch_Read_Stream::Result result (this);

  // Call the application handler.
  this->handler_.handle_read_stream (result);
}

PosixAsynchReadStreamResult::~PosixAsynchReadStreamResult (void)
{
}

// = Base class operations. These operations are here to kill
//   dominance warnings. These methods call the base class methods.

u_long
PosixAsynchReadStreamResult::bytes_transferred (void) const
{
  return PosixAsynchResult::bytes_transferred ();
}

const void *
PosixAsynchReadStreamResult::act (void) const
{
  return PosixAsynchResult::act ();
}

int
PosixAsynchReadStreamResult::success (void) const
{
  return PosixAsynchResult::success ();
}

const void *
PosixAsynchReadStreamResult::completion_key (void) const
{
  return PosixAsynchResult::completion_key ();
}

u_long
PosixAsynchReadStreamResult::error (void) const
{
  return PosixAsynchResult::error ();
}

ACE_HANDLE
PosixAsynchReadStreamResult::event (void) const
{
  return PosixAsynchResult::event ();
}

u_long
PosixAsynchReadStreamResult::offset (void) const
{
  return PosixAsynchResult::offset ();
}

u_long
PosixAsynchReadStreamResult::offset_high (void) const
{
  return PosixAsynchResult::offset_high ();
}

int
PosixAsynchReadStreamResult::priority (void) const
{
  return PosixAsynchResult::priority ();
}

int
PosixAsynchReadStreamResult::signal_number (void) const
{
  return PosixAsynchResult::signal_number ();
}

int
PosixAsynchReadStreamResult::post_completion (ProactorImpl *proactor)
{
  return PosixAsynchResult::post_completion (proactor);
}

// ************************************************************

PosixAiocbAsynchReadStream::PosixAiocbAsynchReadStream (PosixAiocbProactor  *posix_aiocb_proactor)
  : AsynchOperationImpl (),
    AsynchReadStreamImpl (),
    PosixAiocbAsynchOperation (posix_aiocb_proactor)
{
}

int
PosixAiocbAsynchReadStream::read (MessageBlock &message_block,
                                          u_long bytes_to_read,
                                          const void *act,
                                          int priority,
                                          int signal_number)
{
  // Create the Asynch_Result.
  PosixAsynchReadStreamResult *result = 0;
  ACE_NEW_RETURN (result,
                  PosixAsynchReadStreamResult (*this->handler_,
                                                       this->handle_,
                                                       message_block,
                                                       bytes_to_read,
                                                       act,
                                                       this->posix_proactor ()->get_handle (),
                                                       priority,
                                                       signal_number),
                  -1);

  // we do not need shared_read anymore
  //ssize_t return_val = this->shared_read (result);

  // try to start read
  // we will setup aio_sigevent later
  // in ACE_POSIX_AIOCB/SIG_Proactor::register_and_start_aio ()

  ssize_t return_val = this->register_and_start_aio (result, 0);

  if (return_val == -1)
    delete result;

  return return_val;
}

PosixAiocbAsynchReadStream::~PosixAiocbAsynchReadStream (void)
{
}

//int
//PosixAiocbAsynchReadStream::shared_read (PosixAsynchReadStreamResult *result)
//{
//
// result->aio_sigevent.sigev_notify = SIGEV_NONE;
//
//  // try start read
//  return register_and_start_aio (result, 0);
//}

// Methods belong to PosixAsynchOperation base class. These
// methods are defined here to avoid dominance warnings. They route
// the call to the PosixAsynchOperation base class.

int
PosixAiocbAsynchReadStream::open (ACE_Handler &handler,
                                          ACE_HANDLE handle,
                                          const void *completion_key,
                                          ACE_Proactor *proactor)
{
  return PosixAsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
PosixAiocbAsynchReadStream::cancel (void)
{
  return PosixAsynchOperation::cancel ();
}

ACE_Proactor *
PosixAiocbAsynchReadStream::proactor (void) const
{
  return PosixAsynchOperation::proactor ();
}

// *********************************************************************

u_long
PosixAsynchWriteStreamResult::bytes_to_write (void) const
{
  return this->aio_nbytes;
}

MessageBlock &
PosixAsynchWriteStreamResult::message_block (void) const
{
  return this->message_block_;
}

ACE_HANDLE
PosixAsynchWriteStreamResult::handle (void) const
{
  return this->aio_fildes;
}

PosixAsynchWriteStreamResult::PosixAsynchWriteStreamResult (ACE_Handler &handler,
                                                                            ACE_HANDLE handle,
                                                                            MessageBlock &message_block,
                                                                            u_long bytes_to_write,
                                                                            const void* act,
                                                                            ACE_HANDLE event,
                                                                            int priority,
                                                                            int signal_number)
  : AsynchResultImpl (),
    AsynchWriteStreamResultImpl (),
    PosixAsynchResult (handler, act, event, 0, 0, priority, signal_number),
    message_block_ (message_block)
{
  this->aio_fildes = handle;
  this->aio_buf = message_block.rdPtr ();
  this->aio_nbytes = bytes_to_write;
  ACE_UNUSED_ARG (event);
}

void
PosixAsynchWriteStreamResult::complete (u_long bytes_transferred,
                                                int success,
                                                const void *completion_key,
                                                u_long error)
{
  // Get all the data copied.
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // <errno> is available in the aiocb.
  ACE_UNUSED_ARG (error);

  // Appropriately move the pointers in the message block.
  this->message_block_.rdPtr (bytes_transferred);

  // Create the interface result class.
  ACE_Asynch_Write_Stream::Result result (this);

  // Call the application handler.
  this->handler_.handle_write_stream (result);
}

PosixAsynchWriteStreamResult::~PosixAsynchWriteStreamResult (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
PosixAsynchWriteStreamResult::bytes_transferred (void) const
{
  return PosixAsynchResult::bytes_transferred ();
}

const void *
PosixAsynchWriteStreamResult::act (void) const
{
  return PosixAsynchResult::act ();
}

int
PosixAsynchWriteStreamResult::success (void) const
{
  return PosixAsynchResult::success ();
}

const void *
PosixAsynchWriteStreamResult::completion_key (void) const
{
  return PosixAsynchResult::completion_key ();
}

u_long
PosixAsynchWriteStreamResult::error (void) const
{
  return PosixAsynchResult::error ();
}

ACE_HANDLE
PosixAsynchWriteStreamResult::event (void) const
{
  return PosixAsynchResult::event ();
}

u_long
PosixAsynchWriteStreamResult::offset (void) const
{
  return PosixAsynchResult::offset ();
}

u_long
PosixAsynchWriteStreamResult::offset_high (void) const
{
  return PosixAsynchResult::offset_high ();
}

int
PosixAsynchWriteStreamResult::priority (void) const
{
  return PosixAsynchResult::priority ();
}

int
PosixAsynchWriteStreamResult::signal_number (void) const
{
  return PosixAsynchResult::signal_number ();
}

int
PosixAsynchWriteStreamResult::post_completion (ProactorImpl *proactor)
{
  return PosixAsynchResult::post_completion (proactor);
}

// *********************************************************************

PosixAiocbAsynchWriteStream::PosixAiocbAsynchWriteStream (PosixAiocbProactor *posix_aiocb_proactor)
  : AsynchOperationImpl (),
    AsynchWriteStreamImpl (),
    PosixAiocbAsynchOperation (posix_aiocb_proactor)
{
}

int
PosixAiocbAsynchWriteStream::write (MessageBlock &message_block,
                                            u_long bytes_to_write,
                                            const void *act,
                                            int priority,
                                            int signal_number)
{
  PosixAsynchWriteStreamResult *result = 0;
  ACE_NEW_RETURN (result,
                  PosixAsynchWriteStreamResult (*this->handler_,
                                                        this->handle_,
                                                        message_block,
                                                        bytes_to_write,
                                                        act,
                                                        this->posix_proactor ()->get_handle (),
                                                        priority,
                                                        signal_number),
                  -1);

  // we do not need shared_write anymore
  //ssize_t return_val = this->shared_write (result);

  // try to start write
  // we will setup aio_sigevent later
  // in ACE_POSIX_AIOCB/SIG_Proactor::register_and_start_aio ()

  ssize_t return_val = this->register_and_start_aio (result, 1);

  if (return_val == -1)
    delete result;

  return return_val;
}

PosixAiocbAsynchWriteStream::~PosixAiocbAsynchWriteStream (void)
{
}

//int
//PosixAiocbAsynchWriteStream::shared_write (PosixAsynchWriteStreamResult *result)
//{
//
//   result->aio_sigevent.sigev_notify = SIGEV_NONE;
//
//  // try start write
//  return register_and_start_aio (result, 1);
//}

// Methods belong to PosixAsynchOperation base class. These
// methods are defined here to avoid dominance warnings. They route
// the call to the PosixAsynchOperation base class.

int
PosixAiocbAsynchWriteStream::open (ACE_Handler &handler,
                                     ACE_HANDLE handle,
                                     const void *completion_key,
                                     ACE_Proactor *proactor)
{
  return PosixAsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
PosixAiocbAsynchWriteStream::cancel (void)
{
  return PosixAsynchOperation::cancel ();
}

ACE_Proactor *
PosixAiocbAsynchWriteStream::proactor (void) const
{
  return PosixAsynchOperation::proactor ();
}

// *********************************************************************


PosixAsynchReadFileResult::PosixAsynchReadFileResult (ACE_Handler &handler,
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
    PosixAsynchReadStreamResult (handler,
                                         handle,
                                         message_block,
                                         bytes_to_read,
                                         act,
                                         event,
                                         priority,
                                         signal_number)
{
  this->aio_offset = offset;
  //
  // @@ Use aiocb64??
  //
  ACE_UNUSED_ARG (offset_high);
}

void
PosixAsynchReadFileResult::complete (u_long bytes_transferred,
                                             int success,
                                             const void *completion_key,
                                             u_long error)
{
  // Copy all the data.
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // <errno> is available in the aiocb.
  ACE_UNUSED_ARG (error);

  // Appropriately move the pointers in the message block.
  this->message_block_.wrPtr (bytes_transferred);

  // Create the interface result class.
  ACE_Asynch_Read_File::Result result (this);

  // Call the application handler.
  this->handler_.handle_read_file (result);
}

PosixAsynchReadFileResult::~PosixAsynchReadFileResult (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
PosixAsynchReadFileResult::bytes_transferred (void) const
{
  return PosixAsynchResult::bytes_transferred ();
}

const void *
PosixAsynchReadFileResult::act (void) const
{
  return PosixAsynchResult::act ();
}

int
PosixAsynchReadFileResult::success (void) const
{
  return PosixAsynchResult::success ();
}

const void *
PosixAsynchReadFileResult::completion_key (void) const
{
  return PosixAsynchResult::completion_key ();
}

u_long
PosixAsynchReadFileResult::error (void) const
{
  return PosixAsynchResult::error ();
}

ACE_HANDLE
PosixAsynchReadFileResult::event (void) const
{
  return PosixAsynchResult::event ();
}

u_long
PosixAsynchReadFileResult::offset (void) const
{
  return PosixAsynchResult::offset ();
}

u_long
PosixAsynchReadFileResult::offset_high (void) const
{
  return PosixAsynchResult::offset_high ();
}

int
PosixAsynchReadFileResult::priority (void) const
{
  return PosixAsynchResult::priority ();
}

int
PosixAsynchReadFileResult::signal_number (void) const
{
  return PosixAsynchResult::signal_number ();
}

// The following methods belong to
// PosixAsynchReadStreamResult. They are here to avoid
// dominace warnings. These methods route their call to the
// PosixAsynchReadStreamResult base class.

u_long
PosixAsynchReadFileResult::bytes_to_read (void) const
{
  return PosixAsynchReadStreamResult::bytes_to_read ();
}

MessageBlock &
PosixAsynchReadFileResult::message_block (void) const
{
  return PosixAsynchReadStreamResult::message_block ();
}

ACE_HANDLE
PosixAsynchReadFileResult::handle (void) const
{
  return PosixAsynchReadStreamResult::handle ();
}

int
PosixAsynchReadFileResult::post_completion (ProactorImpl *proactor)
{
  return PosixAsynchResult::post_completion (proactor);
}

// *********************************************************************

PosixAiocbAsynchReadFile::PosixAiocbAsynchReadFile (PosixAiocbProactor *posix_aiocb_proactor)
  : AsynchOperationImpl (),
    AsynchReadStreamImpl (),
    AsynchReadFileImpl (),
    PosixAiocbAsynchReadStream (posix_aiocb_proactor)
{
}

int
PosixAiocbAsynchReadFile::read (MessageBlock &message_block,
                                        u_long bytes_to_read,
                                        u_long offset,
                                        u_long offset_high,
                                        const void *act,
                                        int priority,
                                        int signal_number)
{
  PosixAsynchReadFileResult *result = 0;
  ACE_NEW_RETURN (result,
                  PosixAsynchReadFileResult (*this->handler_,
                                                     this->handle_,
                                                     message_block,
                                                     bytes_to_read,
                                                     act,
                                                     offset,
                                                     offset_high,
                                                     this->posix_proactor ()->get_handle (),
                                                     priority,
                                                     signal_number),
                  -1);

  //ssize_t return_val = this->shared_read (result);
  ssize_t return_val = this->register_and_start_aio (result, 0);

  if (return_val == -1)
    delete result;

  return return_val;
}

PosixAiocbAsynchReadFile::~PosixAiocbAsynchReadFile (void)
{
}

int
PosixAiocbAsynchReadFile::read (MessageBlock &message_block,
                                        u_long bytes_to_read,
                                        const void *act,
                                        int priority,
                                        int signal_number)
{
  return PosixAiocbAsynchReadStream::read (message_block,
                                                   bytes_to_read,
                                                   act,
                                                   priority,
                                                   signal_number);
}

// Methods belong to PosixAsynchOperation base class. These
// methods are defined here to avoid dominance warnings. They route
// the call to the PosixAsynchOperation base class.

int
PosixAiocbAsynchReadFile::open (ACE_Handler &handler,
                                        ACE_HANDLE handle,
                                        const void *completion_key,
                                        ACE_Proactor *proactor)
{
  return PosixAsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
PosixAiocbAsynchReadFile::cancel (void)
{
  return PosixAsynchOperation::cancel ();
}

ACE_Proactor *
PosixAiocbAsynchReadFile::proactor (void) const
{
  return PosixAsynchOperation::proactor ();
}

// ************************************************************

PosixAsynchWriteFileResult::PosixAsynchWriteFileResult (ACE_Handler &handler,
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
    PosixAsynchWriteStreamResult (handler,
                                          handle,
                                          message_block,
                                          bytes_to_write,
                                          act,
                                          event,
                                          priority,
                                          signal_number)
{
  this->aio_offset = offset;
  //
  // @@ Support offset_high with aiocb64.
  //
  ACE_UNUSED_ARG (offset_high);
}

void
PosixAsynchWriteFileResult::complete (u_long bytes_transferred,
                                              int success,
                                              const void *completion_key,
                                              u_long error)
{
  // Copy the data.
  this->bytes_transferred_ = bytes_transferred;
  this->success_ = success;
  this->completion_key_ = completion_key;
  this->error_ = error;

  // <error> is available in <aio_resultp.aio_error>
  ACE_UNUSED_ARG (error);

  // Appropriately move the pointers in the message block.
  this->message_block_.rdPtr (bytes_transferred);

  // Create the interface result class.
  ACE_Asynch_Write_File::Result result (this);

  // Call the application handler.
  this->handler_.handle_write_file (result);
}

PosixAsynchWriteFileResult::~PosixAsynchWriteFileResult  (void)
{
}

// Base class operations. These operations are here to kill dominance
// warnings. These methods call the base class methods.

u_long
PosixAsynchWriteFileResult::bytes_transferred (void) const
{
  return PosixAsynchResult::bytes_transferred ();
}

const void *
PosixAsynchWriteFileResult::act (void) const
{
  return PosixAsynchResult::act ();
}

int
PosixAsynchWriteFileResult::success (void) const
{
  return PosixAsynchResult::success ();
}

const void *
PosixAsynchWriteFileResult::completion_key (void) const
{
  return PosixAsynchResult::completion_key ();
}

u_long
PosixAsynchWriteFileResult::error (void) const
{
  return PosixAsynchResult::error ();
}

ACE_HANDLE
PosixAsynchWriteFileResult::event (void) const
{
  return PosixAsynchResult::event ();
}

u_long
PosixAsynchWriteFileResult::offset (void) const
{
  return PosixAsynchResult::offset ();
}

u_long
PosixAsynchWriteFileResult::offset_high (void) const
{
  return PosixAsynchResult::offset_high ();
}

int
PosixAsynchWriteFileResult::priority (void) const
{
  return PosixAsynchResult::priority ();
}

int
PosixAsynchWriteFileResult::signal_number (void) const
{
  return PosixAsynchResult::signal_number ();
}

// The following methods belong to
// PosixAsynchWriteStreamResult. They are here to avoid
// dominance warnings. These methods route their call to the
// PosixAsynchWriteStreamResult base class.

u_long
PosixAsynchWriteFileResult::bytes_to_write (void) const
{
  return PosixAsynchWriteStreamResult::bytes_to_write ();
}

MessageBlock &
PosixAsynchWriteFileResult::message_block (void) const
{
  return PosixAsynchWriteStreamResult::message_block ();
}

ACE_HANDLE
PosixAsynchWriteFileResult::handle (void) const
{
  return PosixAsynchWriteStreamResult::handle ();
}

int
PosixAsynchWriteFileResult::post_completion (ProactorImpl *proactor)
{
  return PosixAsynchResult::post_completion (proactor);
}

// *********************************************************************

PosixAiocbAsynchWriteFile::PosixAiocbAsynchWriteFile (PosixAiocbProactor *posix_aiocb_proactor)
  : AsynchOperationImpl (),
    AsynchWriteStreamImpl (),
    ACE_Asynch_Write_File_Impl (),
    PosixAiocbAsynchWriteStream (posix_aiocb_proactor)
{
}

int
PosixAiocbAsynchWriteFile::write (MessageBlock &message_block,
                                          u_long bytes_to_write,
                                          u_long offset,
                                          u_long offset_high,
                                          const void *act,
                                          int priority,
                                          int signal_number)
{
  PosixAsynchWriteFileResult *result = 0;
  ACE_NEW_RETURN (result,
                  PosixAsynchWriteFileResult (*this->handler_,
                                                      this->handle_,
                                                      message_block,
                                                      bytes_to_write,
                                                      act,
                                                      offset,
                                                      offset_high,
                                                      this->posix_proactor ()->get_handle (),
                                                      priority,
                                                      signal_number),
                  -1);

  //ssize_t return_val = this->shared_write (result);
  ssize_t return_val = this->register_and_start_aio (result, 1);

  if (return_val == -1)
    delete result;

  return return_val;
}

PosixAiocbAsynchWriteFile::~PosixAiocbAsynchWriteFile (void)
{
}

int
PosixAiocbAsynchWriteFile::write (MessageBlock &message_block,
                                          u_long bytes_to_write,
                                          const void *act,
                                          int priority,
                                          int signal_number)
{
  return PosixAiocbAsynchWriteStream::write (message_block,
                                                     bytes_to_write,
                                                     act,
                                                     priority,
                                                     signal_number);
}

// Methods belong to PosixAsynchOperation base class. These
// methods are defined here to avoid dominance warnings. They route
// the call to the PosixAsynchOperation base class.

int
PosixAiocbAsynchWriteFile::open (ACE_Handler &handler,
                                   ACE_HANDLE handle,
                                   const void *completion_key,
                                   ACE_Proactor *proactor)
{
  return PosixAsynchOperation::open (handler,
                                           handle,
                                           completion_key,
                                           proactor);
}

int
PosixAiocbAsynchWriteFile::cancel (void)
{
  return PosixAsynchOperation::cancel ();
}

ACE_Proactor *
PosixAiocbAsynchWriteFile::proactor (void) const
{
  return PosixAsynchOperation::proactor ();
}


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Unbounded_Queue<ACE_POSIX_Asynch_Accept_Result *>;
template class ACE_Node<ACE_POSIX_Asynch_Accept_Result *>;
template class ACE_Unbounded_Queue_Iterator<ACE_POSIX_Asynch_Accept_Result *>;
#elif  defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Unbounded_Queue<ACE_POSIX_Asynch_Accept_Result *>
#pragma instantiate ACE_Node<ACE_POSIX_Asynch_Accept_Result *>
#pragma instantiate ACE_Unbounded_Queue_Iterator<ACE_POSIX_Asynch_Accept_Result *>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */


//#endif /* ACE_HAS_AIO_CALLS */
