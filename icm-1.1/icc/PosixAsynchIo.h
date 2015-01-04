/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    POSIX_Asynch_IO.h
 *
 *  POSIX_Asynch_IO.h,v 4.26 2001/07/02 01:03:11 schmidt Exp
 *
 *  The implementation classes for POSIX implementation of Asynch
 *  Operations are defined here in this file.
 *
 *  @author Irfan Pyarali <irfan@cs.wustl.edu>
 *  @author Tim Harrison <harrison@cs.wustl.edu>
 *  @author Alexander Babu Arulanthu <alex@cs.wustl.edu>
 *  @author Roger Tragin <r.tragin@computer.org>
 *  @author Alexander Libman <alibman@baltimore.com>
 */
//=============================================================================

#ifndef ACE_POSIX_ASYNCH_IO_H
#define ACE_POSIX_ASYNCH_IO_H

#include "os/OS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

//#if defined (ACE_HAS_AIO_CALLS)

#include "icc/AsynchIoImpl.h"
#include "icc/Reactor.h"

// Forward declarations
//class PosixSigProactor;
class PosixAiocbProactor;
class ProactorImpl;
class PosixProactor;

/**
 * @class PosixAsynchResult
 *
 * This class provides concrete implementation for <ACE_Asynch_Result>
 * for POSIX4 platforms. This class extends <aiocb> and makes it more
 * useful.
 */
class ACE_Export PosixAsynchResult : public virtual AsynchResultImpl,
                                           public aiocb
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
   * @@ This is not implemented for POSIX4 platforms.
   *
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// This returns ACE_INVALID_HANDLE on POSIX4 platforms.
  ACE_HANDLE event (void) const;

  /**
   * This really make sense only when doing file I/O.
   *
   * @@ On POSIX4-Unix, offset_high should be supported using
   *     aiocb64.
   *
   */
  u_long offset (void) const;
  u_long offset_high (void) const;

  /// Priority of the operation.
  int priority (void) const;

  /**
   * POSIX4 realtime signal number to be used for the
   * operation. <signal_number> ranges from SIGRTMIN to SIGRTMAX. By
   * default, SIGRTMIN is used to issue <aio_> calls.
   */
  int signal_number (void) const;

  /// Post <this> to the Proactor.
  int post_completion (ProactorImpl *proactor);

  /// Destructor.
  virtual ~PosixAsynchResult (void);

  /// Simulate error value to use in the post_completion ()
  void set_error (u_long errcode);

  /// Simulate value to use in the post_completion ()
  void set_bytes_transferred (u_long nbytes);

protected:
  /// Constructor. <Event> is not used on POSIX.
  PosixAsynchResult (ACE_Handler &handler,
                           const void* act,
                           ACE_HANDLE event,
                           u_long offset,
                           u_long offset_high,
                           int priority,
                           int signal_number);

  /// Handler that will be called back.
  ACE_Handler &handler_;

  /**
   * ACT for this operation.
   * We could use <aiocb::aio_sigevent.sigev_value.sival_ptr> for
   * this. But it doesnot provide the constness, so this may be
   * better.
   */
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
 * @class PosixAsynchOperation
 *
 * @brief This class abstracts out the common things needed for
 * implementing <Asynch_Operation> for POSIX platforms. Specific
 * implementations such as <POSIX_AIOCB_Asynch_Operation>
 * can derive from this class.
 */
class ACE_Export PosixAsynchOperation : public virtual AsynchOperationImpl
{
public:
  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <ACE_Handler::handle> will be called on the <handler> to get the
   * correct handle. No need for the Proactor since the sub classes
   * will know the correct implementation Proactor class, since this
   * Operation class itself was created by the correct implementation
   * Proactor class.
   */
  int open (ACE_Handler &handler,
            ACE_HANDLE handle,
            const void *completion_key,
            ACE_Proactor *proactor = 0);

  /// Check the documentation for <ACE_Asynch_Operation::cancel>.
  int cancel (void);

