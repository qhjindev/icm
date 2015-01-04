// Asynch_IO.cpp,v 4.55 2001/05/17 12:08:25 schmidt Exp

#include "icc/AsynchIo.h"


#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS))
// This only works on platforms with Asynchronous IO

#include "icc/Proactor.h"
#include "icc/MessageBlock.h"
//#include "icc/InetAddr.h"
#include "icc/AsynchIoImpl.h"

u_long
AsynchResult::bytes_transferred (void) const
{
  return this->implementation ()->bytes_transferred ();
}

const void *
AsynchResult::act (void) const
{
  return this->implementation ()->act ();
}

int
AsynchResult::success (void) const
{
  return this->implementation ()->success ();
}

const void *
AsynchResult::completion_key (void) const
{
  return this->implementation ()->completion_key ();
}

u_long
AsynchResult::error (void) const
{
  return this->implementation ()->error ();
}

ACE_HANDLE
AsynchResult::event (void) const
{
  return this->implementation ()->event ();
}

u_long
AsynchResult::offset (void) const
{
  return this->implementation ()->offset ();
}

u_long
AsynchResult::offset_high (void) const
{
  return this->implementation ()->offset_high ();
}

int
AsynchResult::priority (void) const
{
  return this->implementation ()->priority ();
}

int
AsynchResult::signal_number (void) const
{
  return this->implementation ()->signal_number ();
}

AsynchResult::AsynchResult (AsynchResultImpl *implementation)
  : implementation_ (implementation)
{
}

AsynchResult::~AsynchResult (void)
{
  // Proactor deletes the implementation when the <complete> finishes.  
}

AsynchResultImpl *
AsynchResult::implementation (void) const
{
  return this->implementation_;
}

// *********************************************************************

int
AsynchOperation::open (Handler &handler,
                            ACE_HANDLE handle,
                            const void *completion_key,
                            Proactor *proactor)
{
  return this->implementation ()->open (handler,
                                        handle,
                                        completion_key,
                                        proactor);
}

int
AsynchOperation::cancel (void)
{
  return this->implementation ()->cancel ();
}

Proactor *
AsynchOperation::proactor (void) const
{
  return this->implementation ()->proactor ();
}

AsynchOperationImpl *
AsynchOperation::implementation (void) const
{
  return this->implementation_;
}

AsynchOperation::AsynchOperation (void)
  : implementation_ (0)
{
}

AsynchOperation::~AsynchOperation (void)
{
  delete this->implementation_;
  this->implementation_ = 0;
}

void
AsynchOperation::implementation (AsynchOperationImpl *implementation)
{
  this->implementation_ = implementation;
}

Proactor *
AsynchOperation::get_proactor (Proactor *user_proactor,
                                    Handler &handler) const
{
  if (user_proactor == 0)
    {
      // Grab the singleton proactor if <handler->proactor> is zero
      user_proactor = handler.proactor ();
      if (user_proactor == 0)
        user_proactor = Proactor::instance ();
    }

  return user_proactor;
}

// ************************************************************

AsynchReadStream::AsynchReadStream (void)
  : implementation_ (0)
{
}

AsynchReadStream::~AsynchReadStream (void)
{
}

int
AsynchReadStream::open (Handler &handler,
                              ACE_HANDLE handle,
                              const void *completion_key,
                              Proactor *proactor)
{
  // Get a proactor for/from the user.
  proactor = this->get_proactor (proactor, handler);

  // Delete the old implementation.
  delete this->implementation_;
  this->implementation_ = 0;

  // Now let us get the implementation initialized.
  AsynchReadStreamImpl *implementation = proactor->create_asynch_read_stream ();
  if (implementation == 0)
    return -1;
  
  // Set the implementation class
  this->implementation (implementation);
  
  // Call the <open> method of the base class. 
  return AsynchOperation::open (handler,
                                     handle,
                                     completion_key,
                                     proactor);
}
  
int
AsynchReadStream::read (MessageBlock &message_block,
                              u_long bytes_to_read,
                              const void *act,
                              int priority,
                              int signal_number)
{
  return this->implementation ()->read (message_block,
                                        bytes_to_read,
                                        act,
                                        priority,
                                        signal_number);
}

AsynchReadStreamImpl *
AsynchReadStream::implementation (void) const
{
  return implementation_;
}

