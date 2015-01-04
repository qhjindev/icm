
#ifndef ACE_ASYNCH_IO_IMPL_H
#define ACE_ASYNCH_IO_IMPL_H


#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS))
// This only works on Win32 platforms and on Unix platforms supporting
// aio calls.

#include "icc/AsynchIo.h"

// Forward declaration.
class ProactorImpl;

/**
 * @class AsynchResultImpl
 *
 * @brief Abstract base class for the all the classes that provide
 * concrete implementations for AsynchResult.
 *
 */
class ACE_Export AsynchResultImpl
{
public:
  virtual ~AsynchResultImpl (void);

  /// Number of bytes transferred by the operation.
  virtual u_long bytes_transferred (void) const = 0;

  /// ACT associated with the operation.
  virtual const void *act (void) const = 0;

  /// Did the operation succeed?
  virtual int success (void) const = 0;

  /// This ACT is not the same as the ACT associated with the
  /// asynchronous operation.
  virtual const void *completion_key (void) const = 0;

  /// Error value if the operation fail.
  virtual u_long error (void) const = 0;

  /// Event associated with the OVERLAPPED structure.
  virtual ACE_HANDLE event (void) const = 0;

  /// This really make sense only when doing file I/O.
  virtual u_long offset (void) const = 0;
  virtual u_long offset_high (void) const = 0;

  /// Priority of the operation.
  virtual int priority (void) const = 0;

  /**
   * POSIX4 real-time signal number to be used for the
   * operation. <signal_number> ranges from SIGRTMIN to SIGRTMAX. By
   * default, SIGRTMIN is used to issue <aio_> calls. This is a no-op
   * on non-POSIX4 systems and returns 0.
   */
  virtual int signal_number (void) const = 0;

  // protected:
  //
  // These two should really be protected.  But sometimes it
  // simplifies code to be able to "fake" a result.  Use carefully.
  /// This is called when the asynchronous operation completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error = 0) = 0;

  /// Post <this> to the Proactor's completion port.
  virtual int post_completion (ProactorImpl *proactor) = 0;

protected:
  /// Do-nothing constructor.
  AsynchResultImpl (void);
};

/**
 * @class AsynchOperationImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchOperation.
 */
class ACE_Export AsynchOperationImpl
{
public:
  virtual ~AsynchOperationImpl (void);

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  virtual int open (Handler &handler,
                    ACE_HANDLE handle,
                    const void *completion_key,
                    Proactor *proactor) = 0;

  /**
   * This cancels all pending accepts operations that were issued by
   * the calling thread.  The function does not cancel asynchronous
   * operations issued by other threads.
   */
  virtual int cancel (void) = 0;

  // = Access methods.

  /// Return the underlying proactor.
  virtual Proactor* proactor (void) const = 0;

protected:
  /// Do-nothing constructor.
  AsynchOperationImpl (void);
};

/**
 * @class AsynchReadStreamImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchReadStream
 *
 */
class ACE_Export AsynchReadStreamImpl : public virtual AsynchOperationImpl
{
public:
  virtual ~AsynchReadStreamImpl (void);

  /// This starts off an asynchronous read. Upto <bytes_to_read> will
  /// be read and stored in the <message_block>.
  virtual int read (MessageBlock &message_block,
                    u_long bytes_to_read,
                    const void *act,
                    int priority,
                    int signal_number) = 0;

protected:
  /// Do-nothing constructor.
  AsynchReadStreamImpl (void);
};

/**
 * @class AsynchReadStreamResultImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchReadStream::Result class.
 *
 */
class ACE_Export AsynchReadStreamResultImpl : public virtual AsynchResultImpl
{
public:
  virtual ~AsynchReadStreamResultImpl (void);

  /// The number of bytes which were requested at the start of the
  /// asynchronous read.
  virtual u_long bytes_to_read (void) const = 0;

  /// Message block which contains the read data.
  virtual MessageBlock &message_block (void) const = 0;

  /// I/O handle used for reading.
  virtual ACE_HANDLE handle (void) const = 0;

protected:
  /// Do-nothing constructor.
  AsynchReadStreamResultImpl (void);
};

/**
 * @class AsynchWriteStreamImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchWriteStream class.
 *
 */
class ACE_Export AsynchWriteStreamImpl : public virtual AsynchOperationImpl
{
public:
  virtual ~AsynchWriteStreamImpl (void);

