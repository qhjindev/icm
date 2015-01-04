/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    WIN32_Proactor.h
 *
 *  WIN32_Proactor.h,v 4.16 2001/05/17 21:42:00 schmidt Exp
 *
 *  @author Irfan Pyarali (irfan@cs.wustl.edu)
 *  @author Tim Harrison (harrison@cs.wustl.edu)
 *  @author Alexander Babu Arulanthu <alex@cs.wustl.edu>
 *  @author Roger Tragin <r.tragin@computer.org>
 */
//=============================================================================

#ifndef ACE_WIN32_PROACTOR_H
#define ACE_WIN32_PROACTOR_H


#include "os/OS.h"
#include "icc/Win32AsynchIo.h"
#include "icc/EventHandler.h"
#include "icc/AutoEvent.h"

#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE))
// WIN32 implementation of the Proactor.

#include "icc/ProactorImpl.h"

// Forward declarations.
class Win32AsynchResult;
class ACE_WIN32_Proactor_Timer_Handler;


/**
 * @class Win32Proactor
 *
 * @brief A manager for asynchronous event demultiplexing.
 *
 * See the Proactor pattern description at
 * http://www.cs.wustl.edu/~schmidt/proactor.ps.gz for more
 * details.
 */
class ACE_Export Win32Proactor : public ProactorImpl
{
public:
  /// A do nothing constructor.
  Win32Proactor (size_t number_of_threads = 0,
                      int used_with_reactor_event_loop = 0);

  /// Virtual destruction.
  virtual ~Win32Proactor (void);

  /// Close the IO completion port.
  virtual int close (void);

  /// This method adds the <handle> to the I/O completion port. This
  /// function is a no-op function for Unix systems.
  virtual int register_handle (ACE_HANDLE handle,
			       const void *completion_key);

  /**
   * Dispatch a single set of events.  If <wait_time> elapses before
   * any events occur, return 0.  Return 1 on success i.e., when a
   * completion is dispatched, non-zero (-1) on errors and errno is
   * set accordingly.
   */
  virtual int handle_events (TimeValue &wait_time);

  /**
   * Block indefinitely until at least one event is dispatched.
   * Dispatch a single set of events.  If <wait_time> elapses before
   * any events occur, return 0.  Return 1 on success i.e., when a
   * completion is dispatched, non-zero (-1) on errors and errno is
   * set accordingly.
   */
  virtual int handle_events (void);

  /**
   * Post a result to the completion port of the Proactor.  If errors
   * occur, the result will be deleted by this method.  If successful,
   * the result will be deleted by the Proactor when the result is
   * removed from the completion port.  Therefore, the result should
   * have been dynamically allocated and should be orphaned by the
   * user once this method is called.
   */
  virtual int post_completion (Win32AsynchResult *result);

  /// Add wakeup dispatch threads (reinit).
  int wake_up_dispatch_threads (void);

  /// Close all dispatch threads.
  int close_dispatch_threads (int wait);

  /// Number of thread used as a parameter to CreatIoCompletionPort.
  size_t number_of_threads (void) const;
  void number_of_threads (size_t threads);

  /// Get the event handle.
  virtual ACE_HANDLE get_handle (void) const;

  virtual AsynchReadStreamImpl *create_asynch_read_stream (void);
  virtual AsynchWriteStreamImpl *create_asynch_write_stream (void);
  virtual AsynchReadFileImpl *create_asynch_read_file (void);
  virtual AsynchWriteFileImpl *create_asynch_write_file (void);

  // Methods used to create Asynch_IO_Result objects. We create the right
  // objects here in these methods.

  virtual AsynchReadStreamResultImpl *create_asynch_read_stream_result (Handler &handler,
                                                                                ACE_HANDLE handle,
                                                                                MessageBlock &message_block,
                                                                                u_long bytes_to_read,
                                                                                const void* act,
                                                                                ACE_HANDLE event,
                                                                                int priority,
                                                                                int signal_number = 0);