void
AsynchReadStream::implementation (AsynchReadStreamImpl *implementation)
{
  this->implementation_ = implementation;

  // Init the base class' implementation also.
  AsynchOperation::implementation (this->implementation_);
}

// ************************************************************

u_long
AsynchReadStream::Result::bytes_to_read (void) const
{
  return this->implementation ()->bytes_to_read ();
}

MessageBlock &
AsynchReadStream::Result::message_block (void) const
{
  return this->implementation ()->message_block ();
}

ACE_HANDLE
AsynchReadStream::Result::handle (void) const
{
  return this->implementation ()->handle ();
}

AsynchReadStream::Result::Result (AsynchReadStreamResultImpl *implementation)
  : AsynchResult (implementation), 
    implementation_ (implementation)
{
}

AsynchReadStream::Result::~Result (void)
{
  // Proactor will delete the implementation after <complete> is
  // finished.
}

AsynchReadStreamResultImpl *
AsynchReadStream::Result::implementation (void) const
{
  return this->implementation_;
}

// ***************************************************

AsynchWriteStream::AsynchWriteStream (void)
  : implementation_ (0)
{
}

AsynchWriteStream::~AsynchWriteStream (void)
{
}

int
AsynchWriteStream::open (Handler &handler,
                               ACE_HANDLE handle,
                               const void *completion_key,
                               Proactor *proactor)
{
  // Get a proactor for/from the user.
  proactor = this->get_proactor (proactor, handler);

  // Delete the old implementation.
  delete this->implementation_;
  this->implementation_ = 0;

  // Now let us get the implementation initialized.
  AsynchWriteStreamImpl *implementation = proactor->create_asynch_write_stream ();
  if (implementation == 0)
    return -1;
  
  // Set the implementation class
  this->implementation (implementation);
  
  // Call the <open> method of the base class.
  return AsynchOperation::open (handler,
                                     handle,
                                     completion_key,
                                     proactor);
}

int
AsynchWriteStream::write (MessageBlock &message_block,
                                u_long bytes_to_write,
                                const void *act,
                                int priority,
                                int signal_number)
{
  return this->implementation ()->write (message_block,
                                         bytes_to_write,
                                         act,
                                         priority,
                                        signal_number);
}

AsynchWriteStreamImpl *
AsynchWriteStream::implementation (void) const
{
  return this->implementation_;
}

void
AsynchWriteStream::implementation (AsynchWriteStreamImpl *implementation)
{
  this->implementation_ = implementation;

  // Init the base class' implementation also.
  AsynchOperation::implementation (this->implementation_);
}

// ************************************************************

u_long
AsynchWriteStream::Result::bytes_to_write (void) const
{
  return this->implementation ()->bytes_to_write ();
}

MessageBlock &
AsynchWriteStream::Result::message_block (void) const
{
  return this->implementation ()->message_block ();
}

ACE_HANDLE
AsynchWriteStream::Result::handle (void) const
{
  return this->implementation ()->handle ();
}

AsynchWriteStream::Result::Result (AsynchWriteStreamResultImpl *implementation)
  : AsynchResult (implementation),
    implementation_ (implementation)
{
}

AsynchWriteStream::Result::~Result (void)
{
  // Proactor will delte the implementation when the <complete> call
  // finishes.
}

AsynchWriteStreamResultImpl *
AsynchWriteStream::Result::implementation (void) const
{
  return this->implementation_;
}

// ************************************************************

AsynchReadFile::AsynchReadFile (void)
  : implementation_ (0)
{
} 

AsynchReadFile::~AsynchReadFile (void)
{
} 

int
AsynchReadFile::open (Handler &handler,
                            ACE_HANDLE handle,
                            const void *completion_key,
                            Proactor *proactor)
{
  // Get a proactor for/from the user.
  proactor = this->get_proactor (proactor, handler);

  // Delete the old implementation.
  delete this->implementation_;
  this->implementation_ = 0;

  // Now let us get the implementation initialized.
  AsynchReadFileImpl *implementation = proactor->create_asynch_read_file ();
  if (implementation == 0)
    return -1;
  
  // Set the implementation class
  this->implementation (implementation);
  
  // Call the <open> method of the base class.
  return AsynchOperation::open (handler,
                                     handle,
                                     completion_key,
                                     proactor);
}