  /// This starts off an asynchronous write.  Upto <bytes_to_write>
  /// will be written from the <message_block>.
  virtual int write (MessageBlock &message_block,
                     u_long bytes_to_write,
                     const void *act,
                     int priority,
                     int signal_number) = 0;

protected:
  /// Do-nothing constructor.
  AsynchWriteStreamImpl (void);
};

/**
 * @class AsynchWriteStreamResultImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchWriteStream::Result.
 *
 */
class ACE_Export AsynchWriteStreamResultImpl : public virtual AsynchResultImpl
{
public:
  virtual ~AsynchWriteStreamResultImpl (void);

  /// The number of bytes which were requested at the start of the
  /// asynchronous write.
  virtual u_long bytes_to_write (void) const = 0;

  /// Message block that contains the data to be written.
  virtual MessageBlock &message_block (void) const = 0;

  /// I/O handle used for writing.
  virtual ACE_HANDLE handle (void) const = 0;

protected:
  /// Do-nothing constructor.
  AsynchWriteStreamResultImpl (void);
};

/**
 * @class AsynchReadFileImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchReadFile::Result.
 *
 */
class ACE_Export AsynchReadFileImpl : public virtual AsynchReadStreamImpl
{
public:
  virtual ~AsynchReadFileImpl (void);

  /**
   * This starts off an asynchronous read.  Upto <bytes_to_read> will
   * be read and stored in the <message_block>.  The read will start
   * at <offset> from the beginning of the file.
   */
  virtual int read (MessageBlock &message_block,
                    u_long bytes_to_read,
                    u_long offset,
                    u_long offset_high,
                    const void *act,
                    int priority,
                    int signal_number) = 0;

  // We don;t need to redefine the following function again because it
  // has already been defined in AsynchReadStreamImpl.  But we
  // still need it here to supress a overwriting pure virtual function
  // warning in KAI compiler.
  /// This starts off an asynchronous read. Upto <bytes_to_read> will
  /// be read and stored in the <message_block>.
  virtual int read (MessageBlock &message_block,
                    u_long bytes_to_read,
                    const void *act,
                    int priority,
                    int signal_number) = 0;

protected:
  /// Do-nothing constructor.
  AsynchReadFileImpl (void);
};

/**
 * @class AsynchReadFileResultImpl
 *
 * @brief This is the abstract base class for all the concrete
 * implementation classes for AsynchReadFile::Result.
 *
 */
class ACE_Export AsynchReadFileResultImpl : public virtual AsynchReadStreamResultImpl
{
public:
  /// Destructor.
  virtual ~AsynchReadFileResultImpl (void);

protected:
  /// Do-nothing constructor.
  AsynchReadFileResultImpl (void);
};

/**
 * @class AsynchWriteFileImpl
 *
 * @brief Abstract base class for all the concrete implementation
 * classes that provide different implementations for the
 * AsynchWriteFile.
 *
 */
class ACE_Export AsynchWriteFileImpl : public virtual AsynchWriteStreamImpl
{
public:
  virtual ~AsynchWriteFileImpl (void);

  /**
   * This starts off an asynchronous write.  Upto <bytes_to_write>
   * will be write and stored in the <message_block>.  The write will
   * start at <offset> from the beginning of the file.
   */
  virtual int write (MessageBlock &message_block,
                     u_long bytes_to_write,
                     u_long offset,
                     u_long offset_high,
                     const void *act,
                     int priority,
                    int signal_number) = 0;

  // We don;t need to redefine the following function again because it
  // has already been defined in AsynchWriteStreamImpl.  But we
  // still need it here to supress a overwriting pure virtual function
  // warning in KAI compiler.
  /// This starts off an asynchronous write.  Upto <bytes_to_write>
  /// will be written from the <message_block>.
  virtual int write (MessageBlock &message_block,
                     u_long bytes_to_write,
                     const void *act,
                     int priority,
                     int signal_number) = 0;

protected:
  /// Do-nothing constructor.
  AsynchWriteFileImpl (void);
};

/**
 * @class AsynchWriteFileResultImpl
 *
 * @brief This is the abstract base class for all the concrete
 * implementation classes that provide different implementations
 * for the AsynchWriteFile::Result.
 *
 */
class ACE_Export AsynchWriteFileResultImpl : public virtual AsynchWriteStreamResultImpl
{
public:
  virtual ~AsynchWriteFileResultImpl (void);

protected:
  /// Do-nothing constructor.
  AsynchWriteFileResultImpl (void);
};


#endif /* ACE_HAS_AIO_CALLS  || !ACE_HAS_WINCE && ACE_WIN32 */

#endif /* ACE_ASYNCH_IO_IMPL_H */
