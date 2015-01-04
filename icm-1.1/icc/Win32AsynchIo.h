/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    WIN32_Asynch_IO.h
 *
 *  WIN32_Asynch_IO.h,v 4.16 2001/08/08 14:24:27 schmidt Exp
 *
 *
 *  These classes only works on Win32 platforms.
 *
 *  The implementation of <ACE_Asynch_Transmit_File> and
 *  <ACE_Asynch_Accept> are only supported if ACE_HAS_WINSOCK2 is
 *  defined or you are on WinNT 4.0 or higher.
 *
 *
 *  @author Irfan Pyarali <irfan@cs.wustl.edu>
 *  @author Tim Harrison <harrison@cs.wustl.edu>
 *  @author Alexander Babu Arulanthu <alex@cs.wustl.edu>
 *  @author Roger Tragin <r.tragin@computer.org>
 */
//=============================================================================

#ifndef ACE_WIN32_ASYNCH_IO_H
#define ACE_WIN32_ASYNCH_IO_H


#include "os/OS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE))

#include "icc/AsynchIoImpl.h"
#include "icc/Addr.h"

// Forward declaration
class Win32Proactor;

/**
 * @class Win32AsynchResult
 *
 * @brief An abstract class which adds information to the OVERLAPPED
 *     structure to make it more useful.
 *
 *     An abstract base class from which you can obtain some basic
 *     information like the number of bytes transferred, the ACT
 *     associated with the asynchronous operation, indication of
 *     success or failure, etc.  Subclasses may want to store more
 *     information that is particular to the asynchronous operation
 *     it represents.
 */
class ACE_Export Win32AsynchResult : public virtual AsynchResultImpl,
                                           public OVERLAPPED
{
  /// Factory class has special permissions.
  friend class ACE_WIN32_Asynch_Accept;

  /// Proactor class has special permission.
  friend class Win32Proactor;

public:
  /// Number of bytes transferred by the operation.
  u_long bytes_transferred (void) const;

  /// ACT associated with the operation.
  const void *act (void) const;

  /// Did the operation succeed?
  int success (void) const;

  /**
   * This returns the ACT associated with the handle when it was
   * registered with the I/O completion port.  This ACT is not the
   * same as the ACT associated with the asynchronous operation.
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// Event associated with the OVERLAPPED structure.
  ACE_HANDLE event (void) const;

  /// This really make sense only when doing file I/O.
  u_long offset (void) const;

  /// Offset_high associated with the OVERLAPPED structure.
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation. Currently, this is
  /// not supported on Win32.
  int priority (void) const;

  /// Returns 0.
  int signal_number (void) const;

  /// Post <this> to the Proactor's completion port.
  int post_completion (ProactorImpl *proactor);

  /// Destructor.
  virtual ~Win32AsynchResult (void);

  /// Simulate error value to use in the post_completion ()
  void set_error (u_long errcode);

  /// Simulate value to use in the post_completion ()
  void set_bytes_transferred (u_long nbytes);

protected:
  /// Constructor.
  Win32AsynchResult (Handler &handler,
                           const void* act,
                           ACE_HANDLE event,
                           u_long offset,
                           u_long offset_high,
                           int priority,
                           int signal_number = 0);

  /// Handler that will be called back.
  Handler &handler_;

  /// ACT for this operation.
  const void *act_;

  /// Bytes transferred by this operation.
  u_long bytes_transferred_;

  /// Success indicator.
  int success_;

  /// ACT associated with handle.
  const void *completion_key_;

  /// Error if operation failed.
  u_long error_;
};

/**
 * @class Win32AsynchOperation
 *
 * @brief This class abstracts out the common things needed for
 * implementing Asynch_Operation for WIN32 platform.
 *
 */
class ACE_Export Win32AsynchOperation : public virtual AsynchOperationImpl
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
   * This cancels all pending accepts operations that were issued by
   * the calling thread.  The function does not cancel asynchronous
   * operations issued by other threads.
   */
  int cancel (void);

  // = Access methods.

  /// Return the underlying proactor.
  Proactor* proactor (void) const;

protected:
  /// Constructor.
  Win32AsynchOperation (Win32Proactor *win32_proactor);

  /// Destructor.
  virtual ~Win32AsynchOperation (void);

  /// WIn32 Proactor.
  Win32Proactor *win32_proactor_;

  /// Proactor that this asynch IO is registered with.
  Proactor *proactor_;

  /// Handler that will receive the callback.
  Handler *handler_;

  /// I/O handle used for reading.
  ACE_HANDLE handle_;
};