  // = Access methods.

  /// Return the underlying proactor.
  ACE_Proactor* proactor (void) const;

protected:
  /// No op contructor.
  PosixAsynchOperation (void);

  /// Destructor.
  virtual ~PosixAsynchOperation (void);

  /// Proactor that this Asynch IO will be registered with.
  ACE_Proactor *proactor_;

  /// Handler that will receive the callback.
  ACE_Handler *handler_;

  /// I/O handle used for reading.
  ACE_HANDLE handle_;
};

/**
 * @class PosixAiocbAsynchOperation
 *
 * @brief This class implements <ACE_Asynch_Operation> for <AIOCB>
 * (Asynchronous I/O Control Blocks) based implementation of
 * Proactor.
 */
class ACE_Export PosixAiocbAsynchOperation : public virtual PosixAsynchOperation
{
public:
  /// Return the underlying Proactor implementation.
  PosixAiocbProactor *posix_proactor (void) const;

protected:
  /// Contructor.
  PosixAiocbAsynchOperation (PosixAiocbProactor *posix_aiocb_proactor);

  /// Destructor.
  virtual ~PosixAiocbAsynchOperation (void);

  /// This is the method which does the real work to start aio
  /// and should be used instead of shared_read/shared_write
  /// <op> means operation : 0 - read , 1 - write.
  virtual int register_and_start_aio (PosixAsynchResult *result,
                                      int op);

  // This call is for the POSIX implementation. This method is used by
  // <ACE_Asynch_Operation> to store some information with the
  // Proactor after an <aio_> call is issued, so that the Proactor can
  // retreve this information to do <aio_return> and <aio_error>.
  // Passing a '0' ptr returns the status, indicating whether there
  // are slots available or no. Passing a valid ptr stores the ptr
  // with the Proactor.

  /**
   * It is easy to get this specific implementation proactor here,
   * since it is the one that creates the correct POSIX_Asynch_*
   * objects. We can use this to get to the implementation proactor
   * directly.
   */
  PosixAiocbProactor *posix_aiocb_proactor_;
};

/**
 * @class PosixAsynchReadStreamResult
 *
 * @brief This class provides concrete implementation for
 *    <ACE_Asynch_Read_Stream::Result> class for POSIX platforms.
 */
class ACE_Export PosixAsynchReadStreamResult : public virtual AsynchReadStreamResultImpl,
                                                       public PosixAsynchResult
{

  /// Factory classes willl have special permissions.
  friend class PosixAiocbAsynchReadStream;

  /// The Proactor constructs the Result class for faking results.
  friend class PosixProactor;

public:
  /// The number of bytes which were requested at the start of the
  /// asynchronous read.
  u_long bytes_to_read (void) const;

  /// Message block which contains the read data.
  MessageBlock &message_block (void) const;

  /// I/O handle used for reading.
  ACE_HANDLE handle (void) const;

  // = Base class operations. These operations are here to kill
  //   dominance warnings. These methods call the base class methods.

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
   * @@ This is not implemented for POSIX4 platforms.
   *
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// This returns ACE_INVALID_HANDLE.
  ACE_HANDLE event (void) const;

  /**
   * This really make sense only when doing file I/O.
   *
   * @@ On POSIX4-Unix, offset_high should be supported using
   *     aiocb64.
   *
   */
  u_long offset (void) const;
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation.
  int priority (void) const;

  /**
   * POSIX4 realtime signal number to be used for the
   * operation. <signal_number> ranges from SIGRTMIN to SIGRTMAX. By
   * default, SIGRTMIN is used to issue <aio_> calls. This is a no-op
   * on non-POSIX4 systems and returns 0.
   */
  int signal_number (void) const;

  /// Post <this> to the Proactor.
  int post_completion (ProactorImpl *proactor);

protected:
  PosixAsynchReadStreamResult (ACE_Handler &handler,
                                       ACE_HANDLE handle,
                                       MessageBlock &message_block,
                                       u_long bytes_to_read,
                                       const void* act,
                                       ACE_HANDLE event,
                                       int priority,
                                       int signal_number);
  // Constructor is protected since creation is limited to
  // ACE_Asynch_Read_Stream factory.

  /// Get the data copied to this class, before calling application
  /// handler.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destrcutor.
  virtual ~PosixAsynchReadStreamResult (void);

  // aiocb::aio_nbytes
  // Bytes requested when the asynchronous read was initiated.

  /// Message block for reading the data into.
  MessageBlock &message_block_;

  // aiocb::aio_filedes
  // I/O handle used for reading.
};

