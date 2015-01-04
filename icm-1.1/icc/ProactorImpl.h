/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    Proactor_Impl.h
 *
 *  Proactor_Impl.h,v 4.14 2001/05/17 15:35:41 schmidt Exp
 *
 *  @author Alexander Babu Arulanthu <alex@cs.wustl.edu>
 */
//=============================================================================


#ifndef ACE_PROACTOR_IMPL_H
#define ACE_PROACTOR_IMPL_H

#include "os/OS.h"

#if ((defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS)))
// This only works on Win32 platforms and on Unix platforms supporting
// aio calls.

#include "icc/AsynchIo.h"
#include "icc/Reactor.h"

/**
 * @class ProactorImpl
 *
 * @brief A manager for asynchronous event demultiplexing. This class
 * is the base class for all the concrete implementation
 * classes.
 *
 * See the Proactor pattern description at
 * http://www.cs.wustl.edu/~schmidt/proactor.ps.gz for more
 * details.
 */
class ACE_Export ProactorImpl : public EventHandler
{

public:
  /// Virtual destruction.
  virtual ~ProactorImpl (void) {}

  /// Close the IO completion port.
  virtual int close (void) = 0;

  /// This method adds the <handle> to the I/O completion port. This
  /// function is a no-op function for Unix systems.
  virtual int register_handle (ACE_HANDLE handle,
			       const void *completion_key) = 0;

  /**
   * Dispatch a single set of events.  If <wait_time> elapses before
   * any events occur, return 0.  Return 1 on success i.e., when a
   * completion is dispatched, non-zero (-1) on errors and errno is
   * set accordingly.
   */
  virtual int handle_events (TimeValue &wait_time) = 0;

  /**
   * Block indefinitely until at least one event is dispatched.
   * Dispatch a single set of events.  If <wait_time> elapses before
   * any events occur, return 0.  Return 1 on success i.e., when a
   * completion is dispatched, non-zero (-1) on errors and errno is
   * set accordingly.
   */
  virtual int handle_events (void) = 0;

  /// Add wakeup dispatch threads (reinit).
  virtual int wake_up_dispatch_threads (void) = 0;

  /// Close all dispatch threads.
  virtual int close_dispatch_threads (int wait) = 0;

  /// Number of thread used as a parameter to CreatIoCompletionPort.
  virtual size_t number_of_threads (void) const = 0;
  virtual void number_of_threads (size_t threads) = 0;

  /// Get the event handle.
  virtual ACE_HANDLE get_handle (void) const = 0;

  //
  // = Factory methods for the operations
  //
  // Note that the user does not have to use or know about these
  // methods.

  /// Create the correct implementation class for doing Asynch_Read_Stream.
  virtual AsynchReadStreamImpl *create_asynch_read_stream (void) = 0;

  /// Create the correct implementation class for doing Asynch_Write_Stream.
  virtual AsynchWriteStreamImpl *create_asynch_write_stream (void) = 0;

  /// Create the correct implementation class for doing Asynch_Read_File.
  virtual AsynchReadFileImpl *create_asynch_read_file (void) = 0;

    /// Create the correct implementation class for doing Asynch_Write_File.
  virtual AsynchWriteFileImpl *create_asynch_write_file (void) = 0;


  //
  // = Factory methods for the results
  //
  // Note that the user does not have to use or know about these
  // methods unless they want to "fake" results.

  /// Create the correct implementation class for AsynchReadStream::Result class.
  virtual AsynchReadStreamResultImpl *create_asynch_read_stream_result (Handler &handler,
                                                                                ACE_HANDLE handle,
                                                                                MessageBlock &message_block,
                                                                                u_long bytes_to_read,
                                                                                const void* act,
                                                                                ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                                int priority = 0,
                                                                                int signal_number = ACE_SIGRTMIN) = 0;

  /// Create the correct implementation class for AsynchWriteStream::Result.
  virtual AsynchWriteStreamResultImpl *create_asynch_write_stream_result (Handler &handler,
                                                                                  ACE_HANDLE handle,
                                                                                  MessageBlock &message_block,
                                                                                  u_long bytes_to_write,
                                                                                  const void* act,
                                                                                  ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                                  int priority = 0,
                                                                                  int signal_number = ACE_SIGRTMIN) = 0;

  /// Create the correct implementation class for AsynchReadFile::Result.
  virtual AsynchReadFileResultImpl *create_asynch_read_file_result (Handler &handler,
                                                                            ACE_HANDLE handle,
                                                                            MessageBlock &message_block,
                                                                            u_long bytes_to_read,
                                                                            const void* act,
                                                                            u_long offset,
                                                                            u_long offset_high,
                                                                            ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                            int priority = 0,
                                                                            int signal_number = ACE_SIGRTMIN) = 0;

    /// Create the correct implementation class for AsynchWriteFile::Result.
  virtual AsynchWriteFileResultImpl *create_asynch_write_file_result (Handler &handler,
                                                                              ACE_HANDLE handle,
                                                                              MessageBlock &message_block,
                                                                              u_long bytes_to_write,
                                                                              const void* act,
                                                                              u_long offset,
                                                                              u_long offset_high,
                                                                              ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                                              int priority = 0,
                                                                              int signal_number = ACE_SIGRTMIN) = 0;

  /**
   * Create the correct implementation object for the Timer
   * result. POSIX_SIG_Proactor will create a Timer object with a
   * meaningful signal number, if you leave the signal number as 0.
   */
  virtual AsynchResultImpl *create_asynch_timer (Handler &handler,
                                                       const void *act,
                                                       const TimeValue &tv,
                                                       ACE_HANDLE event = ACE_INVALID_HANDLE,
                                                       int priority = 0,
                                                       int signal_number = 0) = 0;

  /**
   * Post <how_many> completions to the completion port so that all
   * threads can wake up. This is used in conjunction with the
   * <run_event_loop>.
   */
  virtual int post_wakeup_completions (int how_many) = 0;
};

#endif /* (ACE_WIN32 && ACE_HAS_WINCE) || ACE_HAS_AIO_CALLS */

#endif /* ACE_PROACTOR_IMPL_H */
