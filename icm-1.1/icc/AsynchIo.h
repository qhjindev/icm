
#ifndef ICC_ASYNCH_IO_H
#define ICC_ASYNCH_IO_H

#include "os/OS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS))

# if defined (ACE_WIN32) && ! defined (ACE_HAS_WINCE) \
                         && ! defined (ACE_HAS_PHARLAP)
typedef TRANSMIT_FILE_BUFFERS ACE_TRANSMIT_FILE_BUFFERS;
typedef LPTRANSMIT_FILE_BUFFERS ACE_LPTRANSMIT_FILE_BUFFERS;
typedef PTRANSMIT_FILE_BUFFERS ACE_PTRANSMIT_FILE_BUFFERS;

#   define ACE_INFINITE INFINITE
#   define ACE_STATUS_TIMEOUT STATUS_TIMEOUT
#   define ACE_WAIT_FAILED WAIT_FAILED
#   define ACE_WAIT_TIMEOUT WAIT_TIMEOUT
# else /* ACE_WIN32 */
struct ACE_TRANSMIT_FILE_BUFFERS
{
  void *Head;
  size_t HeadLength;
  void *Tail;
  size_t TailLength;
};
typedef ACE_TRANSMIT_FILE_BUFFERS* ACE_PTRANSMIT_FILE_BUFFERS;
typedef ACE_TRANSMIT_FILE_BUFFERS* ACE_LPTRANSMIT_FILE_BUFFERS;

#   if !defined (ACE_INFINITE)
#     define ACE_INFINITE LONG_MAX
#   endif /* ACE_INFINITE */
#   define ACE_STATUS_TIMEOUT LONG_MAX
#   define ACE_WAIT_FAILED LONG_MAX
#   define ACE_WAIT_TIMEOUT LONG_MAX
# endif /* ACE_WIN32 */

// Forward declarations
class Proactor;
class Handler;
class MessageBlock;
//class ACE_INET_Addr;

// Forward declarations
class AsynchResultImpl;

/**
 * @class AsynchResult
 *
 * @brief An interface base class which allows users access to common
 * information related to an asynchronous operation.
 *
 * An interface base class from which you can obtain some basic
 * information like the number of bytes transferred, the ACT
 * associated with the asynchronous operation, indication of
 * success or failure, etc. Subclasses may want to store more
 * information that is particular to the asynchronous operation
 * it represents.
 */
class ACE_Export AsynchResult
{

public:
  /// Number of bytes transferred by the operation.
  u_long bytes_transferred (void) const;

  /// ACT associated with the operation.
  const void *act (void) const;

  /// Did the operation succeed?
  int success (void) const;

  /**
   * This is the ACT associated with the handle on which the
   * Asynch_Operation takes place.
   *
   * On WIN32, this returns the ACT associated with the handle when it
   * was registered with the I/O completion port.
   *
   * @@ This is not implemented for POSIX4 platforms. Returns 0.
   */
  const void *completion_key (void) const;

  /// Error value if the operation fails.
  u_long error (void) const;

  /**
   * On WIN32, this returns the event associated with the OVERLAPPED
   * structure.
   *
   * This returns ACE_INVALID_HANDLE on POSIX4-Unix platforms.
   */
  ACE_HANDLE event (void) const;

  /**
   * This really makes sense only when doing file I/O.
   *
   * On WIN32, these are represented in the OVERLAPPED datastructure.
   *
   * @@ On POSIX4-Unix, offset_high should be supported using
   *    aiocb64.
   */
  u_long offset (void) const;
  u_long offset_high (void) const;

  /**
   * Priority of the operation.
   *
   * On POSIX4-Unix, this is supported. Priority works like <nice> in
   * Unix. Negative values are not allowed. 0 means priority of the
   * operation same as the process priority. 1 means priority of the
   * operation is one less than process. And so forth.
   *
   * On Win32, this is a no-op.
   */
  int priority (void) const;