/**
 * @class PosixAiocbAsynchReadStream
 *
 * This class implements <ACE_Asynch_Read_Stream> for <AIOCB>
 * (Asynchronous I/O Control Blocks) based implementation of Proactor.
 *
 */
class ACE_Export PosixAiocbAsynchReadStream : public virtual AsynchReadStreamImpl,
                                                      public PosixAiocbAsynchOperation
{
public:
  /// Constructor.
  PosixAiocbAsynchReadStream (PosixAiocbProactor *posix_aiocb_proactor);

  /// This starts off an asynchronous read.  Upto <bytes_to_read> will
  /// be read and stored in the <message_block>.
  int read (MessageBlock &message_block,
            u_long bytes_to_read,
            const void *act,
            int priority,
            int signal_number = 0);

  /// Destructor.
  virtual ~PosixAiocbAsynchReadStream (void);

  // = Methods belong to PosixAsynchOperation base class. These
  //   methods are defined here to avoid dominace warnings. They route
  //   the call to the PosixAsynchOperation base class.

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <ACE_Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (ACE_Handler &handler,
            ACE_HANDLE handle,
            const void *completion_key,
            ACE_Proactor *proactor = 0);

  ///
  /// @@ Not implemented. Returns 0.
  int cancel (void);

  /// Return the underlying proactor.
  ACE_Proactor* proactor (void) const;

};


/**
 * @class PosixAsynchWriteStreamResult
 *
 * @brief This class provides concrete implementation for
 *     <ACE_Asynch_Write_Stream::Result> on POSIX platforms.
 *
 *
 *     This class has all the information necessary for the
 *     <handler> to uniquiely identify the completion of the
 *     asynchronous write.
 */
class ACE_Export PosixAsynchWriteStreamResult : public virtual AsynchWriteStreamResultImpl,
                                                        public PosixAsynchResult
{
  /// Factory classes will have special privilages.
  friend class PosixAiocbAsynchWriteStream;

  /// The Proactor constructs the Result class for faking results.
  friend class PosixProactor;

public:
  /// The number of bytes which were requested at the start of the
  /// asynchronous write.
  u_long bytes_to_write (void) const;

  /// Message block that contains the data to be written.
  MessageBlock &message_block (void) const;

  /// I/O handle used for writing.
  ACE_HANDLE handle (void) const;

  // = Base class operations. These operations are here to kill
  //   dominance warnings. These methods call the base class methods.

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
   * @@ This is not implemented for POSIX4 platforms.
   *
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// This returns ACE_INVALID_HANDLE on POSIX4 platforms.
  ACE_HANDLE event (void) const;

  /**
   * This really make sense only when doing file I/O.
   *
   * @@ On POSIX4-Unix, offset_high should be supported using
   *     aiocb64.
   *
   */
  u_long offset (void) const;
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation.
  int priority (void) const;

  /**
   * POSIX4 realtime signal number to be used for the
   * operation. <signal_number> ranges from SIGRTMIN to SIGRTMAX. By
   * default, SIGRTMIN is used to issue <aio_> calls. This is a no-op
   * on non-POSIX4 systems and returns 0.
   */
  int signal_number (void) const;

  /// Post <this> to the Proactor.
  int post_completion (ProactorImpl *proactor);

protected:
  PosixAsynchWriteStreamResult (ACE_Handler &handler,
                                        ACE_HANDLE handle,
                                        MessageBlock &message_block,
                                        u_long bytes_to_write,
                                        const void* act,
                                        ACE_HANDLE event,
                                        int priority,
                                        int signal_number);
  // Constructor is protected since creation is limited to
  // ACE_Asynch_Write_Stream factory.

  /// ACE_Proactor will call this method when the write completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~PosixAsynchWriteStreamResult (void);

protected:
  // aiocb::aio_nbytes
  // The number of bytes which were requested at the start of the
  // asynchronous write.

  /// Message block that contains the data to be written.
  MessageBlock &message_block_;

  // aiocb::aio_filedes
  // I/O handle used for writing.
};