  virtual AsynchWriteStreamResultImpl *create_asynch_write_stream_result (Handler &handler,
                                                                                  ACE_HANDLE handle,
                                                                                  MessageBlock &message_block,
                                                                                  u_long bytes_to_write,
                                                                                  const void* act,
                                                                                  ACE_HANDLE event,
                                                                                  int priority,
                                                                                  int signal_number = 0);

  virtual AsynchReadFileResultImpl *create_asynch_read_file_result (Handler &handler,
                                                                            ACE_HANDLE handle,
                                                                            MessageBlock &message_block,
                                                                            u_long bytes_to_read,
                                                                            const void* act,
                                                                            u_long offset,
                                                                            u_long offset_high,
                                                                            ACE_HANDLE event,
                                                                            int priority,
                                                                            int signal_number = 0);

  virtual AsynchWriteFileResultImpl *create_asynch_write_file_result (Handler &handler,
                                                                              ACE_HANDLE handle,
                                                                              MessageBlock &message_block,
                                                                              u_long bytes_to_write,
                                                                              const void* act,
                                                                              u_long offset,
                                                                              u_long offset_high,
                                                                              ACE_HANDLE event,
                                                                              int priority,
                                                                              int signal_number = 0);


  /// Create a timer result object which can be used with the Timer
  /// mechanism of the Proactor.
  virtual AsynchResultImpl *create_asynch_timer (Handler &handler,
                                                       const void *act,
                                                       const TimeValue &tv,
                                                       ACE_HANDLE event,
                                                       int priority,
                                                       int signal_number = 0);

protected:
  /// Called when object is signaled by OS (either via UNIX signals or
  /// when a Win32 object becomes signaled).
  //virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0);

  /// Called when object is removed from the ACE_Reactor.
  virtual int handleClose (ACE_HANDLE handle,
			    ReactorMask close_mask);

  /**
   * Dispatch a single set of events.  If <milli_seconds> elapses
   * before any events occur, return 0. Return 1 if a completion is
   * dispatched. Return -1 on errors.
   */
  virtual int handle_events (unsigned long milli_seconds);

  /// Protect against structured exceptions caused by user code when
  /// dispatching handles.
  void application_specific_code (Win32AsynchResult *asynch_result,
				  u_long bytes_transferred,
				  int success,
				  const void *completion_key,
				  u_long error);

  /**
   * Post <how_many> completions to the completion port so that all
   * threads can wake up. This is used in conjunction with the
   * <run_event_loop>.
   */
  virtual int post_wakeup_completions (int how_many);

  /// Handle for the completion port. Unix doesnt have completion
  /// ports.
  ACE_HANDLE completion_port_;

  /// This number is passed to the <CreatIOCompletionPort> system
  /// call.
  size_t number_of_threads_;

  /// This event is used in conjunction with Reactor when we try to
  /// integrate the event loops of Reactor and the Proactor.
  ACE_Auto_Event event_;

  /// Flag that indicates whether we are used in conjunction with
  /// Reactor.
  int used_with_reactor_event_loop_;

  /// Handler to handle the wakeups. This works in conjunction with the
  /// <Proactor::run_event_loop>.
  Handler wakeup_handler_;
};

/**
 * @class Win32AsynchTimer
 *
 * @brief This class is posted to the completion port when a timer
 * expires. When the complete method of this object is
 * called, the <handler>'s handle_timeout method will be
 * called.
 */
class ACE_Export Win32AsynchTimer : public Win32AsynchResult
{

  /// The factory method for this class is with the POSIX_Proactor
  /// class.
  friend class Win32Proactor;

protected:
  /// Constructor.
  Win32AsynchTimer (Handler &handler,
                          const void *act,
                          const TimeValue &tv,
                          ACE_HANDLE event = ACE_INVALID_HANDLE,
                          int priority = 0,
                          int signal_number = 0);

  /// This method calls the <handler>'s handle_timeout method.
  virtual void complete (u_long bytes_transferred,
                         int success,
                         const void *completion_key,
                         u_long error = 0);

  /// Time value requested by caller
  TimeValue time_;
};

#endif /* ACE_WIN32 */

#endif /* ACE_PROACTOR_H */