  /**
   * POSIX4 real-time signal number to be used for the
   * operation. <signal_number> ranges from ACE_SIGRTMIN to ACE_SIGRTMAX. By
   * default, ACE_SIGRTMIN is used to issue <aio_> calls. This is a no-op
   * on non-POSIX4 systems and returns 0.
   */
  int signal_number (void) const;


  /// Destructor.
  virtual ~AsynchResult (void);

protected:
  /// Constructor. This implementation will not be deleted.  The
  /// implementation will be deleted by the Proactor.
  AsynchResult (AsynchResultImpl *implementation);

  /// Get the implementation class.
  AsynchResultImpl *implementation (void) const;

  /// Implementation class.
  AsynchResultImpl *implementation_;
};

// Forward declarations
class AsynchOperationImpl;

/**
 * @class AsynchOperation
 *
 * @brief This is an interface base class for all asynch
 * operations. The resposiblility of this class is to forward
 * all methods to its delegation/implementation class, e.g.,
 * <Win32AsynchOperation> or <ACE_POSIX_Asynch_Operation>.
 *
 * There are some attributes and functionality which is common
 * to all asychronous operations. The delegation classes of this
 * class will factor out this code.
 */
class ACE_Export AsynchOperation
{

public:
  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (Handler &handler,
            ACE_HANDLE handle,
            const void *completion_key,
            Proactor *proactor);

  /**
   * (Attempts to) cancel the asynchronous operation pending against
   * the <handle> registered with this Operation.
   *
   * All completion notifications for the I/O operations will occur
   * normally.
   *
   * = Return Values:
   *
   * -1 : Operation failed. (can get only in POSIX).
   *  0 : All the operations were cancelled.
   *  1 : All the operations were already finished in this
   *      handle. Unable to cancel them.
   *  2 : Atleast one of the requested operations cannot be
   *      cancelled.
   *
   * There is slight difference in the semantics between NT and POSIX
   * platforms which is given below.
   *
   * = Win32 :
   *
   *   cancels all pending accepts operations that were issued by the
   *   calling thread.  The function does not cancel asynchronous
   *   operations issued by other threads.
   *   All I/O operations that are canceled will complete with the
   *   error ERROR_OPERATION_ABORTED.
   *
   * = POSIX:
   *
   *   Attempts to cancel one or more asynchronous I/O requests
   *   currently outstanding against the <handle> registered in this
   *   operation.
   *   For requested operations that are successfully canceled, the
   *   associated  error  status is set to ECANCELED.
   */
  int cancel (void);


  // = Access methods.

  /// Return the underlying proactor.
  Proactor* proactor (void) const;

  /// Destructor.
  virtual ~AsynchOperation (void);

protected:
  /// Constructor.
  AsynchOperation (void);

  /// Return the underlying implementation class.
  AsynchOperationImpl *implementation (void) const;

  /// Set the implementation class.
  void implementation (AsynchOperationImpl *implementation);

  /// Get a proactor for/from the user
  Proactor *get_proactor (Proactor *user_proactor,
                              Handler &handler) const;

  /// Implementation class.
  AsynchOperationImpl *implementation_;
};

// Forward declarations
class AsynchReadStreamResultImpl;
class AsynchReadStreamImpl;

/**
 * @class AsynchReadStream
 *
 * @brief This class is a factory for starting off asynchronous reads
 * on a stream. This class forwards all methods to its
 * implementation class.
 *
 * Once <open> is called, multiple asynchronous <read>s can
 * started using this class.  An AsynchReadStream::Result
 * will be passed back to the <handler> when the asynchronous
 * reads completes through the <Handler::handle_read_stream>
 * callback.
 */
class ACE_Export AsynchReadStream : public AsynchOperation
{

public:
  /// A do nothing constructor.
  AsynchReadStream (void);