/**
 * @class PosixAiocbAsynchWriteStream
 *
 * @brief This class implements <ACE_Asynch_Write_Stream> for <AIOCB>
 *     (Asynchronous I/O Control Blocks) based implementation of
 *      Proactor.
 */
class ACE_Export PosixAiocbAsynchWriteStream : public virtual AsynchWriteStreamImpl,
                                                       public PosixAiocbAsynchOperation
{
public:
  /// Constructor.
  PosixAiocbAsynchWriteStream (PosixAiocbProactor *posix_aiocb_proactor);

  /// This starts off an asynchronous write.  Upto <bytes_to_write>
  /// will be written from the <message_block>.
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             const void *act,
             int priority,
             int signal_number = 0);

  /// Destructor.
  virtual ~PosixAiocbAsynchWriteStream (void);

  // = Methods belong to PosixAsynchOperation base class. These
  //   methods are defined here to avoid dominace warnings. They route
  //   the call to the PosixAsynchOperation base class.

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <ACE_Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (ACE_Handler &handler,
            ACE_HANDLE handle,
            const void *completion_key,
            ACE_Proactor *proactor =  0);

  ///
  /// @@ Not implemented. Returns 0.
  int cancel (void);

  /// Return the underlying proactor.
  ACE_Proactor* proactor (void) const;

};

/**
 * @class PosixAsynchReadFileResult
 *
 * @brief This class provides concrete implementation for
 *     <ACE_Asynch_Read_File::Result> class for POSIX platforms.
 */
class ACE_Export PosixAsynchReadFileResult : public virtual AsynchReadFileResultImpl,
                                                     public PosixAsynchReadStreamResult
{
  /// Factory classes willl have special permissions.
  friend class PosixAiocbAsynchReadFile;

  /// The Proactor constructs the Result class for faking results.
  friend class PosixProactor;

public:
  // = These methods belong to PosixAsynchResult class base
  //   class. These operations are here to kill dominance
  //   warnings. These methods call the base class methods.

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
   * @@ This is not implemented for POSIX4 platforms.
   *
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// This returns ACE_INVALID_HANDLE on POSIX4 platforms.
  ACE_HANDLE event (void) const;

  /**
   * This really make sense only when doing file I/O.
   *
   * @@ On POSIX4-Unix, offset_high should be supported using
   *     aiocb64.
   *
   */
  u_long offset (void) const;
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation.
  int priority (void) const;

  /**
   * POSIX4 realtime signal number to be used for the
   * operation. <signal_number> ranges from SIGRTMIN to SIGRTMAX. By
   * default, SIGRTMIN is used to issue <aio_> calls. This is a no-op
   * on non-POSIX4 systems and returns 0.
   */
  int signal_number (void) const;

  // = The following methods belong to
  //   PosixAsynchReadStreamResult. They are here to avoid
  //   dominance warnings. These methods route their call to the
  //   PosixAsynchReadStreamResult base class.

  /// The number of bytes which were requested at the start of the
  /// asynchronous read.
  u_long bytes_to_read (void) const;

  /// Message block which contains the read data.
  MessageBlock &message_block (void) const;

  /// I/O handle used for reading.
  ACE_HANDLE handle (void) const;

  /// Post <this> to the Proactor.
  int post_completion (ProactorImpl *proactor);

protected:
  PosixAsynchReadFileResult (ACE_Handler &handler,
                                     ACE_HANDLE handle,
                                     MessageBlock &message_block,
                                     u_long bytes_to_read,
                                     const void* act,
                                     u_long offset,
                                     u_long offset_high,
                                     ACE_HANDLE event,
                                     int priority,
                                     int signal_number);
  // Constructor is protected since creation is limited to
  // ACE_Asynch_Read_File factory.

  /// ACE_Proactor will call this method when the read completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~PosixAsynchReadFileResult (void);
};