/**
 * @class Win32AsynchReadStreamResult
 *
 * @brief This is class provides concrete implementation for
 * AsynchReadStream::Result class.
 */
class ACE_Export Win32AsynchReadStreamResult : public virtual AsynchReadStreamResultImpl,
                                                       public Win32AsynchResult
{
  /// Factory class will have special permissions.
  friend class Win32AsynchReadStream;

  /// Proactor class has special permission.
  friend class Win32Proactor;

public:
  /// The number of bytes which were requested at the start of the
  /// asynchronous read.
  u_long bytes_to_read (void) const;

  /// Message block which contains the read data.
  MessageBlock &message_block (void) const;

  /// I/O handle used for reading.
  ACE_HANDLE handle (void) const;

  // Base class operations. These operations are here to kill
  // dominance warnings. These methods call the base class methods.

  /// Number of bytes transferred by the operation.
  u_long bytes_transferred (void) const;

  /// ACT associated with the operation.
  const void *act (void) const;

  /// Did the operation succeed?
  int success (void) const;

  /**
   * This returns the ACT associated with the handle when it was
   * registered with the I/O completion port.  This ACT is not the
   * same as the ACT associated with the asynchronous operation.
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// Event associated with the OVERLAPPED structure.
  ACE_HANDLE event (void) const;

  /// This really make sense only when doing file I/O.
  u_long offset (void) const;

  /// Offset_high associated with the OVERLAPPED structure.
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation. Currently, this is
  /// not supported on Win32.
  int priority (void) const;

  /// No-op. Returns 0.
  int signal_number (void) const;

  /// Post <this> to the Proactor's completion port.
  int post_completion (ProactorImpl *proactor);

protected:
  Win32AsynchReadStreamResult (Handler &handler,
                                       ACE_HANDLE handle,
                                       MessageBlock &message_block,
                                       u_long bytes_to_read,
                                       const void* act,
                                       ACE_HANDLE event,
                                       int priority,
                                       int signal_number = 0);
  // Constructor is protected since creation is limited to
  // AsynchReadStream factory.

  /// Proactor will call this method when the read completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~Win32AsynchReadStreamResult (void);

  /// Bytes requested when the asynchronous read was initiated.
  u_long bytes_to_read_;

  /// Message block for reading the data into.
  MessageBlock &message_block_;

  /// I/O handle used for reading.
  ACE_HANDLE handle_;
};

/**
 * @class Win32AsynchReadStream
 *
 * @brief This class is a factory for starting off asynchronous reads
 *     on a stream.
 *
 *     Once <open> is called, multiple asynchronous <read>s can
 *     started using this class.  An AsynchReadStream::Result
 *     will be passed back to the <handler> when the asynchronous
 *     reads completes through the <Handler::handle_read_stream>
 *     callback.
 */
class ACE_Export Win32AsynchReadStream : public virtual AsynchReadStreamImpl,
                                                public Win32AsynchOperation
{

public:
  /// Constructor.
  Win32AsynchReadStream (Win32Proactor *win32_proactor);

  /// This starts off an asynchronous read.  Upto <bytes_to_read> will
  /// be read and stored in the <message_block>.
  int read (MessageBlock &message_block,
            u_long bytes_to_read,
            const void *act,
            int priority,
            int signal_number = 0);

  /// Destructor.
  virtual ~Win32AsynchReadStream (void);

  // Methods belong to Win32AsynchOperation base class. These
  // methods are defined here to avoid VC++ warnings. They route the
  // call to the Win32AsynchOperation base class.

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
   * This cancels all pending accepts operations that were issued by
   * the calling thread.  The function does not cancel asynchronous
   * operations issued by other threads.
   */
  int cancel (void);

  /// Return the underlying proactor.
  Proactor* proactor (void) const;

protected:
  /// This is the method which does the real work and is there so that
  /// the AsynchReadFile class can use it too.
  int shared_read (Win32AsynchReadStreamResult *result);
};

/**
 * @class Win32AsynchWriteStreamResult
 *
 * @brief This is class provides concrete implementation for
 *    AsynchWriteStream::Result class.
 */
class ACE_Export Win32AsynchWriteStreamResult : public virtual AsynchWriteStreamResultImpl,
                                                        public Win32AsynchResult
{
  /// Factory class willl have special permissions.
  friend class Win32AsynchWriteStream;

  /// Proactor class has special permission.
  friend class Win32Proactor;

public:
  /// The number of bytes which were requested at the start of the
  /// asynchronous write.
  u_long bytes_to_write (void) const;

  /// Message block that contains the data to be written.
  MessageBlock &message_block (void) const;

  /// I/O handle used for writing.
  ACE_HANDLE handle (void) const;

  // = Base class operations. These operations are here to kill some
  //   warnings. These methods call the base class methods.

  /// Number of bytes transferred by the operation.
  u_long bytes_transferred (void) const;

  /// ACT associated with the operation.
  const void *act (void) const;

  /// Did the operation succeed?
  int success (void) const;

  /**
   * This returns the ACT associated with the handle when it was
   * registered with the I/O completion port.  This ACT is not the
   * same as the ACT associated with the asynchronous operation.
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// Event associated with the OVERLAPPED structure.
  ACE_HANDLE event (void) const;

  /// This really make sense only when doing file I/O.
  u_long offset (void) const;

  /// Offset_high associated with the OVERLAPPED structure.
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation. Currently, this is
  /// not supported on Win32.
  int priority (void) const;

  /// No-op. Returns 0.
  int signal_number (void) const;

  /// Post <this> to the Proactor's completion port.
  int post_completion (ProactorImpl *proactor);

protected:
  Win32AsynchWriteStreamResult (Handler &handler,
                                        ACE_HANDLE handle,
                                        MessageBlock &message_block,
                                        u_long bytes_to_write,
                                        const void* act,
                                        ACE_HANDLE event,
                                        int priority,
                                        int signal_number = 0);
  // Constructor is protected since creation is limited to
  // AsynchWriteStream factory.

  /// Proactor will call this method when the write completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~Win32AsynchWriteStreamResult (void);

  /// The number of bytes which were requested at the start of the
  /// asynchronous write.
  u_long bytes_to_write_;

  /// Message block that contains the data to be written.
  MessageBlock &message_block_;

  /// I/O handle used for writing.
  ACE_HANDLE handle_;
};

/**
 * @class Win32AsynchWriteStream
 *
 * @brief This class is a factory for starting off asynchronous writes
 *    on a stream.
 *
 *
 *     Once <open> is called, multiple asynchronous <writes>s can
 *     started using this class.  A AsynchWriteStream::Result
 *     will be passed back to the <handler> when the asynchronous
 *     write completes through the
 *     <Handler::handle_write_stream> callback.
 */
class ACE_Export Win32AsynchWriteStream : public virtual AsynchWriteStreamImpl,
                                                 public Win32AsynchOperation
{
public:
  /// Constructor.
  Win32AsynchWriteStream (Win32Proactor *win32_proactor);

  /// This starts off an asynchronous write.  Upto <bytes_to_write>
  /// will be written from the <message_block>.
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             const void *act,
             int priority,
             int signal_number = 0);

  /// Destructor.
  virtual ~Win32AsynchWriteStream (void);

  // = Methods belonging to <Win32AsynchOperation> base class.

  // These methods are defined here to avoid VC++ warnings. They route
  // the call to the <Win32AsynchOperation> base class.

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
   * This cancels all pending accepts operations that were issued by
   * the calling thread.  The function does not cancel asynchronous
   * operations issued by other threads.
   */
  int cancel (void);

  /// Return the underlying proactor.
  Proactor* proactor (void) const;

protected:
  /// This is the method which does the real work and is there so that
  /// the AsynchWriteFile class can use it too.
  int shared_write (Win32AsynchWriteStreamResult *result);
};

/**
 * @class Win32AsynchReadFileResult
 *
 * @brief This is class provides concrete implementation for
 *     AsynchReadFile::Result class.
 */
class ACE_Export Win32AsynchReadFileResult : public virtual AsynchReadFileResultImpl,
                                                     public Win32AsynchReadStreamResult
{
  /// Factory class will have special permissions.
  friend class Win32AsynchReadFile;

  /// Proactor class has special permission.
  friend class Win32Proactor;

public:
  // = These methods belong to Win32AsynchResult class base
  //   class. These operations are here to kill some warnings. These
  //   methods call the base class methods.

  /// Number of bytes transferred by the operation.
  u_long bytes_transferred (void) const;

  /// ACT associated with the operation.
  const void *act (void) const;

  /// Did the operation succeed?
  int success (void) const;

  /**
   * This returns the ACT associated with the handle when it was
   * registered with the I/O completion port.  This ACT is not the
   * same as the ACT associated with the asynchronous operation.
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// Event associated with the OVERLAPPED structure.
  ACE_HANDLE event (void) const;

  /// This really make sense only when doing file I/O.
  u_long offset (void) const;

  /// Offset_high associated with the OVERLAPPED structure.
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation. Currently, this is
  /// not supported on Win32.
  int priority (void) const;

  /// No-op. Returns 0.
  int signal_number (void) const;

  // The following methods belong to
  // Win32AsynchReadStreamResult. They are here to avoid VC++
  // dominance warnings. These methods route their call to the
  // Win32AsynchReadStreamResult base class.

  /// The number of bytes which were requested at the start of the
  /// asynchronous read.
  u_long bytes_to_read (void) const;

  /// Message block which contains the read data.
  MessageBlock &message_block (void) const;

  /// I/O handle used for reading.
  ACE_HANDLE handle (void) const;

  /// Post <this> to the Proactor's completion port.
  int post_completion (ProactorImpl *proactor);

protected:
  Win32AsynchReadFileResult (Handler &handler,
                                     ACE_HANDLE handle,
                                     MessageBlock &message_block,
                                     u_long bytes_to_read,
                                     const void* act,
                                     u_long offset,
                                     u_long offset_high,
                                     ACE_HANDLE event,
                                     int priority,
                                     int signal_number = 0);
  // Constructor is protected since creation is limited to
  // AsynchReadFile factory.

  /// Proactor will call this method when the read completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~Win32AsynchReadFileResult (void);
};

/**
 * @class Win32AsynchReadFile
 *
 * @brief This class is a factory for starting off asynchronous reads
 *     on a file.
 *
 *     Once <open> is called, multiple asynchronous <read>s can
 *     started using this class.  A AsynchReadFile::Result
 *     will be passed back to the <handler> when the asynchronous
 *     reads completes through the <Handler::handle_read_file>
 *     callback.
 *
 *     This class differs slightly from AsynchReadStream as it
 *     allows the user to specify an offset for the read.
 */
class ACE_Export Win32AsynchReadFile : public virtual AsynchReadFileImpl,
                                              public Win32AsynchReadStream
{

public:
  /// Constructor.
  Win32AsynchReadFile (Win32Proactor *win32_proactor);

  /**
   * This starts off an asynchronous read.  Upto <bytes_to_read> will
   * be read and stored in the <message_block>.  The read will start
   * at <offset> from the beginning of the file.
   */
  int read (MessageBlock &message_block,
            u_long bytes_to_read,
            u_long offset,
            u_long offset_high,
            const void *act,
            int priority,
            int signal_number = 0);

  /// Destructor.
  virtual ~Win32AsynchReadFile (void);

  // = Methods belong to Win32AsynchOperation base class. These
  //   methods are defined here to avoid VC++ warnings. They route the
  //   call to the Win32AsynchOperation base class.

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
   * This cancels all pending accepts operations that were issued by
   * the calling thread.  The function does not cancel asynchronous
   * operations issued by other threads.
   */
  int cancel (void);

  /// Return the underlying proactor.
  Proactor* proactor (void) const;

private:
  /**
   * This method belongs to Win32AsynchReadStream. It is here
   * to avoid the compiler warnings. We forward this call to the
   * Win32AsynchReadStream class.
   */
  int read (MessageBlock &message_block,
            u_long bytes_to_read,
            const void *act,
            int priority,
            int signal_number = 0);
};

/**
 * @class Win32AsynchWriteFileResult
 *
 * @brief  This class provides implementation for
 *      ACE_Asynch_Write_File_Result for WIN32 platforms.
 *
 *     This class has all the information necessary for the
 *     <handler> to uniquiely identify the completion of the
 *     asynchronous write.
 *
 *     This class differs slightly from
 *     AsynchWriteStream::Result as it calls back
 *     <Handler::handle_write_file> on the <handler> instead
 *     of <Handler::handle_write_stream>.  No additional state
 *     is required by this class as AsynchResult can store
 *     the <offset>.
 */
class ACE_Export Win32AsynchWriteFileResult : public virtual AsynchWriteFileResultImpl,
                                                      public Win32AsynchWriteStreamResult
{
  /// Factory class will have special permission.
  friend class Win32AsynchWriteFile;

  /// Proactor class has special permission.
  friend class Win32Proactor;

public:
  // = Base class operations. These operations are here to kill some
  //   warnings. These methods call the base class methods.

  /// Number of bytes transferred by the operation.
  u_long bytes_transferred (void) const;

  /// ACT associated with the operation.
  const void *act (void) const;

  /// Did the operation succeed?
  int success (void) const;

  /**
   * This returns the ACT associated with the handle when it was
   * registered with the I/O completion port.  This ACT is not the
   * same as the ACT associated with the asynchronous operation.
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// Event associated with the OVERLAPPED structure.
  ACE_HANDLE event (void) const;

  /// This really make sense only when doing file I/O.
  u_long offset (void) const;

  /// Offset_high associated with the OVERLAPPED structure.
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation. Currently, this is
  /// not supported on Win32.
  int priority (void) const;

  /// No-op. Returns 0.
  int signal_number (void) const;

  // The following methods belong to
  // Win32AsynchReadStreamResult. They are here to avoid VC++
  // warnings. These methods route their call to the
  // Win32AsynchReadStreamResult base class.

  /// The number of bytes which were requested at the start of the
  /// asynchronous write.
  u_long bytes_to_write (void) const;

  /// Message block that contains the data to be written.
  MessageBlock &message_block (void) const;

  /// I/O handle used for writing.
  ACE_HANDLE handle (void) const;

  /// Post <this> to the Proactor's completion port.
  int post_completion (ProactorImpl *proactor);

protected:
  Win32AsynchWriteFileResult (Handler &handler,
                                      ACE_HANDLE handle,
                                      MessageBlock &message_block,
                                      u_long bytes_to_write,
                                      const void* act,
                                      u_long offset,
                                      u_long offset_high,
                                      ACE_HANDLE event,
                                      int priority,
                                      int signal_number = 0);
  // Constructor is protected since creation is limited to
  // AsynchWriteFile factory.

  /// Proactor will call this method when the write completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~Win32AsynchWriteFileResult (void);
};

/**
 * @class Win32AsynchWriteFile
 *
 * @brief This class is a factory for starting off asynchronous writes
 *     on a file.
 *
 *     Once <open> is called, multiple asynchronous <write>s can be
 *     started using this class.  A AsynchWriteFile::Result
 *     will be passed back to the <handler> when the asynchronous
 *     writes completes through the <Handler::handle_write_file>
 *     callback.
 */
class ACE_Export Win32AsynchWriteFile : public virtual AsynchWriteFileImpl,
                                               public Win32AsynchWriteStream
{
public:
  /// Constructor.
  Win32AsynchWriteFile (Win32Proactor *win32_proactor);

  /**
   * This starts off an asynchronous write.  Upto <bytes_to_write>
   * will be write and stored in the <message_block>.  The write will
   * start at <offset> from the beginning of the file.
   */
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             u_long offset,
             u_long offset_high,
             const void *act,
             int priority,
             int signal_number = 0);

  /// Destrcutor.
  virtual ~Win32AsynchWriteFile (void);

  // = Methods belong to Win32AsynchOperation base class. These
  //   methods are defined here to avoid VC++ warnings. They route the
  //   call to the Win32AsynchOperation base class.

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
   * This cancels all pending accepts operations that were issued by
   * the calling thread.  The function does not cancel asynchronous
   * operations issued by other threads.
   */
  int cancel (void);

  /// Return the underlying proactor.
  Proactor* proactor (void) const;

private:
  /**
   * This method belongs to Win32AsynchWriteStream. It is here
   * to avoid compiler warnings. This method is forwarded to the
   * Win32AsynchWriteStream class.
   */
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             const void *act,
             int priority,
             int signal_number = 0);
};

#endif /* ACE_WIN32 && !ACE_HAS_WINCE */

#endif /* ACE_WIN32_ASYNCH_IO_H */