  /// Destructor
  virtual ~AsynchReadStream (void);

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call. If (<handle> == ACE_INVALID_HANDLE),
   * <Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (Handler &handler,
            ACE_HANDLE handle = ACE_INVALID_HANDLE,
            const void *completion_key = 0,
            Proactor *proactor = 0);

  /**
   * This starts off an asynchronous read.  Upto <bytes_to_read> will
   * be read and stored in the <message_block>. <message_block>'s
   * <wr_ptr> will be updated to reflect the added bytes if the read
   * operation is successful completed. Priority of the
   * operation is specified by <priority>. On POSIX4-Unix, this is
   * supported. Works like <nice> in Unix. Negative values are not
   * allowed. 0 means priority of the operation same as the process
   * priority. 1 means priority of the operation is one less than
   * process. And so forth. On Win32, <priority> is a no-op.
   * <signal_number> is the POSIX4 real-time signal number to be used
   * for the operation. <signal_number> ranges from ACE_SIGRTMIN to
   * ACE_SIGRTMAX. This argument is a no-op on non-POSIX4 systems.
   */
  int read (MessageBlock &message_block,
            u_long num_bytes_to_read,
            const void *act = 0,
            int priority = 0,
            int signal_number = ACE_SIGRTMIN);

  /// Return the underlying implementation class.
  AsynchReadStreamImpl *implementation (void) const;

protected:
  /// Set the implementation class.
  void implementation (AsynchReadStreamImpl *implementation);

  /// Implementation class that all methods will be forwarded to.
  AsynchReadStreamImpl *implementation_;

public:
/**
 * @class Result
 *
 * @brief This is the class which will be passed back to the
 * <handler> when the asynchronous read completes. This class
 * forwards all the methods to the implementation classes.
 *
 * This class has all the information necessary for the
 * <handler> to uniquiely identify the completion of the
 * asynchronous read.
 */
  class ACE_Export Result : public AsynchResult
  {

    /// The concrete implementation result classes only construct this
    /// class.
    friend class ACE_POSIX_Asynch_Read_Stream_Result;
    friend class Win32AsynchReadStreamResult;

  public:
    /// The number of bytes which were requested at the start of the
    /// asynchronous read.
    u_long bytes_to_read (void) const;

    /// Message block which contains the read data.
    MessageBlock &message_block (void) const;

    /// I/O handle used for reading.
    ACE_HANDLE handle (void) const;

    /// Get the implementation class.
    AsynchReadStreamResultImpl *implementation (void) const;

  protected:
    /// Constructor.
    Result (AsynchReadStreamResultImpl *implementation);

    /// Destructor.
    virtual ~Result (void);

    /// The implementation class.
    AsynchReadStreamResultImpl *implementation_;
  };
};

// Forward declarations
class AsynchWriteStreamImpl;
class AsynchWriteStreamResultImpl;

/**
 * @class AsynchWriteStream
 *
 * @brief This class is a factory for starting off asynchronous writes
 * on a stream. This class forwards all methods to its
 * implementation class.
 *
 * Once <open> is called, multiple asynchronous <writes>s can
 * started using this class.  An AsynchWriteStream::Result
 * will be passed back to the <handler> when the asynchronous
 * write completes through the
 * <Handler::handle_write_stream> callback.
 */
class ACE_Export AsynchWriteStream : public AsynchOperation
{

public:
  /// A do nothing constructor.
  AsynchWriteStream (void);

  /// Destructor.
  virtual ~AsynchWriteStream (void);

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call. If (<handle> == ACE_INVALID_HANDLE),
   * <Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (Handler &handler,
            ACE_HANDLE handle = ACE_INVALID_HANDLE,
            const void *completion_key = 0,
            Proactor *proactor = 0);