/**
 * @class PosixAiocbAsynchReadFile
 *
 * @brief This class is a factory for starting off asynchronous reads
 *     on a file. This class implements <ACE_Asynch_Read_File> for
 *     <AIOCB> (Asynchronous I/O Control Blocks) based implementation
 *     of Proactor.
 *
 *     Once <open> is called, multiple asynchronous <read>s can
 *     started using this class.  A <ACE_Asynch_Read_File::Result>
 *     will be passed back to the <handler> when the asynchronous
 *     reads completes through the <ACE_Handler::handle_read_file>
 *     callback.
 *
 *     This class differs slightly from <ACE_Asynch_Read_Stream> as it
 *     allows the user to specify an offset for the read.
 */
class ACE_Export PosixAiocbAsynchReadFile : public virtual AsynchReadFileImpl,
                                                    public PosixAiocbAsynchReadStream
{

public:
  /// Constructor.
  PosixAiocbAsynchReadFile (PosixAiocbProactor *posix_aiocb_proactor);

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
  virtual ~PosixAiocbAsynchReadFile (void);


  // = Methods belong to PosixAsynchOperation base class. These
  //   methods are defined here to avoid dominace warnings. They route
  //   the call to the PosixAsynchOperation base class.

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <ACE_Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (ACE_Handler &handler,
            ACE_HANDLE handle,
            const void *completion_key,
            ACE_Proactor *proactor = 0);

  ///
  /// @@ Not implemented. Returns 0.
  int cancel (void);

  /// Return the underlying proactor.
  ACE_Proactor* proactor (void) const;

private:
  /**
   * This belongs to PosixAiocbAsynchReadStream. We have
   * defined this here to avoid compiler warnings and forward the
   * method to <PosixAiocbAsynchReadStream::read>.
   */
  int read (MessageBlock &message_block,
            u_long bytes_to_read,
            const void *act,
            int priority,
            int signal_number = 0);
};


/**
 * @class PosixAsynchWriteFileResult
 *
 * @brief This class provides implementation for
 *     <ACE_Asynch_Write_File_Result> for POSIX platforms.
 *
 *     This class has all the information necessary for the
 *     <handler> to uniquiely identify the completion of the
 *     asynchronous write.
 *
 *     This class differs slightly from
 *     <ACE_Asynch_Write_Stream::Result> as it calls back
 *     <ACE_Handler::handle_write_file> on the <handler> instead of
 *     <ACE_Handler::handle_write_stream>.  No additional state is
 *     required by this class as <ACE_Asynch_Result> can store the
 *     <offset>.
 */