int
AsynchReadFile::read (MessageBlock &message_block,
                            u_long bytes_to_read,
                            u_long offset,
                            u_long offset_high,
                            const void *act,
                            int priority,
                            int signal_number)
{
  return this->implementation ()->read (message_block,
                                        bytes_to_read,
                                        offset,
                                        offset_high,
                                        act,
                                        priority,
                                        signal_number);
}

AsynchReadFileImpl *
AsynchReadFile::implementation (void) const
{
  return this->implementation_;
}

void
AsynchReadFile::implementation (AsynchReadFileImpl *implementation)
{
  this->implementation_ = implementation;

  // Set also the base class' implementation.
  AsynchReadStream::implementation (implementation);
}

// ************************************************************

AsynchReadFile::Result::Result (AsynchReadFileResultImpl *implementation)
  : AsynchReadStream::Result (implementation),
    implementation_ (implementation)
{
}
  
AsynchReadFile::Result::~Result (void)
{
  // Proactor will delete the implementation when <complete> call
  // completes.
}

AsynchReadFileResultImpl *
AsynchReadFile::Result::implementation (void) const
{
  return this->implementation_;
}

// ************************************************************

AsynchWriteFile::AsynchWriteFile (void)
  : implementation_ (0)
{
}

AsynchWriteFile::~AsynchWriteFile (void)
{
}

int
AsynchWriteFile::open (Handler &handler,
                             ACE_HANDLE handle,
                             const void *completion_key,
                             Proactor *proactor)
{
  // Get a proactor for/from the user.
  proactor = this->get_proactor (proactor, handler);

  // Delete the old implementation.
  delete this->implementation_;
  this->implementation_ = 0;

  // Now let us get the implementation initialized.
  AsynchWriteFileImpl *implementation = proactor->create_asynch_write_file ();
  if (implementation == 0)
    return -1;
  
  // Set the implementation class
  this->implementation (implementation);
  
  // Call the <open> method of the base class.
  return AsynchOperation::open (handler,
                                     handle,
                                     completion_key,
                                     proactor);
}

int
AsynchWriteFile::write (MessageBlock &message_block,
                              u_long bytes_to_write,
                              u_long offset,
                              u_long offset_high,
                              const void *act,
                              int priority,
                              int signal_number)
{
  return this->implementation ()->write (message_block,
                                         bytes_to_write,
                                         offset,
                                         offset_high,
                                         act,
                                         priority,
                                         signal_number);
}

AsynchWriteFileImpl *
AsynchWriteFile::implementation (void) const
{
  return this->implementation_;
}

void
AsynchWriteFile::implementation (AsynchWriteFileImpl *implementation)
{
  this->implementation_ = implementation;
  
  // Init the base class' implementation also.
  AsynchWriteStream::implementation (implementation);
}

// ************************************************************

AsynchWriteFile::Result::Result (AsynchWriteFileResultImpl *implementation)
  : AsynchWriteStream::Result (implementation),
    implementation_ (implementation)
{
}

AsynchWriteFile::Result::~Result (void)
{
  // Proactor will delete the implementation when the <complete> call
  // completes. 
}

AsynchWriteFileResultImpl *
AsynchWriteFile::Result::implementation (void) const
{
  return this->implementation_;
}

// *********************************************************************

Handler::Handler (void)
  : proactor_ (0), handle_ (ACE_INVALID_HANDLE)
{
}

Handler::Handler (Proactor *d)
  : proactor_ (d), handle_ (ACE_INVALID_HANDLE)
{
}

Handler::~Handler (void)
{
}

void
Handler::handle_read_stream (const AsynchReadStream::Result & /* result */)
{
}

void
Handler::handle_write_stream (const AsynchWriteStream::Result & /* result */)
{
}

void
Handler::handle_read_file (const AsynchReadFile::Result & /* result */)
{
}

void
Handler::handle_write_file (const AsynchWriteFile::Result & /* result */)
{
}

void
Handler::handle_time_out (const TimeValue & /* tv */,
                              const void *           /* act */)
{
}

void
Handler::handle_wakeup (void)
{
}

Proactor *
Handler::proactor (void)
{
  return this->proactor_;
}

void
Handler::proactor (Proactor *p)
{
  this->proactor_ = p;
}

ACE_HANDLE
Handler::handle (void) const
{
  return this->handle_;
}

void
Handler::handle (ACE_HANDLE h)
{
  this->handle_ = h;
}


#endif /* ACE_WIN32 || ACE_HAS_AIO_CALLS */