  /**
   * This starts off an asynchronous write.  Upto <bytes_to_write>
   * will be written from the <message_block>. Upon successful completion
   * of the write operation, <message_block>'s <rd_ptr> is updated to
   * reflect the data that was written. Priority of the
   * operation is specified by <priority>. On POSIX4-Unix, this is
   * supported. Works like <nice> in Unix. Negative values are not
   * allowed. 0 means priority of the operation same as the process
   * priority. 1 means priority of the operation is one less than
   * process. And so forth. On Win32, this argument is a no-op.
   * <signal_number> is the POSIX4 real-time signal number to be used
   * for the operation. <signal_number> ranges from ACE_SIGRTMIN to
   * ACE_SIGRTMAX. This argument is a no-op on non-POSIX4 systems.
   */
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             const void *act = 0,
             int priority = 0,
             int signal_number = ACE_SIGRTMIN);

  /// Return the underlying implementation class.
  AsynchWriteStreamImpl *implementation (void) const;

protected:
  /// Set the implementation class.
  void implementation (AsynchWriteStreamImpl *implementation);

  /// Implementation class that all methods will be forwarded to.
  AsynchWriteStreamImpl *implementation_;

public:
/**
 * @class
 *
 * @brief This is that class which will be passed back to the
 * <handler> when the asynchronous write completes. This class
 * forwards all the methods to the implementation class.
 *
 * This class has all the information necessary for the
 * <handler> to uniquiely identify the completion of the
 * asynchronous write.
 */
  class ACE_Export Result : public AsynchResult
  {

    /// The concrete implementation result classes only construct this
    /// class.
    friend class ACE_POSIX_Asynch_Write_Stream_Result;
    friend class Win32AsynchWriteStreamResult;

  public:
    /// The number of bytes which were requested at the start of the
    /// asynchronous write.
    u_long bytes_to_write (void) const;

    /// Message block that contains the data to be written.
    MessageBlock &message_block (void) const;

    /// I/O handle used for writing.
    ACE_HANDLE handle (void) const;

    /// Get the implementation class.
    AsynchWriteStreamResultImpl *implementation (void) const;

  protected:
    /// Constrcutor.
    Result (AsynchWriteStreamResultImpl *implementation);

    /// Destructor.
    virtual ~Result (void);

    /// Implementation class.
    AsynchWriteStreamResultImpl *implementation_;
  };
};

// Forward declarations
class AsynchReadFileImpl;
class AsynchReadFileResultImpl;

/**
 * @class AsynchReadFile
 *
 * @brief This class is a factory for starting off asynchronous reads
 * on a file. This class forwards all methods to its
 * implementation class.
 *
 * Once <open> is called, multiple asynchronous <read>s can
 * started using this class. An AsynchReadFile::Result
 * will be passed back to the <handler> when the asynchronous
 * reads completes through the <Handler::handle_read_file>
 * callback.
 * This class differs slightly from AsynchReadStream as it
 * allows the user to specify an offset for the read.
 */
class ACE_Export AsynchReadFile : public AsynchReadStream
{

public:
  /// A do nothing constructor.
  AsynchReadFile (void);

  /// Destructor.
  virtual ~AsynchReadFile (void);

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call. If (<handle> == ACE_INVALID_HANDLE),
   * <Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (Handler &handler,
            ACE_HANDLE handle = ACE_INVALID_HANDLE,
            const void *completion_key = 0,
            Proactor *proactor = 0);

  /**
   * This starts off an asynchronous read.  Upto <bytes_to_read> will
   * be read and stored in the <message_block>.  The read will start
   * at <offset> from the beginning of the file. Priority of the
   * operation is specified by <priority>. On POSIX4-Unix, this is
   * supported. Works like <nice> in Unix. Negative values are not
   * allowed. 0 means priority of the operation same as the process
   * priority. 1 means priority of the operation is one less than
   * process. And so forth. On Win32, this argument is a no-op.
   * <signal_number> is the POSIX4 real-time signal number to be used
   * for the operation. <signal_number> ranges from ACE_SIGRTMIN to
   * ACE_SIGRTMAX. This argument is a no-op on non-POSIX4 systems.
   */
  int read (MessageBlock &message_block,
            u_long bytes_to_read,
            u_long offset = 0,
            u_long offset_high = 0,
            const void *act = 0,
            int priority = 0,
            int signal_number = ACE_SIGRTMIN);