class ACE_Export PosixAsynchWriteFileResult : public virtual AsynchWriteFileResultImpl,
                                                      public PosixAsynchWriteStreamResult
{
  /// Factory classes will have special permissions.
  friend class PosixAiocbAsynchWriteFile;

  /// The Proactor constructs the Result class for faking results.
  friend class PosixProactor;

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
   * This is the ACT associated with the handle on which the
   * Asynch_Operation takes place.
   *
   * @@ This is not implemented for POSIX4 platforms.
   *
   */
  const void *completion_key (void) const;

  /// Error value if the operation fail.
  u_long error (void) const;

  /// This returns ACE_INVALID_HANDLE on POSIX4 platforms.
  ACE_HANDLE event (void) const;

  /**
   * This really make sense only when doing file I/O.
   *
   * @@ On POSIX4-Unix, offset_high should be supported using
   *     aiocb64.
   *
   */
  u_long offset (void) const;
  u_long offset_high (void) const;

  /// The priority of the asynchronous operation.
  int priority (void) const;

  /**
   * POSIX4 realtime signal number to be used for the
   * operation. <signal_number> ranges from SIGRTMIN to SIGRTMAX. By
   * default, SIGRTMIN is used to issue <aio_> calls. This is a no-op
   * on non-POSIX4 systems and returns 0.
   */
  int signal_number (void) const;

  // = The following methods belong to
  //   PosixAsynchWriteStreamResult. They are here to avoid
  //   dominace warnings. These methods route their call to the
  //   PosixAsynchWriteStreamResult base class.

  /// The number of bytes which were requested at the start of the
  /// asynchronous write.
  u_long bytes_to_write (void) const;

  /// Message block that contains the data to be written.
  MessageBlock &message_block (void) const;

  /// I/O handle used for writing.
  ACE_HANDLE handle (void) const;

  /// Post <this> to the Proactor.
  int post_completion (ProactorImpl *proactor);

protected:
  PosixAsynchWriteFileResult (ACE_Handler &handler,
                                      ACE_HANDLE handle,
                                      MessageBlock &message_block,
                                      u_long bytes_to_write,
                                      const void* act,
                                      u_long offset,
                                      u_long offset_high,
                                      ACE_HANDLE event,
                                      int priority,
                                      int signal_number);
  // Constructor is protected since creation is limited to
  // ACE_Asynch_Write_File factory.

  /// ACE_Proactor will call this method when the write completes.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error);

  /// Destructor.
  virtual ~PosixAsynchWriteFileResult (void);
};

/**
 * @class PosixAiocbAsynchWriteFile
 *
 *     This class provides concrete implementation for
 *     <ACE_Asynch_Write_File> for POSIX platforms where the
 *     completion strategy for Proactor is based on AIOCB (AIO
 *     Control Blocks).
 *
 */
class ACE_Export PosixAiocbAsynchWriteFile : public virtual ACE_Asynch_Write_File_Impl,
                                                     public PosixAiocbAsynchWriteStream
{
public:
  /// Constructor.
  PosixAiocbAsynchWriteFile (PosixAiocbProactor *posix_aiocb_proactor);

  /**
   * This starts off an asynchronous write.  Upto <bytes_to_write>
   * will be written and stored in the <message_block>.  The write will
   * start at <offset> from the beginning of the file.
   */
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             u_long offset,
             u_long offset_high,
             const void *act,
             int priority,
             int signal_number = 0);

  /// Destructor.
  virtual ~PosixAiocbAsynchWriteFile (void);

  // = Methods belong to PosixAsynchOperation base class. These
  //   methods are defined here to avoid dominace warnings. They route
  //   the call to the PosixAsynchOperation base class.

  /**
   * Initializes the factory with information which will be used with
   * each asynchronous call.  If (<handle> == ACE_INVALID_HANDLE),
   * <ACE_Handler::handle> will be called on the <handler> to get the
   * correct handle.
   */
  int open (ACE_Handler &handler,
            ACE_HANDLE handle,
            const void *completion_key,
            ACE_Proactor *proactor = 0);

  ///
  /// @@ Not implemented. Returns 0.
  int cancel (void);

  /// Return the underlying proactor.
  ACE_Proactor* proactor (void) const;

private:
  /**
   * This <write> belongs to PosixAiocbAsynchWriteStream. We
   * have put this here to avoid compiler warnings. We forward this
   * method call to the <PosixAiocbAsynchWriteStream::write>
   * one.
   */
  int write (MessageBlock &message_block,
             u_long bytes_to_write,
             const void *act,
             int priority,
             int signal_number = 0);
};


//#endif /* ACE_HAS_AIO_CALLS */
#endif /* ACE_POSIX_ASYNCH_IO_H */