  /// Return the underlying implementation class.
  AsynchReadFileImpl *implementation (void) const;

protected:
  /// Set the implementation class.
  void implementation (AsynchReadFileImpl *implementation);

  /// Delegation/implementation class that all methods will be
  /// forwarded to.
  AsynchReadFileImpl *implementation_;

public:
/**
 * @class Result
 *
 * @brief This is that class which will be passed back to the
 * <handler> when the asynchronous read completes. This class
 * forwards all the methods to the implementation class.
 *
 * This class has all the information necessary for the
 * <handler> to uniquiely identify the completion of the
 * asynchronous read.
 * This class differs slightly from
 * AsynchReadStream::Result as it calls back
 * <Handler::handle_read_file> on the <handler> instead of
 * <Handler::handle_read_stream>.  No additional state is
 * required by this class as AsynchResult can store the
 * <offset>.
 */
  class ACE_Export Result : public AsynchReadStream::Result
  {

    /// The concrete implementation result classes only construct this
    /// class.
    friend class ACE_POSIX_Asynch_Read_File_Result;
    friend class Win32AsynchReadFileResult;

  public:
    /// Get the implementation class.
    AsynchReadFileResultImpl *implementation (void) const;

  protected:
    /// Constructor. This implementation will not be deleted.
    Result (AsynchReadFileResultImpl *implementation);

    /// Destructor.
    virtual ~Result (void);

    /// The implementation class.
    AsynchReadFileResultImpl *implementation_;

  private:
    /// Here just to provide an dummpy implementation, since the
    /// one auto generated by MSVC is flagged as infinitely recursive
    void operator= (Result &) {}
  };
};

// Forward declarations
class AsynchWriteFileImpl;
class AsynchWriteFileResultImpl;

/**
 * @class AsynchWriteFile
 *
 * @brief This class is a factory for starting off asynchronous writes
 * on a file. This class forwards all methods to its
 * implementation class.
 *
 * Once <open> is called, multiple asynchronous <write>s can be
 * started using this class.  A AsynchWriteFile::Result
 * will be passed back to the <handler> when the asynchronous
 * writes completes through the <Handler::handle_write_file>
 * callback.
 * This class differs slightly from AsynchWriteStream as
 * it allows the user to specify an offset for the write.
 */
class ACE_Export AsynchWriteFile : public AsynchWriteStream
{

public:
  /// A do nothing constructor.
  AsynchWriteFile (void);

  /// Destructor.
  virtual ~AsynchWriteFile (void);

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call. If (<handle> == ACE_INVALID_HANDLE),
   * <Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (Handler &handler,
            ACE_HANDLE handle = ACE_INVALID_HANDLE,
            const void *completion_key = 0,
            Proactor *proactor = 0);

  /**
   * This starts off an asynchronous write.  Upto <bytes_to_write>
   * will be written from the <message_block>, starting at the
   * block's <rd_ptr>.  The write will go to the file, starting
   * <offset> bytes from the beginning of the file. Priority of the
   * operation is specified by <priority>. On POSIX4-Unix, this is
   * supported. Works like <nice> in Unix. Negative values are not
   * allowed. 0 means priority of the operation same as the process
   * priority. 1 means priority of the operation is one less than
   * process. And so forth. On Win32, this is a no-op.
   * <signal_number> is the POSIX4 real-time signal number to be used
   * for the operation. <signal_number> ranges from ACE_SIGRTMIN to
   * ACE_SIGRTMAX. This argument is a no-op on non-POSIX4 systems.
   */
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             u_long offset = 0,
             u_long offset_high = 0,
             const void *act = 0,
             int priority = 0,
             int signal_number = ACE_SIGRTMIN);

  /// Return the underlying implementation class.
  AsynchWriteFileImpl *implementation (void) const;

protected:
  /// Set the implementation.
  void implementation (AsynchWriteFileImpl *implementation);

  /// Implementation object.
  AsynchWriteFileImpl *implementation_;

public:
/**
 * @class Result
 *
 * @brief This is that class which will be passed back to the
 * <handler> when the asynchronous write completes. This class
 * forwards all the methods to the implementation class.
 *
 * This class has all the information necessary for the
 * <handler> to uniquiely identify the completion of the
 * asynchronous write.
 * This class differs slightly from
 * AsynchWriteStream::Result as it calls back
 * <Handler::handle_write_file> on the <handler> instead
 * of <Handler::handle_write_stream>.  No additional state
 * is required by this class as AsynchResult can store
 * the <offset>.
 */
  class ACE_Export Result : public AsynchWriteStream::Result
  {

    /// The concrete implementation result classes only construct this
    /// class.
    friend class ACE_POSIX_Asynch_Write_File_Result;
    friend class Win32AsynchWriteFileResult;

  public:
    ///  Get the implementation class.
    AsynchWriteFileResultImpl *implementation (void) const;

  protected:
    /// Constructor. This implementation will not be deleted.
    Result (AsynchWriteFileResultImpl *implementation);

    /// Destructor.
    virtual ~Result (void);

    /// The implementation class.
    AsynchWriteFileResultImpl *implementation_;

  private:
    /// Here just to provide an dummpy implementation, since the
    /// one auto generated by MSVC is flagged as infinitely recursive
    void operator= (Result &) {};
  };
};


/**
 * @class Handler
 *
 * @brief This base class defines the interface for receiving the
 * results of asynchronous operations.
 *
 * Subclasses of this class will fill in appropriate methods.
 */
class ACE_Export Handler
{
public:
  /// A do nothing constructor.
  Handler (void);

  /// A do nothing constructor which allows proactor to be set to <p>.
  Handler (Proactor *p);

  /// Virtual destruction.
  virtual ~Handler (void);

  /// This method will be called when an asynchronous read completes on
  /// a stream.
  virtual void handle_read_stream (const AsynchReadStream::Result &result);

  /// This method will be called when an asynchronous write completes
  /// on a stream.
  virtual void handle_write_stream (const AsynchWriteStream::Result &result);

  /// This method will be called when an asynchronous read completes on
  /// a file.
  virtual void handle_read_file (const AsynchReadFile::Result &result);

  /// This method will be called when an asynchronous write completes
  /// on a file.
  virtual void handle_write_file (const AsynchWriteFile::Result &result);

  /// Called when timer expires.  <tv> was the requested time value and
  /// <act> is the ACT passed when scheduling the timer.
  virtual void handle_time_out (const TimeValue &tv,
                                const void *act = 0);

  /**
   * This is method works with the <run_event_loop> of the
   * Proactor. A special <Wake_Up_Completion> is used to wake up
   * all the threads that are blocking for completions.
   */
  virtual void handle_wakeup (void);

  /// Get the proactor associated with this handler.
  Proactor *proactor (void);

  /// Set the proactor.
  void proactor (Proactor *p);

  /**
   * Get the I/O handle used by this <handler>. This method will be
   * called by the ACE_Asynch_* classes when an ACE_INVALID_HANDLE is
   * passed to <open>.
   */
  virtual ACE_HANDLE handle (void) const;

  /// Set the ACE_HANDLE value for this Handler.
  virtual void handle (ACE_HANDLE);

protected:
  /// The proactor associated with this handler.
  Proactor *proactor_;

  /// The ACE_HANDLE in use with this handler.
  ACE_HANDLE    handle_;
};


#endif /* ACE_WIN32 || ACE_HAS_AIO_CALLS*/

#endif /* ASYNCH_IO_H */
