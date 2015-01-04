#include "os/OS_NS_Thread.h"

//#include "os/OS_NS_stdio.h"
//#include "Sched_Params.h"
#include "os/OS_Memory.h"
//#include "os/OS_Thread_Adapter.h"
#include "os/Min_Max.h"
//#include "Object_Manager_Base.h"
#include "os/OS_NS_errno.h"
//#include "os/OS_NS_ctype.h"
//#include "Log_Msg.h" // for ACE_ASSERT
// This is necessary to work around nasty problems with MVS C++.
//#include "Auto_Ptr.h"
#include "icc/ThreadMutex.h"
//#include "Condition_T.h"
//#include "Guard_T.h"

#include "os/OS_NS_macros.h"
// for timespec_t, perhaps move it to os_time.h
#include "icc/TimeValue.h"
//#include "os/OS_NS_sys_mman.h"
#include "os/OS_NS_sys_time.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_unistd.h"
#include "os/OS_NS_stdio.h"
#include "os/OS_NS_errno.h"

#if defined (ACE_USES_FIFO_SEM)
#  include "OS_NS_sys_stat.h"
#  include "OS_NS_sys_select.h"
#  include "OS_NS_fcntl.h"
#  include "Handle_Set.h"
# endif /* ACE_USES_FIFO_SEM */

#if defined (ACE_HAS_PRIOCNTL)
#  include /**/ <sys/priocntl.h>
#endif /* ACE_HAS_PRIOCNTL */

#include <memory>

#define ACE_HAS_THREADS
#if !defined(ACE_WIN32)
#define ACE_HAS_PTHREADS
#endif //ACE_WIN32

extern "C" void
ACE_MUTEX_LOCK_CLEANUP_ADAPTER_NAME (void *args)
{
  //ACE_VERSIONED_NAMESPACE_NAME::OS::mutex_lock_cleanup (args);
}


#if !defined(ACE_WIN32) && defined (__IBMCPP__) && (__IBMCPP__ >= 400)
# define ACE_BEGINTHREADEX(STACK, STACKSIZE, ENTRY_POINT, ARGS, FLAGS, THR_ID) \
       (*THR_ID = ::_beginthreadex ((void(_Optlink*)(void*))ENTRY_POINT, STACK, STACKSIZE, ARGS), *THR_ID)
#elif defined (ACE_HAS_WINCE) && defined (UNDER_CE) && (UNDER_CE >= 211)
# define ACE_BEGINTHREADEX(STACK, STACKSIZE, ENTRY_POINT, ARGS, FLAGS, THR_ID) \
      CreateThread (0, STACKSIZE, (unsigned long (__stdcall *) (void *)) ENTRY_POINT, ARGS, (FLAGS) & CREATE_SUSPENDED, (unsigned long *) THR_ID)
#elif defined(ACE_HAS_WTHREADS)
  // Green Hills compiler gets confused when __stdcall is imbedded in
  // parameter list, so we define the type ACE_WIN32THRFUNC_T and use it
  // instead.
  typedef unsigned (__stdcall *ACE_WIN32THRFUNC_T)(void*);
# define ACE_BEGINTHREADEX(STACK, STACKSIZE, ENTRY_POINT, ARGS, FLAGS, THR_ID) \
      ::_beginthreadex (STACK, STACKSIZE, (ACE_WIN32THRFUNC_T) ENTRY_POINT, ARGS, FLAGS, (unsigned int *) THR_ID)
#endif /* defined (__IBMCPP__) && (__IBMCPP__ >= 400) */



/*****************************************************************************/

#if defined (ACE_LACKS_COND_T) && defined (ACE_HAS_THREADS)
ACE_INLINE long
ACE_cond_t::waiters (void) const
{
  return this->waiters_;
}
#endif /* ACE_LACKS_COND_T && ACE_HAS_THREADS */

/*****************************************************************************/

#if defined (ACE_HAS_TSS_EMULATION)

#  if !defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
ACE_INLINE
void **&
ACE_TSS_Emulation::tss_base ()
{
#    if defined (ACE_VXWORKS)
  return (void **&) taskIdCurrent->ACE_VXWORKS_SPARE;
#    else
  // Uh oh.
  ACE_NOTSUP_RETURN (0);
#    endif /* ACE_VXWORKS */
}
#  endif /* ! ACE_HAS_THREAD_SPECIFIC_STORAGE */

ACE_INLINE
ACE_TSS_Emulation::ACE_TSS_DESTRUCTOR
ACE_TSS_Emulation::tss_destructor (const ACE_thread_key_t key)
{
  ACE_KEY_INDEX (key_index, key);
  return tss_destructor_ [key_index];
}

ACE_INLINE
void
ACE_TSS_Emulation::tss_destructor (const ACE_thread_key_t key,
                                   ACE_TSS_DESTRUCTOR destructor)
{
  ACE_KEY_INDEX (key_index, key);
  tss_destructor_ [key_index] = destructor;
}

ACE_INLINE
void *&
ACE_TSS_Emulation::ts_object (const ACE_thread_key_t key)
{
  ACE_KEY_INDEX (key_index, key);

#    if defined (ACE_VXWORKS)
    /* If someone wants tss_base make sure they get one.  This
       gets used if someone spawns a VxWorks task directly, not
       through ACE.  The allocated array will never be deleted! */
    if (0 == taskIdCurrent->ACE_VXWORKS_SPARE)
      {
        taskIdCurrent->ACE_VXWORKS_SPARE =
          reinterpret_cast<int> (new void *[ACE_TSS_THREAD_KEYS_MAX]);

        // Zero the entire TSS array.  Do it manually instead of using
        // memset, for optimum speed.  Though, memset may be faster :-)
        void **tss_base_p =
          reinterpret_cast<void **> (taskIdCurrent->ACE_VXWORKS_SPARE);
        for (u_int i = 0; i < ACE_TSS_THREAD_KEYS_MAX; ++i, ++tss_base_p)
          {
            *tss_base_p = 0;
          }
      }
#    endif /* ACE_VXWORKS */

  return tss_base ()[key_index];
}

#endif /* ACE_HAS_TSS_EMULATION */

/*****************************************************************************/

ACE_INLINE int
OS::thr_equal (ACE_thread_t t1, ACE_thread_t t2)
{
#if defined (ACE_HAS_PTHREADS)
# if defined (pthread_equal)
  // If it's a macro we can't say "pthread_equal"...
  return pthread_equal (t1, t2);
# else
  return pthread_equal (t1, t2);
# endif /* pthread_equal */
#elif defined (ACE_VXWORKS)
  return ! OS::strcmp (t1, t2);
#else /* For both STHREADS and WTHREADS... */
  // Hum, Do we need to treat WTHREAD differently?
  // levine 13 oct 98 % I don't think so, ACE_thread_t is a DWORD.
  return t1 == t2;
#endif /* ACE_HAS_PTHREADS */
}

#if !defined (ACE_LACKS_COND_T)
// NOTE: The OS::cond_* functions for Unix platforms are defined
// here because the OS::sema_* functions below need them.
// However, ACE_WIN32 and VXWORKS define the OS::cond_* functions
// using the OS::sema_* functions.  So, they are defined in OS.cpp.

ACE_INLINE int
OS::condattr_destroy (ACE_condattr_t &attributes)
{
#if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)

#     if defined (ACE_HAS_PTHREADS_DRAFT4)
  pthread_condattr_delete (&attributes);
#     else
  pthread_condattr_destroy (&attributes);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */

#   elif defined (ACE_HAS_STHREADS)
  attributes.type = 0;

#   endif /* ACE_HAS_PTHREADS vs. ACE_HAS_STHREADS */
  return 0;
# else
  ACE_UNUSED_ARG (attributes);
  return 0;
# endif /* ACE_HAS_THREADS  */
}

ACE_INLINE int
OS::condattr_init (ACE_condattr_t &attributes,
                       int type)
{
  ACE_UNUSED_ARG (type);
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = -1;

#   if defined (ACE_VXWORKS) && (ACE_VXWORKS >= 0x600) && (ACE_VXWORKS <= 0x620)
      /* Tests show that VxWorks 6.x pthread lib does not only
       * require zeroing of mutex/condition objects to function correctly
       * but also of the attribute objects.
       */
      OS::memset (&attributes, 0, sizeof (attributes));
#   endif
  if (
#     if defined  (ACE_HAS_PTHREADS_DRAFT4)
      pthread_condattr_create (&attributes) == 0
#     elif defined (ACE_HAS_PTHREADS_STD) || defined (ACE_HAS_PTHREADS_DRAFT7)
      ACE_ADAPT_RETVAL (pthread_condattr_init (&attributes), result) == 0
#       if defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)
      && ACE_ADAPT_RETVAL (pthread_condattr_setpshared (&attributes, type),
                           result) == 0
#       endif /* _POSIX_THREAD_PROCESS_SHARED && ! ACE_LACKS_CONDATTR_PSHARED */
#     else  /* this is draft 6 */
      pthread_condattr_init (&attributes) == 0
#       if !defined (ACE_LACKS_CONDATTR_PSHARED)
      && pthread_condattr_setpshared (&attributes, type) == 0
#       endif /* ACE_LACKS_CONDATTR_PSHARED */
#       if defined (ACE_HAS_PTHREAD_CONDATTR_SETKIND_NP)
      && pthread_condattr_setkind_np (&attributes, type) == 0
#       endif /* ACE_HAS_PTHREAD_CONDATTR_SETKIND_NP */
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
      )
     result = 0;
  else
     result = -1;       // ACE_ADAPT_RETVAL used it for intermediate status

  return result;
#   elif defined (ACE_HAS_STHREADS)
  attributes.type = type;

  return 0;

#   else
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (type);
  ACE_NOTSUP_RETURN (-1);

#   endif /* ACE_HAS_PTHREADS vs. ACE_HAS_STHREADS */

# else
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (type);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::cond_broadcast (ACE_cond_t *cv)
{
  OS_TRACE ("OS::cond_broadcast");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
#     if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
  OSCALL_RETURN (pthread_cond_broadcast (cv), int, -1);
#     else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_broadcast (cv),
                                       result),
                     int, -1);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
#   elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::cond_broadcast (cv),
                                       result),
                     int, -1);
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::cond_destroy (ACE_cond_t *cv)
{
  OS_TRACE ("OS::cond_destroy");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
#     if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
  OSCALL_RETURN (pthread_cond_destroy (cv), int, -1);
#     else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_destroy (cv), result), int, -1);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
#   elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::cond_destroy (cv), result), int, -1);
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::cond_init (ACE_cond_t *cv,
                   ACE_condattr_t &attributes,
                   const char *name,
                   void *arg)
{
  // OS_TRACE ("OS::cond_init");
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
  int result = -1;

#     if defined (ACE_VXWORKS) && (ACE_VXWORKS >= 0x600) && (ACE_VXWORKS <= 0x620)
  /* VxWorks 6.x API reference states:
   *   If the memory for the condition variable object has been allocated
   *   dynamically, it is a good policy to always zero out the
   *   block of memory so as to avoid spurious EBUSY return code
   *   when calling this routine.
   */
  OS::memset (cv, 0, sizeof (*cv));
#     endif

  if (
#     if defined  (ACE_HAS_PTHREADS_DRAFT4)
      pthread_cond_init (cv, attributes) == 0
#     elif defined (ACE_HAS_PTHREADS_STD) || defined (ACE_HAS_PTHREADS_DRAFT7)
      ACE_ADAPT_RETVAL (pthread_cond_init (cv, &attributes), result) == 0
#     else  /* this is draft 6 */
      pthread_cond_init (cv, &attributes) == 0
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
      )
     result = 0;
  else
     result = -1;       // ACE_ADAPT_RETVAL used it for intermediate status

  return result;
#   elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::cond_init (cv,
                                                    attributes.type,
                                                    arg),
                                       result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS vs. ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::cond_init (ACE_cond_t *cv,
                   ACE_condattr_t &attributes,
                   const wchar_t *name,
                   void *arg)
{
  return OS::cond_init (cv, attributes, ACE_Wide_To_Ascii (name).char_rep (), arg);
}
#endif /* ACE_HAS_WCHAR */

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::cond_init (ACE_cond_t *cv, short type, const wchar_t *name, void *arg)
{
  return OS::cond_init (cv, type, ACE_Wide_To_Ascii (name).char_rep (), arg);
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
OS::cond_signal (ACE_cond_t *cv)
{
  OS_TRACE ("OS::cond_signal");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
#     if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
  OSCALL_RETURN (pthread_cond_signal (cv), int, -1);
#     else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_signal (cv), result),
                     int, -1);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
#   elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::cond_signal (cv), result), int, -1);
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::cond_wait (ACE_cond_t *cv,
                   ACE_mutex_t *external_mutex)
{
  OS_TRACE ("OS::cond_wait");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)
#     if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
  OSCALL_RETURN (pthread_cond_wait (cv, external_mutex), int, -1);
#     else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cond_wait (cv, external_mutex), result),
                     int, -1);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
#   elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::cond_wait (cv, external_mutex), result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (external_mutex);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::cond_timedwait (ACE_cond_t *cv,
                        ACE_mutex_t *external_mutex,
                        TimeValue *timeout)
{
  OS_TRACE ("OS::cond_timedwait");
# if defined (ACE_HAS_THREADS)
  int result;
  timespec_t ts;

  if (timeout != 0)
    ts = *timeout; // Calls TimeValue::operator timespec_t().

#   if defined (ACE_HAS_PTHREADS)

#     if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
  if (timeout == 0)
    OSCALL (pthread_cond_wait (cv, external_mutex),
                int, -1, result);
  else
    {
      OSCALL (pthread_cond_timedwait (cv, external_mutex,
                                            (ACE_TIMESPEC_PTR) &ts),
                  int, -1, result);
    }

#     else
  OSCALL (ACE_ADAPT_RETVAL (timeout == 0
                                ? pthread_cond_wait (cv, external_mutex)
                                : pthread_cond_timedwait (cv, external_mutex,
                                                            (ACE_TIMESPEC_PTR) &ts),
                                result),
              int, -1, result);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6*/
  // We need to adjust this to make the POSIX and Solaris return
  // values consistent.  EAGAIN is from Pthreads DRAFT4 (HP-UX 10.20 and down)
  if (result == -1 &&
      (errno == ETIMEDOUT || errno == EAGAIN))
    errno = ETIME;

#   elif defined (ACE_HAS_STHREADS)
  OSCALL (ACE_ADAPT_RETVAL (timeout == 0
                                ? ::cond_wait (cv, external_mutex)
                                : ::cond_timedwait (cv,
                                                    external_mutex,
                                                    (timestruc_t*)&ts),
                                result),
              int, -1, result);
#   endif /* ACE_HAS_STHREADS */
  if (timeout != 0)
    timeout->set (ts); // Update the time value before returning.

  return result;
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (external_mutex);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}
#endif /* !ACE_LACKS_COND_T */

ACE_INLINE int
OS::mutex_lock (ACE_mutex_t *m,
                    const TimeValue *timeout)
{
  return timeout == 0 ? OS::mutex_lock (m) : OS::mutex_lock (m, *timeout);
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::event_init (ACE_event_t *event,
                    int manual_reset,
                    int initial_state,
                    int type,
                    const wchar_t *name,
                    void *arg,
                    LPSECURITY_ATTRIBUTES sa)
{
#if defined (ACE_WIN32)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *event = ::CreateEventW (OS::default_win32_security_attributes_r
      (sa, &sa_buffer, &sd_buffer),
  manual_reset,
  initial_state,
  name);
  if (*event == 0)
    ACE_FAIL_RETURN (-1);

  return 0;
#else  /* ACE_WIN32 */
  return OS::event_init (event,
                             manual_reset,
                             initial_state,
                             type,
                             ACE_Wide_To_Ascii (name).char_rep (),
                             arg,
                             sa);
#endif /* ACE_WIN32 */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE long
OS::priority_control (ACE_idtype_t idtype, ACE_id_t identifier, int cmd, void *arg)
{
  OS_TRACE ("OS::priority_control");
#if defined (ACE_HAS_PRIOCNTL)
  OSCALL_RETURN (priocntl (idtype, identifier, cmd, static_cast<caddr_t> (arg)),
                     long, -1);
#else  /* ! ACE_HAS_PRIOCNTL*/
  ACE_UNUSED_ARG (idtype);
  ACE_UNUSED_ARG (identifier);
  ACE_UNUSED_ARG (cmd);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
#endif /* ! ACE_HAS_PRIOCNTL*/
}

// This method is used to prepare the recursive mutex for releasing
// when waiting on a condition variable. If the platform doesn't have
// native recursive mutex and condition variable support, then ACE needs
// to save the recursion state around the wait and also ensure that the
// wait and lock release are atomic. recursive_mutex_cond_relock()
// is the inverse of this method.
ACE_INLINE int
OS::recursive_mutex_cond_unlock (ACE_recursive_thread_mutex_t *m,
                                     ACE_recursive_mutex_state &state)
{
#if defined (ACE_HAS_THREADS)
  OS_TRACE ("OS::recursive_mutex_cond_unlock");
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
  // Windows need special handling since it has recursive mutexes, but
  // does not integrate them into a condition variable.
#    if defined (ACE_WIN32)
  // For Windows, the OS takes care of the mutex and its recursion. We just
  // need to release the lock one fewer times than this thread has acquired
  // it. Remember how many times, and reacquire it that many more times when
  // the condition is signaled.
  //
  // For WinCE, the situation is a bit trickier. CE doesn't have
  // RecursionCount, and LockCount has changed semantics over time.
  // In CE 3 (and maybe 4?) LockCount is not an indicator of recursion;
  // instead, see when it's unlocked by watching the OwnerThread, which will
  // change to something other than the current thread when it's been
  // unlocked "enough" times. Note that checking for 0 (unlocked) is not
  // sufficient. Another thread may acquire the lock between our unlock and
  // checking the OwnerThread. So grab our thread ID value first, then
  // compare to it in the loop condition. NOTE - the problem with this
  // scheme is that we really want to unlock the mutex one _less_ times than
  // required to release it for another thread to acquire. With CE 5 we
  // can do this by watching LockCount alone. I _think_ it can be done by
  // watching LockCount on CE 4 as well (though its meaning is different),
  // but I'm leary of changing this code since a user reported success
  // with it.
  //
  // We're using undocumented fields in the CRITICAL_SECTION structure
  // and they've been known to change across Windows variants and versions./
  // So be careful if you need to change these - there may be other
  // Windows variants that depend on existing values and limits.
#      if defined (ACE_HAS_WINCE) && (UNDER_CE < 500)
  ACE_thread_t me = OS::thr_self ();
#      endif /* ACE_HAS_WINCE && CE 4 or earlier */

  state.relock_count_ = 0;
  while (
#      if !defined (ACE_HAS_WINCE)
         m->LockCount > 0 && m->RecursionCount > 1
#      else
         // WinCE doesn't have RecursionCount and the LockCount semantic
         // has changed between versions; pre-Mobile 5 the LockCount
         // was 0-indexed, and Mobile 5 has it 1-indexed.
#        if (UNDER_CE < 500)
         m->LockCount > 0 && m->OwnerThread == (HANDLE)me
#        else
         m->LockCount > 1
#        endif /* UNDER_CE < 500 */
#      endif /* ACE_HAS_WINCE */
         )
    {
      // This may fail if the current thread doesn't own the mutex. If it
      // does fail, it'll be on the first try, so don't worry about resetting
      // the state.
      if (OS::recursive_mutex_unlock (m) == -1)
        return -1;
      ++state.relock_count_;
    }
#    else /* not ACE_WIN32 */
    // prevent warnings for unused variables
    ACE_UNUSED_ARG (state);
    ACE_UNUSED_ARG (m);
#    endif /* ACE_WIN32 */
  return 0;
#  else /* ACE_HAS_RECURSIVE_MUTEXES */
  // For platforms without recursive mutexes, we obtain the nesting mutex
  // to gain control over the mutex internals. Then set the internals to say
  // the mutex is available. If there are waiters, signal the condition
  // to notify them (this is mostly like the recursive_mutex_unlock() method).
  // Then, return with the nesting mutex still held. The condition wait
  // will release it atomically, allowing mutex waiters to continue.
  // Note that this arrangement relies on the fact that on return from
  // the condition wait, this thread will again own the nesting mutex
  // and can either set the mutex internals directly or get in line for
  // the mutex... this part is handled in recursive_mutex_cond_relock().
  if (OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    return -1;

#    if !defined (ACE_NDEBUG)
  if (m->nesting_level_ == 0
      || OS::thr_equal (OS::thr_self (), m->owner_id_) == 0)
    {
      OS::thread_mutex_unlock (&m->nesting_mutex_);
      errno = EINVAL;
      return -1;
    }
#    endif /* ACE_NDEBUG */

  // To make error recovery a bit easier, signal the condition now. Any
  // waiter won't regain control until the mutex is released, which won't
  // be until the caller returns and does the wait on the condition.
  if (OS::cond_signal (&m->lock_available_) == -1)
    {
      // Save/restore errno.
      ACE_Errno_Guard error (errno);
      OS::thread_mutex_unlock (&m->nesting_mutex_);
      return -1;
    }

  // Ok, the nesting_mutex_ lock is still held, the condition has been
  // signaled... reset the nesting info and return _WITH_ the lock
  // held. The lock will be released when the condition waits, in the
  // caller.
  state.nesting_level_ = m->nesting_level_;
  state.owner_id_ = m->owner_id_;
  m->nesting_level_ = 0;
  m->owner_id_ = OS::NULL_thread;
  return 0;
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (state);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}


// This method is called after waiting on a condition variable when a
// recursive mutex must be reacquired. If the platform doesn't natively
// integrate recursive mutexes and condition variables, it's taken care
// of here (inverse of OS::recursive_mutex_cond_unlock).
ACE_INLINE void
OS::recursive_mutex_cond_relock (ACE_recursive_thread_mutex_t *m,
                                     ACE_recursive_mutex_state &state)
{
#if defined (ACE_HAS_THREADS)
  OS_TRACE ("OS::recursive_mutex_cond_relock");
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
  // Windows need special handling since it has recursive mutexes, but
  // does not integrate them into a condition variable.
  // On entry, the OS has already reacquired the lock for us. Just
  // reacquire it the proper number of times so the recursion is the same as
  // before waiting on the condition.
#    if defined (ACE_WIN32)
  while (state.relock_count_ > 0)
    {
      OS::recursive_mutex_lock (m);
      --state.relock_count_;
    }
  return;
#    else /* not ACE_WIN32 */
    // prevent warnings for unused variables
    ACE_UNUSED_ARG (state);
    ACE_UNUSED_ARG (m);

#    endif /* ACE_WIN32 */
#  else
  // Without recursive mutex support, it's somewhat trickier. On entry,
  // the current thread holds the nesting_mutex_, but another thread may
  // still be holding the ACE_recursive_mutex_t. If so, mimic the code
  // in OS::recursive_mutex_lock that waits to acquire the mutex.
  // After acquiring it, restore the nesting counts and release the
  // nesting mutex. This will restore the conditions to what they were
  // before calling OS::recursive_mutex_cond_unlock().
  while (m->nesting_level_ > 0)
    OS::cond_wait (&m->lock_available_, &m->nesting_mutex_);

  // At this point, we still have nesting_mutex_ and the mutex is free.
  m->nesting_level_ = state.nesting_level_;
  m->owner_id_ = state.owner_id_;
  OS::thread_mutex_unlock (&m->nesting_mutex_);
  return;
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (state);
  return;
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::recursive_mutex_destroy (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return OS::thread_mutex_destroy (m);
#else
  if (OS::thread_mutex_destroy (&m->nesting_mutex_) == -1)
    return -1;
  else if (OS::cond_destroy (&m->lock_available_) == -1)
    return -1;
  else
    return 0;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::recursive_mutex_init (ACE_recursive_thread_mutex_t *m,
                              const ACE_TCHAR *name,
                              ACE_mutexattr_t *arg,
                              LPSECURITY_ATTRIBUTES sa)
{
  ACE_UNUSED_ARG (sa);
#if defined (ACE_HAS_THREADS)
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
#    if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  return OS::thread_mutex_init (m, PTHREAD_MUTEX_RECURSIVE, name, arg);
#    else
  return OS::thread_mutex_init (m, 0, name, arg);
#    endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
#  else
  if (OS::thread_mutex_init (&m->nesting_mutex_, 0, name, arg) == -1)
    return -1;
  else if (OS::cond_init (&m->lock_available_,
                              (short) USYNC_THREAD,
                              name,
                              0) == -1)
    return -1;
  else
    {
      m->nesting_level_ = 0;
      m->owner_id_ = OS::NULL_thread;
      return 0;
    }
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::recursive_mutex_lock (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return OS::thread_mutex_lock (m);
#else
  ACE_thread_t t_id = OS::thr_self ();
  int result = 0;

  // Acquire the guard.
  if (OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    result = -1;
  else
  {
    // If there's no contention, just grab the lock immediately
    // (since this is the common case we'll optimize for it).
    if (m->nesting_level_ == 0)
      m->owner_id_ = t_id;
      // If we already own the lock, then increment the nesting level
      // and return.
    else if (OS::thr_equal (t_id, m->owner_id_) == 0)
    {
          // Wait until the nesting level has dropped to zero, at
          // which point we can acquire the lock.
      while (m->nesting_level_ > 0)
        OS::cond_wait (&m->lock_available_,
                            &m->nesting_mutex_);

          // At this point the nesting_mutex_ is held...
      m->owner_id_ = t_id;
    }

    // At this point, we can safely increment the nesting_level_ no
    // matter how we got here!
    ++m->nesting_level_;
  }

  {
    // Save/restore errno.
    ACE_Errno_Guard error (errno);
    OS::thread_mutex_unlock (&m->nesting_mutex_);
  }
  return result;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::recursive_mutex_lock (ACE_recursive_thread_mutex_t *m,
                              const TimeValue &timeout)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return OS::thread_mutex_lock (m, timeout);
#else
  ACE_thread_t t_id = OS::thr_self ();
  int result = 0;

  // Try to acquire the guard.
  if (OS::thread_mutex_lock (&m->nesting_mutex_, timeout) == -1)
    result = -1;
  else
    {
      // If there's no contention, just grab the lock immediately
      // (since this is the common case we'll optimize for it).
      if (m->nesting_level_ == 0)
        m->owner_id_ = t_id;
      // If we already own the lock, then increment the nesting level
      // and return.
      else if (OS::thr_equal (t_id, m->owner_id_) == 0)
        {
          // Wait until the nesting level has dropped to zero, at
          // which point we can acquire the lock.
          while (m->nesting_level_ > 0)
            {
              result = OS::cond_timedwait (&m->lock_available_,
                                               &m->nesting_mutex_,
                                               const_cast <TimeValue *> (&timeout));

              // The mutex is reacquired even in the case of a timeout
              // release the mutex to prevent a deadlock
              if (result == -1)
                {
                  // Save/restore errno.
                  ACE_Errno_Guard error (errno);
                  OS::thread_mutex_unlock (&m->nesting_mutex_);

                  return result;
                }
            }

          // At this point the nesting_mutex_ is held...
          m->owner_id_ = t_id;
        }

      // At this point, we can safely increment the nesting_level_ no
      // matter how we got here!
      m->nesting_level_++;

      // Save/restore errno.
      ACE_Errno_Guard error (errno);
      OS::thread_mutex_unlock (&m->nesting_mutex_);
    }
  return result;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::recursive_mutex_lock (ACE_recursive_thread_mutex_t *m,
                              const TimeValue *timeout)
{
  return timeout == 0
    ? OS::recursive_mutex_lock (m)
    : OS::recursive_mutex_lock (m, *timeout);
}

ACE_INLINE int
OS::recursive_mutex_trylock (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return OS::thread_mutex_trylock (m);
#else
  ACE_thread_t t_id = OS::thr_self ();
  int result = 0;

  // Acquire the guard.
  if (OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    result = -1;
  else
  {
      // If there's no contention, just grab the lock immediately.
    if (m->nesting_level_ == 0)
    {
      m->owner_id_ = t_id;
      m->nesting_level_ = 1;
    }
      // If we already own the lock, then increment the nesting level
      // and proceed.
    else if (OS::thr_equal (t_id, m->owner_id_))
      m->nesting_level_++;
    else
    {
      errno = EBUSY;
      result = -1;
    }
  }

  {
    // Save/restore errno.
    ACE_Errno_Guard error (errno);
    OS::thread_mutex_unlock (&m->nesting_mutex_);
  }
  return result;
#endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::recursive_mutex_unlock (ACE_recursive_thread_mutex_t *m)
{
#if defined (ACE_HAS_THREADS)
#  if defined (ACE_HAS_RECURSIVE_MUTEXES)
  return OS::thread_mutex_unlock (m);
#  else
  OS_TRACE ("OS::recursive_mutex_unlock");
#    if !defined (ACE_NDEBUG)
  ACE_thread_t t_id = OS::thr_self ();
#    endif /* ACE_NDEBUG */
  int result = 0;

  if (OS::thread_mutex_lock (&m->nesting_mutex_) == -1)
    result = -1;
  else
  {
#    if !defined (ACE_NDEBUG)
      if (m->nesting_level_ == 0
          || OS::thr_equal (t_id, m->owner_id_) == 0)
{
  errno = EINVAL;
  result = -1;
}
      else
#    endif /* ACE_NDEBUG */
{
  m->nesting_level_--;
  if (m->nesting_level_ == 0)
  {
              // This may not be strictly necessary, but it does put
              // the mutex into a known state...
    m->owner_id_ = OS::NULL_thread;

              // Inform a waiter that the lock is free.
    if (OS::cond_signal (&m->lock_available_) == -1)
      result = -1;
  }
}
  }

{
    // Save/restore errno.
  ACE_Errno_Guard error (errno);
  OS::thread_mutex_unlock (&m->nesting_mutex_);
}
  return result;
#  endif /* ACE_HAS_RECURSIVE_MUTEXES */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::rw_rdlock (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rw_rdlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T) || defined (ACE_HAS_PTHREADS_UNIX98_EXT)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_rdlock (rw),
                                       result),
                     int, -1);
#  else /* Solaris */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_rdlock (rw), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_PUSH (&rw->lock_);
#   endif /* ACE_HAS_PTHREADS */
  int result = 0;
  if (OS::mutex_lock (&rw->lock_) == -1)
    result = -1; // -1 means didn't get the mutex.
  else
    {
      // Give preference to writers who are waiting.
      while (rw->ref_count_ < 0 || rw->num_waiting_writers_ > 0)
        {
          rw->num_waiting_readers_++;
          if (OS::cond_wait (&rw->waiting_readers_, &rw->lock_) == -1)
            {
              result = -2; // -2 means that we need to release the mutex.
              break;
            }
          rw->num_waiting_readers_--;
        }
    }
  if (result == 0)
    rw->ref_count_++;
  if (result != -1)
    OS::mutex_unlock (&rw->lock_);
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_POP (0);
#   endif /* defined (ACE_HAS_PTHREADS) */
  return 0;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::rw_tryrdlock (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rw_tryrdlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T) || defined (ACE_HAS_PTHREADS_UNIX98_EXT)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_tryrdlock (rw),
                                       result),
                     int, -1);
#  else /* Solaris */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_tryrdlock (rw), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  int result = -1;

  if (OS::mutex_lock (&rw->lock_) != -1)
    {
      ACE_Errno_Guard error (errno);

      if (rw->ref_count_ == -1 || rw->num_waiting_writers_ > 0)
        {
          error = EBUSY;
          result = -1;
        }
      else
        {
          rw->ref_count_++;
          result = 0;
        }

      OS::mutex_unlock (&rw->lock_);
    }
  return result;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::rw_trywrlock (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rw_trywrlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T) || defined (ACE_HAS_PTHREADS_UNIX98_EXT)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_trywrlock (rw),
                                       result),
                     int, -1);
#  else /* Solaris */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_trywrlock (rw), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  int result = -1;

  if (OS::mutex_lock (&rw->lock_) != -1)
    {
      ACE_Errno_Guard error (errno);

      if (rw->ref_count_ != 0)
        {
          error = EBUSY;
          result = -1;
        }
      else
        {
          rw->ref_count_ = -1;
          result = 0;
        }

      OS::mutex_unlock (&rw->lock_);
    }
  return result;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

// Note that the caller of this method *must* already possess this
// lock as a read lock.
// return {-1 and no errno set means: error,
//         -1 and errno==EBUSY set means: could not upgrade,
//         0 means: upgraded successfully}

ACE_INLINE int
OS::rw_trywrlock_upgrade (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rw_trywrlock_upgrade");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  // This will probably result in -1, EDEADLK, at least on HP-UX, but let it
  // go - it's a more descriptive error than ENOTSUP.
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_trywrlock (rw),
                                       result),
                     int, -1);
# elif !defined (ACE_LACKS_RWLOCK_T)
  // Some native rwlocks, such as those on Solaris, don't
  // support the upgrade feature . . .
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
# else /* NT, POSIX, and VxWorks don't support this natively. */
  // The ACE rwlock emulation does support upgrade . . .
  int result = 0;

#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_PUSH (&rw->lock_);
#   endif /* defined (ACE_HAS_PTHREADS) */

  if (OS::mutex_lock (&rw->lock_) == -1)
    return -1;
    // -1 means didn't get the mutex, error
  else if (rw->important_writer_)
    // an other reader upgrades already
    {
      result = -1;
      errno = EBUSY;
    }
  else
    {
      while (rw->ref_count_ > 1) // wait until only I am left
        {
          rw->num_waiting_writers_++; // prohibit any more readers
          rw->important_writer_ = 1;

          if (OS::cond_wait (&rw->waiting_important_writer_, &rw->lock_) == -1)
            {
              result = -1;
              // we know that we have the lock again, we have this guarantee,
              // but something went wrong
            }
          rw->important_writer_ = 0;
          rw->num_waiting_writers_--;
        }
      if (result == 0)
        {
          // nothing bad happend
          rw->ref_count_ = -1;
          // now I am a writer
          // everything is O.K.
        }
    }

  OS::mutex_unlock (&rw->lock_);

#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_POP (0);
#   endif /* defined (ACE_HAS_PTHREADS) */

  return result;

# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::rw_unlock (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rw_unlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T) || defined (ACE_HAS_PTHREADS_UNIX98_EXT)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_unlock (rw),
                                       result),
                     int, -1);
#  else /* Solaris */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_unlock (rw), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  if (OS::mutex_lock (&rw->lock_) == -1)
    return -1;

  if (rw->ref_count_ > 0) // Releasing a reader.
    rw->ref_count_--;
  else if (rw->ref_count_ == -1) // Releasing a writer.
    rw->ref_count_ = 0;
  else
    {
      (void) OS::mutex_unlock (&rw->lock_);
      return -1; // @@ ACE_ASSERT (!"count should not be 0!\n");
    }

  int result = 0;
  ACE_Errno_Guard error (errno);

  if (rw->important_writer_ && rw->ref_count_ == 1)
    // only the reader requesting to upgrade its lock is left over.
    {
      result = OS::cond_signal (&rw->waiting_important_writer_);
      error = errno;
    }
  else if (rw->num_waiting_writers_ > 0 && rw->ref_count_ == 0)
    // give preference to writers over readers...
    {
      result = OS::cond_signal (&rw->waiting_writers_);
      error =  errno;
    }
  else if (rw->num_waiting_readers_ > 0 && rw->num_waiting_writers_ == 0)
    {
      result = OS::cond_broadcast (&rw->waiting_readers_);
      error = errno;
    }

  (void) OS::mutex_unlock (&rw->lock_);
  return result;
# endif /* ! ace_lacks_rwlock_t */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ace_has_threads */
}

ACE_INLINE int
OS::rw_wrlock (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rw_wrlock");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T) || defined (ACE_HAS_PTHREADS_UNIX98_EXT)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_wrlock (rw),
                                       result),
                     int, -1);
#  else /* Solaris */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rw_wrlock (rw), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_PUSH (&rw->lock_);
#   endif /* defined (ACE_HAS_PTHREADS) */
  int result = 0;

  if (OS::mutex_lock (&rw->lock_) == -1)
    result = -1; // -1 means didn't get the mutex.
  else
    {
      while (rw->ref_count_ != 0)
        {
          rw->num_waiting_writers_++;

          if (OS::cond_wait (&rw->waiting_writers_, &rw->lock_) == -1)
            {
              result = -2; // -2 means we need to release the mutex.
              break;
            }

          rw->num_waiting_writers_--;
        }
    }
  if (result == 0)
    rw->ref_count_ = -1;
  if (result != -1)
    OS::mutex_unlock (&rw->lock_);
#   if defined (ACE_HAS_PTHREADS)
  ACE_PTHREAD_CLEANUP_POP (0);
#   endif /* defined (ACE_HAS_PTHREADS) */
  return 0;
# endif /* ! ACE_LACKS_RWLOCK_T */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::rwlock_destroy (ACE_rwlock_t *rw)
{
  OS_TRACE ("OS::rwlock_destroy");
#if defined (ACE_HAS_THREADS)
# if !defined (ACE_LACKS_RWLOCK_T) || defined (ACE_HAS_PTHREADS_UNIX98_EXT)
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_rwlock_destroy (rw),
                                       result),
                     int, -1);
#  else /* Solaris */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rwlock_destroy (rw), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
# else /* NT, POSIX, and VxWorks don't support this natively. */
  OS::mutex_destroy (&rw->lock_);
  OS::cond_destroy (&rw->waiting_readers_);
  OS::cond_destroy (&rw->waiting_important_writer_);
  return OS::cond_destroy (&rw->waiting_writers_);
# endif /* ACE_HAS_STHREADS && !defined (ACE_LACKS_RWLOCK_T) */
#else
  ACE_UNUSED_ARG (rw);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_THREADS) && (!defined (ACE_LACKS_RWLOCK_T) || \
                                   defined (ACE_HAS_PTHREADS_UNIX98_EXT))
ACE_INLINE int
OS::rwlock_init (ACE_rwlock_t *rw,
                     int type,
                     const ACE_TCHAR *name,
                     void *arg)
{
  // OS_TRACE ("OS::rwlock_init");
#  if defined (ACE_HAS_PTHREADS_UNIX98_EXT)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);

  int status;
  pthread_rwlockattr_t attr;
  pthread_rwlockattr_init (&attr);
#    if !defined (ACE_LACKS_RWLOCKATTR_PSHARED)
  pthread_rwlockattr_setpshared (&attr, (type == USYNC_THREAD ?
                                         PTHREAD_PROCESS_PRIVATE :
                                         PTHREAD_PROCESS_SHARED));
#    else
  ACE_UNUSED_ARG (type);
#    endif /* !ACE_LACKS_RWLOCKATTR_PSHARED */
  status = ACE_ADAPT_RETVAL (pthread_rwlock_init (rw, &attr), status);
  pthread_rwlockattr_destroy (&attr);

  return status;

#  else
  type = type;
  name = name;
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::rwlock_init (rw, type, arg), result), int, -1);
#  endif /* ACE_HAS_PTHREADS_UNIX98_EXT */
}
#endif /* ACE_HAS THREADS && !defined (ACE_LACKS_RWLOCK_T) */

ACE_INLINE int
OS::sema_destroy (ACE_sema_t *s)
{
  OS_TRACE ("OS::sema_destroy");
# if defined (ACE_HAS_POSIX_SEM)
  int result;
#   if !defined (ACE_HAS_POSIX_SEM_TIMEOUT) && !defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  OS::mutex_destroy (&s->lock_);
  OS::cond_destroy (&s->count_nonzero_);
#   endif /* !ACE_HAS_POSIX_SEM_TIMEOUT && !ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION */
#   if defined (ACE_LACKS_NAMED_POSIX_SEM)
  if (s->name_)
    {
      // Only destroy the semaphore if we're the ones who
      // initialized it.
      OSCALL (::sem_destroy (s->sema_),int, -1, result);
      OS::shm_unlink (s->name_);
      delete s->name_;
      return result;
    }
#   else
  if (s->name_)
    {
      OSCALL (::sem_unlink (s->name_), int, -1, result);
      OS::free ((void *) s->name_);
      OSCALL_RETURN (::sem_close (s->sema_), int, -1);
    }
#   endif /*  ACE_LACKS_NAMED_POSIX_SEM */
  else
    {
      OSCALL (::sem_destroy (s->sema_), int, -1, result);
#   if defined (ACE_LACKS_NAMED_POSIX_SEM)
      if (s->new_sema_ != 0)
#   endif /* ACE_LACKS_NAMED_POSIX_SEM */
        delete s->sema_;
      s->sema_ = 0;
      return result;
    }
# elif defined (ACE_USES_FIFO_SEM)
  int r0 = 0;
  if (s->name_)
    {
      r0 = OS::unlink (s->name_);
      OS::free (s->name_);
      s->name_ = 0;
    }
  int r1 = OS::close (s->fd_[0]);      /* ignore error */
  int r2 = OS::close (s->fd_[1]);      /* ignore error */
  return r0 != 0 || r1 != 0 || r2 != 0 ? -1 : 0;
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::sema_destroy (s), result), int, -1);
#   elif defined (ACE_HAS_PTHREADS)
  int r1 = OS::mutex_destroy (&s->lock_);
  int r2 = OS::cond_destroy (&s->count_nonzero_);
  return r1 != 0 || r2 != 0 ? -1 : 0;
#   elif defined (ACE_HAS_WTHREADS)
#     if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CloseHandle (*s), ace_result_), int, -1);
#     else /* ACE_USES_WINCE_SEMA_SIMULATION */
  // Free up underlying objects of the simulated semaphore.
  int r1 = OS::thread_mutex_destroy (&s->lock_);
  int r2 = OS::event_destroy (&s->count_nonzero_);
  return r1 != 0 || r2 != 0 ? -1 : 0;
#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#   elif defined (ACE_VXWORKS)
  int result;
  OSCALL (::semDelete (s->sema_), int, -1, result);
  s->sema_ = 0;
  return result;
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

// NOTE: The previous four function definitions must appear before
// OS::sema_init ().

ACE_INLINE int
OS::sema_init (ACE_sema_t *s,
                   u_int count,
                   int type,
                   const char *name,
                   void *arg,
                   int max,
                   LPSECURITY_ATTRIBUTES sa)
{
  OS_TRACE ("OS::sema_init");
#if defined (ACE_HAS_POSIX_SEM)
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);

  s->name_ = 0;
#  if defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  ACE_UNUSED_ARG (arg);
#  else
  int result = -1;

  if (OS::mutex_init (&s->lock_, type, name,
                          (ACE_mutexattr_t *) arg) == 0
      && OS::cond_init (&s->count_nonzero_, (short)type, name, arg) == 0
      && OS::mutex_lock (&s->lock_) == 0)
    {
      if (OS::mutex_unlock (&s->lock_) == 0)
        result = 0;
    }

  if (result == -1)
    {
      OS::mutex_destroy (&s->lock_);
      OS::cond_destroy (&s->count_nonzero_);
      return result;
    }
#  endif /* ACE_HAS_POSIX_SEM_TIMEOUT || ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION */

#  if defined (ACE_LACKS_NAMED_POSIX_SEM)
  s->new_sema_ = 0;
  if (type == USYNC_PROCESS)
    {
      // Let's see if it already exists.
      ACE_HANDLE fd = OS::shm_open (ACE_TEXT_CHAR_TO_TCHAR (name),
                                        O_RDWR | O_CREAT | O_EXCL,
                                        ACE_DEFAULT_FILE_PERMS);
      if (fd == ACE_INVALID_HANDLE)
        {
          if (errno == EEXIST)
            fd = OS::shm_open (ACE_TEXT_CHAR_TO_TCHAR (name),
                                   O_RDWR | O_CREAT,
                                   ACE_DEFAULT_FILE_PERMS);
          else
            return -1;
        }
      else
        {
          // We own this shared memory object!  Let's set its
          // size.
          if (OS::ftruncate (fd,
                                 sizeof (ACE_sema_t)) == -1)
            return -1;
          s->name_ = OS::strdup (name);
          if (s->name_ == 0)
            return -1;
        }
      if (fd == -1)
        return -1;

      s->sema_ = (sem_t *)
        OS::mmap (0,
                      sizeof (ACE_sema_t),
                      PROT_RDWR,
                      MAP_SHARED,
                      fd,
                      0);
      OS::close (fd);
      if (s->sema_ == (sem_t *) MAP_FAILED)
        return -1;
      if (s->name_
          // @@ According UNIX Network Programming V2 by Stevens,
          //    sem_init() is currently not required to return zero on
          //    success, but it *does* return -1 upon failure.  For
          //    this reason, check for failure by comparing to -1,
          //    instead of checking for success by comparing to zero.
          //        -Ossama
          // Only initialize it if we're the one who created it.
          && ::sem_init (s->sema_, type == USYNC_PROCESS, count) == -1)
        return -1;
      return 0;
    }
#  else
  if (name)
    {
#    if defined (sun) || defined (HPUX)
      // Solaris and HP-UX require the name to start with a slash. Solaris
      // further requires that there be no other slashes than the first.
      const char *last_slash = OS::strrchr (name, '/');
      char name2[MAXPATHLEN];
      if (0 == last_slash)
        {
          OS::strcpy (name2, "/");
          OS::strcat (name2, name);
          name = name2;
        }
#      if defined (sun)
      else
        name = last_slash;         // Chop off chars preceding last slash
#      endif /* sun */
#    endif /* sun || HPUX */

      ACE_ALLOCATOR_RETURN (s->name_,
                            OS::strdup (name),
                            -1);
      s->sema_ = ::sem_open (s->name_,
                             O_CREAT,
                             ACE_DEFAULT_FILE_PERMS,
                             count);
      if (s->sema_ == (sem_t *) SEM_FAILED)
        return -1;
      else
        return 0;
    }
#  endif /* ACE_LACKS_NAMED_POSIX_SEM */
  else
    {
      ACE_NEW_RETURN (s->sema_,
                      sem_t,
                      -1);
#  if defined (ACE_LACKS_NAMED_POSIX_SEM)
      s->new_sema_ = 1;
#  endif /* ACE_LACKS_NAMED_POSIX_SEM */
      OSCALL_RETURN (::sem_init (s->sema_,
                                     type != USYNC_THREAD,
                                     count), int, -1);
    }

#elif defined (ACE_USES_FIFO_SEM)
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  int             flags = 0;
  mode_t          mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP;

  if (type == USYNC_THREAD)
    {
      // Create systemwide unique name for semaphore
      char   uname[ACE_UNIQUE_NAME_LEN];
      OS::unique_name ((const void *) s,
                            uname,
                            ACE_UNIQUE_NAME_LEN);
      name = &uname[0];
    }

  s->name_ = 0;
  s->fd_[0] = s->fd_[1] = ACE_INVALID_HANDLE;
  bool creator = false;

  if (OS::mkfifo (ACE_TEXT_CHAR_TO_TCHAR (name), mode) < 0)
    {
      if (errno != EEXIST)    /* already exists OK else ERR */
        return -1;
      // check if this is a real FIFO, not just some other existing file
      ACE_stat fs;
      if (OS::stat (name, &fs))
        return -1;
      if (!S_ISFIFO (fs.st_mode))
        {
          // existing file is not a FIFO
          errno = EEXIST;
          return -1;
        }
    }
    else
      creator = true; // remember we created it for initialization at end

  // for processshared semaphores remember who we are to be able to remove
  // the FIFO when we're done with it
  if (type == USYNC_PROCESS)
    {
      s->name_ = OS::strdup (name);
      if (s->name_ == 0)
        {
          if (creator)
            OS::unlink (name);
          return -1;
        }
    }

  if ((s->fd_[0] = OS::open (name, O_RDONLY | O_NONBLOCK)) == ACE_INVALID_HANDLE
      || (s->fd_[1] = OS::open (name, O_WRONLY | O_NONBLOCK)) == ACE_INVALID_HANDLE)
    return (-1);

  /* turn off nonblocking for fd_[0] */
  if ((flags = OS::fcntl (s->fd_[0], F_GETFL, 0)) < 0)
    return (-1);

  flags &= ~O_NONBLOCK;
  if (OS::fcntl (s->fd_[0], F_SETFL, flags) < 0)
    return (-1);

  //if (s->name_ && count)
  if (creator && count)
    {
      char    c = 1;
      for (u_int i=0; i<count ;++i)
        if (OS::write (s->fd_[1], &c, sizeof (char)) != 1)
          return (-1);
    }

  // In the case of process scope semaphores we can already unlink the FIFO now that
  // we completely set it up (the opened handles will keep it active until we close
  // thos down). This way we're protected against unexpected crashes as far as removal
  // is concerned.
  // Unfortunately this does not work for processshared FIFOs since as soon as we
  // have unlinked the semaphore no other process will be able to open it anymore.
  if (type == USYNC_THREAD)
    {
      OS::unlink (name);
    }

  return (0);
#elif defined (ACE_HAS_THREADS)
#  if defined (ACE_HAS_STHREADS)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::sema_init (s, count, type, arg), result),
                     int, -1);
#  elif defined (ACE_HAS_PTHREADS)
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  int result = -1;

  if (OS::mutex_init (&s->lock_, type, name,
                          (ACE_mutexattr_t *) arg) == 0
      && OS::cond_init (&s->count_nonzero_, type, name, arg) == 0
      && OS::mutex_lock (&s->lock_) == 0)
    {
      s->count_ = count;
      s->waiters_ = 0;

      if (OS::mutex_unlock (&s->lock_) == 0)
        result = 0;
    }

  if (result == -1)
    {
      OS::mutex_destroy (&s->lock_);
      OS::cond_destroy (&s->count_nonzero_);
    }
  return result;
#  elif defined (ACE_HAS_WTHREADS)
#    if ! defined (ACE_USES_WINCE_SEMA_SIMULATION)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  // Create the semaphore with its value initialized to <count> and
  // its maximum value initialized to <max>.
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *s = ::CreateSemaphoreA
    (OS::default_win32_security_attributes_r (sa, &sa_buffer, &sd_buffer),
     count,
     max,
     name);

  if (*s == 0)
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
  else
    return 0;
#    else /* ACE_USES_WINCE_SEMA_SIMULATION */
  int result = -1;

  // Initialize internal object for semaphore simulation.
  // Grab the lock as soon as possible when we initializing
  // the semaphore count.  Notice that we initialize the
  // event object as "manually reset" so we can amortize the
  // cost for singling/reseting the event.
  // @@ I changed the mutex type to thread_mutex.  Notice that this
  // is basically a CriticalSection object and doesn't not has
  // any security attribute whatsoever.  However, since this
  // semaphore implementation only works within a process, there
  // shouldn't any security issue at all.
  if (OS::thread_mutex_init (&s->lock_, type, name, (ACE_mutexattr_t *)arg) == 0
      && OS::event_init (&s->count_nonzero_, 1,
                             count > 0, type, name, arg, sa) == 0
      && OS::thread_mutex_lock (&s->lock_) == 0)
    {
      s->count_ = count;

      if (OS::thread_mutex_unlock (&s->lock_) == 0)
        result = 0;
    }

  // Destroy the internal objects if we didn't initialize
  // either of them successfully.  Don't bother to check
  // for errors.
  if (result == -1)
    {
      OS::thread_mutex_destroy (&s->lock_);
      OS::event_destroy (&s->count_nonzero_);
    }
  return result;
#    endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#  elif defined (ACE_VXWORKS)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  s->name_ = 0;
  s->sema_ = ::semCCreate (type, count);
  return s->sema_ ? 0 : -1;
#  endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (count);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (max);
  ACE_UNUSED_ARG (sa);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_POSIX_SEM */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::sema_init (ACE_sema_t *s,
                   u_int count,
                   int type,
                   const wchar_t *name,
                   void *arg,
                   int max,
                   LPSECURITY_ATTRIBUTES sa)
{
# if defined (ACE_HAS_WTHREADS)
#   if ! defined (ACE_USES_WINCE_SEMA_SIMULATION)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  // Create the semaphore with its value initialized to <count> and
  // its maximum value initialized to <max>.
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  *s = ::CreateSemaphoreW
    (OS::default_win32_security_attributes_r (sa, &sa_buffer, &sd_buffer),
     count,
     max,
     name);

  if (*s == 0)
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
  else
    return 0;
#   else /* ACE_USES_WINCE_SEMA_SIMULATION */
  int result = -1;

  // Initialize internal object for semaphore simulation.
  // Grab the lock as soon as possible when we initializing
  // the semaphore count.  Notice that we initialize the
  // event object as "manually reset" so we can amortize the
  // cost for singling/reseting the event.
  // @@ I changed the mutex type to thread_mutex.  Notice that this
  // is basically a CriticalSection object and doesn't not has
  // any security attribute whatsoever.  However, since this
  // semaphore implementation only works within a process, there
  // shouldn't any security issue at all.
  if (OS::thread_mutex_init (&s->lock_, type, name, (ACE_mutexattr_t *)arg) == 0
      && OS::event_init (&s->count_nonzero_, 1,
                             count > 0, type, name, arg, sa) == 0
      && OS::thread_mutex_lock (&s->lock_) == 0)
    {
      s->count_ = count;

      if (OS::thread_mutex_unlock (&s->lock_) == 0)
        result = 0;
    }

  // Destroy the internal objects if we didn't initialize
  // either of them successfully.  Don't bother to check
  // for errors.
  if (result == -1)
    {
      OS::thread_mutex_destroy (&s->lock_);
      OS::event_destroy (&s->count_nonzero_);
    }
  return result;
#   endif /* ACE_USES_WINCE_SEMA_SIMULATION */
# else /* ACE_HAS_WTHREADS */
  // Just call the normal char version.
  return OS::sema_init (s, count, type, ACE_Wide_To_Ascii (name).char_rep (), arg, max, sa);
# endif /* ACE_HAS_WTHREADS */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
OS::sema_post (ACE_sema_t *s)
{
  OS_TRACE ("OS::sema_post");
# if defined (ACE_HAS_POSIX_SEM)
#   if defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  OSCALL_RETURN (::sem_post (s->sema_), int, -1);
#   else
  int result = -1;

  if (OS::mutex_lock (&s->lock_) == 0)
    {
      if (::sem_post (s->sema_) == 0)
        result = OS::cond_signal (&s->count_nonzero_);

      OS::mutex_unlock (&s->lock_);
    }
  return result;
#   endif /* ACE_HAS_POSIX_SEM_TIMEOUT || ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION */
# elif defined (ACE_USES_FIFO_SEM)
  char    c = 1;
  if (OS::write (s->fd_[1], &c, sizeof (char)) == sizeof (char))
    return (0);
  return (-1);
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::sema_post (s), result), int, -1);
#   elif defined (ACE_HAS_PTHREADS)
  int result = -1;

  if (OS::mutex_lock (&s->lock_) == 0)
    {
      // Always allow a waiter to continue if there is one.
      if (s->waiters_ > 0)
        result = OS::cond_signal (&s->count_nonzero_);
      else
        result = 0;

      s->count_++;
      OS::mutex_unlock (&s->lock_);
    }
  return result;
#   elif defined (ACE_HAS_WTHREADS)
#     if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::ReleaseSemaphore (*s, 1, 0),
                                          ace_result_),
                        int, -1);
#     else /* ACE_USES_WINCE_SEMA_SIMULATION */
  int result = -1;

  // Since we are simulating semaphores, we need to update semaphore
  // count manually.  Grab the lock to prevent race condition first.
  if (OS::thread_mutex_lock (&s->lock_) == 0)
    {
      // Check the original state of event object.  Single the event
      // object in transition from semaphore not available to
      // semaphore available.
      if (s->count_++ <= 0)
        result = OS::event_signal (&s->count_nonzero_);
      else
        result = 0;

      OS::thread_mutex_unlock (&s->lock_);
    }
  return result;
#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#   elif defined (ACE_VXWORKS)
  OSCALL_RETURN (::semGive (s->sema_), int, -1);
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
OS::sema_post (ACE_sema_t *s, u_int release_count)
{
#if defined (ACE_WIN32) && !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  // Win32 supports this natively.
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::ReleaseSemaphore (*s, release_count, 0),
                                          ace_result_), int, -1);
#else
  // On POSIX platforms we need to emulate this ourselves.
  // @@ We can optimize on this implementation.  However,
  // the semaphore promitive on Win32 doesn't allow one
  // to increase a semaphore to more than the count it was
  // first initialized.  Posix and solaris don't seem to have
  // this restriction.  Should we impose the restriction in
  // our semaphore simulation?
  for (size_t i = 0; i < release_count; i++)
    if (OS::sema_post (s) == -1)
      return -1;

  return 0;
#endif /* ACE_WIN32 */
}

ACE_INLINE int
OS::sema_trywait (ACE_sema_t *s)
{
  OS_TRACE ("OS::sema_trywait");
# if defined (ACE_HAS_POSIX_SEM)
  // POSIX semaphores set errno to EAGAIN if trywait fails
  OSCALL_RETURN (::sem_trywait (s->sema_), int, -1);
# elif defined (ACE_USES_FIFO_SEM)
  char  c;
  int     rc, flags;

  /* turn on nonblocking for s->fd_[0] */
  if ((flags = OS::fcntl (s->fd_[0], F_GETFL, 0)) < 0)
    return (-1);
  flags |= O_NONBLOCK;
  if (OS::fcntl (s->fd_[0], F_SETFL, flags) < 0)
    return (-1);

  // read sets errno to EAGAIN if no input
  rc = OS::read (s->fd_[0], &c, sizeof (char));

  /* turn off nonblocking for fd_[0] */
  if ((flags = OS::fcntl (s->fd_[0], F_GETFL, 0)) >= 0)
  {
    flags &= ~O_NONBLOCK;
    OS::fcntl (s->fd_[0], F_SETFL, flags);
  }

  return rc == 1 ? 0 : (-1);
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_STHREADS)
  // STHREADS semaphores set errno to EBUSY if trywait fails.
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::sema_trywait (s),
                                       result),
                     int, -1);
#   elif defined (ACE_HAS_PTHREADS)

  int result = -1;

  if (OS::mutex_lock (&s->lock_) == 0)
    {
      if (s->count_ > 0)
        {
          --s->count_;
          result = 0;
        }
      else
        errno = EBUSY;

      OS::mutex_unlock (&s->lock_);
    }
  return result;
#   elif defined (ACE_HAS_WTHREADS)
#     if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  DWORD result = ::WaitForSingleObject (*s, 0);

  if (result == WAIT_OBJECT_0)
    return 0;
  else
    {
      if (result == WAIT_TIMEOUT)
        errno = EBUSY;
      else
        OS::set_errno_to_last_error ();
      // This is a hack, we need to find an appropriate mapping...
      return -1;
    }
#     else /* ACE_USES_WINCE_SEMA_SIMULATION */
  // Check the status of semaphore first.  Return immediately
  // if the semaphore is not available and avoid grabing the
  // lock.
  DWORD result = ::WaitForSingleObject (s->count_nonzero_, 0);

  if (result == WAIT_OBJECT_0)  // Proceed when it is available.
    {
      OS::thread_mutex_lock (&s->lock_);

      // Need to double check if the semaphore is still available.
      // The double checking scheme will slightly affect the
      // efficiency if most of the time semaphores are not blocked.
      result = ::WaitForSingleObject (s->count_nonzero_, 0);
      if (result == WAIT_OBJECT_0)
        {
          // Adjust the semaphore count.  Only update the event
          // object status when the state changed.
          s->count_--;
          if (s->count_ <= 0)
            OS::event_reset (&s->count_nonzero_);
          result = 0;
        }

      OS::thread_mutex_unlock (&s->lock_);
    }

  // Translate error message to errno used by ACE.
  if (result == WAIT_TIMEOUT)
    errno = EBUSY;
  else
    OS::set_errno_to_last_error ();
  // This is taken from the hack above. ;)
  return -1;
#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#   elif defined (ACE_VXWORKS)
  if (::semTake (s->sema_, NO_WAIT) == ERROR)
    if (errno == S_objLib_OBJ_UNAVAILABLE)
      {
        // couldn't get the semaphore
        errno = EBUSY;
        return -1;
      }
    else
      // error
      return -1;
  else
    // got the semaphore
    return 0;
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
OS::sema_wait (ACE_sema_t *s)
{
  OS_TRACE ("OS::sema_wait");
# if defined (ACE_HAS_POSIX_SEM)
  OSCALL_RETURN (::sem_wait (s->sema_), int, -1);
# elif defined (ACE_USES_FIFO_SEM)
  char c;
  if (OS::read (s->fd_[0], &c, sizeof (char)) == 1)
    return (0);
  return (-1);
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::sema_wait (s), result), int, -1);
#   elif defined (ACE_HAS_PTHREADS)
  int result = 0;

  ACE_PTHREAD_CLEANUP_PUSH (&s->lock_);

  if (OS::mutex_lock (&s->lock_) != 0)
    result = -1;
  else
    {
      // Keep track of the number of waiters so that we can signal
      // them properly in <OS::sema_post>.
      s->waiters_++;

      // Wait until the semaphore count is > 0.
      while (s->count_ == 0)
        if (OS::cond_wait (&s->count_nonzero_,
                               &s->lock_) == -1)
          {
            result = -2; // -2 means that we need to release the mutex.
            break;
          }

      --s->waiters_;
    }

  if (result == 0)
    --s->count_;

  if (result != -1)
    OS::mutex_unlock (&s->lock_);
  ACE_PTHREAD_CLEANUP_POP (0);
  return result < 0 ? -1 : result;

#   elif defined (ACE_HAS_WTHREADS)
#     if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  switch (::WaitForSingleObject (*s, INFINITE))
    {
    case WAIT_OBJECT_0:
      return 0;
    default:
      // This is a hack, we need to find an appropriate mapping...
      OS::set_errno_to_last_error ();
      return -1;
    }
  /* NOTREACHED */
#     else /* ACE_USES_WINCE_SEMA_SIMULATION */
  // Timed wait.
  int result = -1;
  for (;;)
    // Check if the semaphore is avialable or not and wait forever.
    // Don't bother to grab the lock if it is not available (to avoid
    // deadlock.)
    switch (::WaitForSingleObject (s->count_nonzero_, INFINITE))
      {
      case WAIT_OBJECT_0:
        OS::thread_mutex_lock (&s->lock_);

        // Need to double check if the semaphore is still available.
        // This time, we shouldn't wait at all.
        if (::WaitForSingleObject (s->count_nonzero_, 0) == WAIT_OBJECT_0)
          {
            // Decrease the internal counter.  Only update the event
            // object's status when the state changed.
            s->count_--;
            if (s->count_ <= 0)
              OS::event_reset (&s->count_nonzero_);
            result = 0;
          }

        OS::thread_mutex_unlock (&s->lock_);
        // if we didn't get a hold on the semaphore, the result won't
        // be 0 and thus, we'll start from the beginning again.
        if (result == 0)
          return 0;
        break;

      default:
        // Since we wait indefinitely, anything other than
        // WAIT_OBJECT_O indicates an error.
        OS::set_errno_to_last_error ();
        // This is taken from the hack above. ;)
        return -1;
      }
  /* NOTREACHED */
#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#   elif defined (ACE_VXWORKS)
  OSCALL_RETURN (::semTake (s->sema_, WAIT_FOREVER), int, -1);
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
OS::sema_wait (ACE_sema_t *s, TimeValue &tv)
{
  OS_TRACE ("OS::sema_wait");
# if defined (ACE_HAS_POSIX_SEM)
#   if defined (ACE_HAS_POSIX_SEM_TIMEOUT)
  int rc;
  timespec_t ts;
  ts = tv; // Calls TimeValue::operator timespec_t().
  OSCALL (::sem_timedwait (s->sema_, &ts), int, -1, rc);
  if (rc == -1 && errno == ETIMEDOUT)
    errno = ETIME;  /* POSIX returns ETIMEDOUT but we need ETIME */
  return rc;
#   elif !defined (ACE_DISABLE_POSIX_SEM_TIMEOUT_EMULATION)
  int result = 0;
  bool expired = false;
  ACE_Errno_Guard error (errno);

  ACE_PTHREAD_CLEANUP_PUSH (&s->lock_);

  if (OS::mutex_lock (&s->lock_) != 0)
    result = -2;
  else
    {
      bool finished = true;
      do
      {
        result = OS::sema_trywait (s);
        if (result == -1 && errno == EAGAIN)
          expired = OS::gettimeofday () > tv;
        else
          expired = false;

        finished = result != -1 || expired ||
                   (result == -1 && errno != EAGAIN);
        if (!finished)
          {
            if (OS::cond_timedwait (&s->count_nonzero_,
                                        &s->lock_,
                                        &tv) == -1)
              {
                error = errno;
                result = -1;
                break;
              }
          }
      } while (!finished);

      if (expired)
        error = ETIME;

#     if defined (ACE_LACKS_COND_TIMEDWAIT_RESET)
      tv = OS::gettimeofday ();
#     endif /* ACE_LACKS_COND_TIMEDWAIT_RESET */
    }

  if (result != -2)
    OS::mutex_unlock (&s->lock_);
  ACE_PTHREAD_CLEANUP_POP (0);
  return result < 0 ? -1 : result;
#   else /* No native sem_timedwait(), and emulation disabled */
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_POSIX_SEM_TIMEOUT */
# elif defined (ACE_USES_FIFO_SEM)
  int rc;
  TimeValue now = OS::gettimeofday ();

  while (tv > now)
    {
      TimeValue timeout = tv;
      timeout -= now;

      ACE_Handle_Set  fds_;

      fds_.set_bit (s->fd_[0]);
      if ((rc = OS::select (ACE_Handle_Set::MAXSIZE, fds_, 0, 0, timeout)) != 1)
        {
          if (rc == 0 || errno != EAGAIN)
          {
            if (rc == 0)
              errno = ETIME;
            return (-1);
          }
        }

      // try to read the signal *but* do *not* block
      if (rc == 1 && OS::sema_trywait (s) == 0)
        return (0);

      // we were woken for input but someone beat us to it
      // so we wait again if there is still time
      now = OS::gettimeofday ();
    }

  // make sure errno is set right
  errno = ETIME;

  return (-1);
# elif defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_STHREADS)
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
#   elif defined (ACE_HAS_PTHREADS)
  int result = 0;
  ACE_Errno_Guard error (errno);

  ACE_PTHREAD_CLEANUP_PUSH (&s->lock_);

  if (OS::mutex_lock (&s->lock_) != 0)
    result = -1;
  else
    {
      // Keep track of the number of waiters so that we can signal
      // them properly in <OS::sema_post>.
      s->waiters_++;

      // Wait until the semaphore count is > 0 or until we time out.
      while (s->count_ == 0)
        if (OS::cond_timedwait (&s->count_nonzero_,
                                    &s->lock_,
                                    &tv) == -1)
          {
            error = errno;
            result = -2; // -2 means that we need to release the mutex.
            break;
          }

      --s->waiters_;
    }

  if (result == 0)
    {
#     if defined (ACE_LACKS_COND_TIMEDWAIT_RESET)
      tv = OS::gettimeofday ();
#     endif /* ACE_LACKS_COND_TIMEDWAIT_RESET */
      --s->count_;
    }

  if (result != -1)
    OS::mutex_unlock (&s->lock_);
  ACE_PTHREAD_CLEANUP_POP (0);
  return result < 0 ? -1 : result;
#   elif defined (ACE_HAS_WTHREADS)
#     if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  int msec_timeout;

  if (tv.sec () == 0 && tv.usec () == 0)
    msec_timeout = 0; // Do a "poll."
  else
    {
      // Note that we must convert between absolute time (which is
      // passed as a parameter) and relative time (which is what
      // <WaitForSingleObjects> expects).
      TimeValue relative_time (tv - OS::gettimeofday ());

      // Watchout for situations where a context switch has caused the
      // current time to be > the timeout.
      if (relative_time < TimeValue::zero)
        msec_timeout = 0;
      else
        msec_timeout = relative_time.msec ();
    }

  switch (::WaitForSingleObject (*s, msec_timeout))
    {
    case WAIT_OBJECT_0:
      tv = OS::gettimeofday ();     // Update time to when acquired
      return 0;
    case WAIT_TIMEOUT:
      errno = ETIME;
      return -1;
    default:
      // This is a hack, we need to find an appropriate mapping...
      OS::set_errno_to_last_error ();
      return -1;
    }
  /* NOTREACHED */
#     else /* ACE_USES_WINCE_SEMA_SIMULATION */
  // Note that in this mode, the acquire is done in two steps, and
  // we may get signaled but cannot grab the semaphore before
  // timeout.  In that case, we'll need to restart the process with
  // updated timeout value.

  // tv is an absolute time, but we need relative to work with the Windows
  // API. Also, some users have become accustomed to using a 0 time value
  // as a shortcut for "now", which works on non-Windows because 0 is
  // always earlier than now. However, the need to convert to relative time
  // means we need to watch out for this case.
  TimeValue end_time = tv;
  if (tv == TimeValue::zero)
    end_time = OS::gettimeofday ();
  TimeValue relative_time = end_time - OS::gettimeofday ();
  int result = -1;

  // While we are not timeout yet. >= 0 will let this go through once
  // and if not able to get the object, it should hit WAIT_TIMEOUT
  // right away.
  while (relative_time >= TimeValue::zero)
    {
      // Wait for our turn to get the object.
      switch (::WaitForSingleObject (s->count_nonzero_, relative_time.msec ()))
        {
        case WAIT_OBJECT_0:
          OS::thread_mutex_lock (&s->lock_);

          // Need to double check if the semaphore is still available.
          // We can only do a "try lock" styled wait here to avoid
          // blocking threads that want to signal the semaphore.
          if (::WaitForSingleObject (s->count_nonzero_, 0) == WAIT_OBJECT_0)
            {
              // As before, only reset the object when the semaphore
              // is no longer available.
              s->count_--;
              if (s->count_ <= 0)
                OS::event_reset (&s->count_nonzero_);
              result = 0;
            }

          OS::thread_mutex_unlock (&s->lock_);

          // Only return when we successfully get the semaphore.
          if (result == 0)
            {
              tv = OS::gettimeofday ();     // Update to time acquired
              return 0;
            }
          break;

          // We have timed out.
        case WAIT_TIMEOUT:
          errno = ETIME;
          return -1;

          // What?
        default:
          OS::set_errno_to_last_error ();
          // This is taken from the hack above. ;)
          return -1;
        };

      // Haven't been able to get the semaphore yet, update the
      // timeout value to reflect the remaining time we want to wait.
      relative_time = end_time - OS::gettimeofday ();
    }

  // We have timed out.
  errno = ETIME;
  return -1;
#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#   elif defined (ACE_VXWORKS)
  // Note that we must convert between absolute time (which is
  // passed as a parameter) and relative time (which is what
  // the system call expects).
  TimeValue relative_time (tv - OS::gettimeofday ());

  int ticks_per_sec = ::sysClkRateGet ();

  int ticks = relative_time.sec () * ticks_per_sec +
              relative_time.usec () * ticks_per_sec / ACE_ONE_SECOND_IN_USECS;
  if (::semTake (s->sema_, ticks) == ERROR)
    {
      if (errno == S_objLib_OBJ_TIMEOUT)
        // Convert the VxWorks errno to one that's common for to ACE
        // platforms.
        errno = ETIME;
      else if (errno == S_objLib_OBJ_UNAVAILABLE)
        errno = EBUSY;
      return -1;
    }
  else
    {
      tv = OS::gettimeofday ();  // Update to time acquired
      return 0;
    }
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (s);
  ACE_UNUSED_ARG (tv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_POSIX_SEM */
}

ACE_INLINE int
OS::sema_wait (ACE_sema_t *s, TimeValue *tv)
{
  return tv == 0 ? OS::sema_wait (s) : OS::sema_wait (s, *tv);
}

ACE_INLINE int
OS::semctl (int int_id, int semnum, int cmd, semun value)
{
  OS_TRACE ("OS::semctl");
#if defined (ACE_HAS_SYSV_IPC)
  OSCALL_RETURN (::semctl (int_id, semnum, cmd, value), int, -1);
#else
  ACE_UNUSED_ARG (int_id);
  ACE_UNUSED_ARG (semnum);
  ACE_UNUSED_ARG (cmd);
  ACE_UNUSED_ARG (value);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
OS::semget (key_t key, int nsems, int flags)
{
  OS_TRACE ("OS::semget");
#if defined (ACE_HAS_SYSV_IPC)
  OSCALL_RETURN (::semget (key, nsems, flags), int, -1);
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (nsems);
  ACE_UNUSED_ARG (flags);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}

ACE_INLINE int
OS::semop (int int_id, struct sembuf *sops, size_t nsops)
{
  OS_TRACE ("OS::semop");
#if defined (ACE_HAS_SYSV_IPC)
  OSCALL_RETURN (::semop (int_id, sops, nsops), int, -1);
#else
  ACE_UNUSED_ARG (int_id);
  ACE_UNUSED_ARG (sops);
  ACE_UNUSED_ARG (nsops);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_SYSV_IPC */
}


ACE_INLINE int
OS::thr_cancel (ACE_thread_t thr_id)
{
  OS_TRACE ("OS::thr_cancel");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
#   if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
#     ifdef pthread_cancel
        // If it's a macro we can't say "pthread_cancel"...
        OSCALL_RETURN (pthread_cancel (thr_id), int, -1);
#     else
        OSCALL_RETURN (pthread_cancel (thr_id), int, -1);
#     endif /* pthread_cancel */
#   else
  int result;
#     ifdef pthread_cancel
        // If it's a macro we can't say "pthread_cancel"...
        OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cancel (thr_id),
                                             result),
                           int, -1);
#     else
        OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_cancel (thr_id),
                                             result),
                           int, -1);
#     endif /* pthread_cancel */
#   endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
# elif defined (ACE_VXWORKS)
  ACE_hthread_t tid;
  OSCALL (::taskNameToId (thr_id), int, ERROR, tid);

  if (tid == ERROR)
    return -1;
  else
    OSCALL_RETURN (::taskDelete (tid), int, -1);
# else /* Could be ACE_HAS_PTHREADS && ACE_LACKS_PTHREAD_CANCEL */
  ACE_UNUSED_ARG (thr_id);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (thr_id);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_cmp (ACE_hthread_t t1, ACE_hthread_t t2)
{
#if defined (ACE_HAS_PTHREADS)
# if defined (pthread_equal)
  // If it's a macro we can't say "pthread_equal"...
  return pthread_equal (t1, t2);
# else
  return pthread_equal (t1, t2);
# endif /* pthread_equal */
#else /* For STHREADS, WTHREADS, and VXWORKS ... */
  // Hum, Do we need to treat WTHREAD differently?
  // levine 13 oct 98 % Probably, ACE_hthread_t is a HANDLE.
  return t1 == t2;
#endif /* ACE_HAS_PTHREADS */
}

ACE_INLINE int
OS::thr_continue (ACE_hthread_t target_thread)
{
  OS_TRACE ("OS::thr_continue");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_continue (target_thread), result), int, -1);
# elif defined (ACE_HAS_PTHREADS)
#  if defined (ACE_HAS_PTHREAD_CONTINUE)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_continue (target_thread),
                                       result),
                     int, -1);
#  elif defined (ACE_HAS_PTHREAD_CONTINUE_NP)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_continue_np (target_thread),
                                       result),
                     int, -1);
#  elif defined (ACE_HAS_PTHREAD_RESUME_NP)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_resume_np (target_thread),
                                       result),
                     int, -1);
#  else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#  endif /* ACE_HAS_PTHREAD_CONTINUE */
# elif defined (ACE_HAS_WTHREADS)
  DWORD result = ::ResumeThread (target_thread);
  if (result == ACE_SYSCALL_FAILED)
    ACE_FAIL_RETURN (-1);
  else
    return 0;
# elif defined (ACE_VXWORKS)
  OSCALL_RETURN (::taskResume (target_thread), int, -1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_getconcurrency (void)
{
  OS_TRACE ("OS::thr_getconcurrency");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
  return ::thr_getconcurrency ();
# elif defined (ACE_HAS_PTHREADS) && defined (ACE_HAS_PTHREAD_GETCONCURRENCY)
  return pthread_getconcurrency ();
# else
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_getprio (ACE_hthread_t ht_id, int &priority, int &policy)
{
  OS_TRACE ("OS::thr_getprio");
  ACE_UNUSED_ARG (policy);
#if defined (ACE_HAS_THREADS)
# if (defined (ACE_HAS_PTHREADS) && \
     (!defined (ACE_LACKS_SETSCHED) || defined (ACE_HAS_PTHREAD_SCHEDPARAM)))

#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  int const result = pthread_getprio (ht_id);
  if (result != -1)
    {
      priority = result;
      return 0;
    }
  else
    return -1;
#   elif defined (ACE_HAS_PTHREADS_DRAFT6)

  pthread_attr_t  attr;
  if (pthread_getschedattr (ht_id, &attr) == 0)
    {
      priority = pthread_attr_getprio (&attr);
      return 0;
    }
  return -1;
#   else

  struct sched_param param;
  int result;

  OSCALL (ACE_ADAPT_RETVAL (pthread_getschedparam (ht_id, &policy, &param),
                                result), int,
              -1, result);
  priority = param.sched_priority;
  return result;
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_getprio (ht_id, &priority), result), int, -1);
# elif defined (ACE_HAS_WTHREADS) && !defined (ACE_HAS_WINCE)
  ACE_Errno_Guard error (errno);

  priority = ::GetThreadPriority (ht_id);

#   if defined (ACE_HAS_PHARLAP)
#     if defined (ACE_PHARLAP_LABVIEW_RT)
  policy = ACE_SCHED_FIFO;
#     else
  DWORD timeslice = ::EtsGetTimeSlice ();
  policy = timeslice == 0 ? ACE_SCHED_OTHER : ACE_SCHED_FIFO;
#     endif /* ACE_PHARLAP_LABVIEW_RT */
#   else
  DWORD priority_class = ::GetPriorityClass (::GetCurrentProcess ());
  if (priority_class == 0 && (error = ::GetLastError ()) != NO_ERROR)
    ACE_FAIL_RETURN (-1);

  policy =
    (priority_class ==
     REALTIME_PRIORITY_CLASS) ? ACE_SCHED_FIFO : ACE_SCHED_OTHER;
#   endif /* ACE_HAS_PHARLAP */

  return 0;
# elif defined (ACE_VXWORKS)
  OSCALL_RETURN (::taskPriorityGet (ht_id, &priority), int, -1);
# else
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_getprio (ACE_hthread_t ht_id, int &priority)
{
  OS_TRACE ("OS::thr_getprio");
  int policy = 0;
  return OS::thr_getprio (ht_id, priority, policy);
}

#if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
ACE_INLINE int
OS::thr_getspecific_native (OS_thread_key_t key, void **data)
{
//  OS_TRACE ("OS::thr_getspecific_native");
# if defined (ACE_HAS_PTHREADS)
#  if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
    return pthread_getspecific (key, data);
#  else /* this is ACE_HAS_PTHREADS_DRAFT7 or STD */
    *data = pthread_getspecific (key);
    return 0;
#  endif       /*  ACE_HAS_PTHREADS */
# elif defined (ACE_HAS_STHREADS)
    int result;
    OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_getspecific (key, data), result), int, -1);
# elif defined (ACE_HAS_WTHREADS)
  *data = ::TlsGetValue (key);
  if (*data == 0 && ::GetLastError () != NO_ERROR)
    {
      OS::set_errno_to_last_error ();
      return -1;
    }
  else
    return 0;
# else /* ACE_HAS_PTHREADS etc.*/
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS etc.*/
}
#endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */

ACE_INLINE int
OS::thr_getspecific (ACE_thread_key_t key, void **data)
{
//   OS_TRACE ("OS::thr_getspecific");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_TSS_EMULATION)
    if (ACE_TSS_Emulation::is_key (key) == 0)
      {
        errno = EINVAL;
        data = 0;
        return -1;
      }
    else
      {
        *data = ACE_TSS_Emulation::ts_object (key);
        return 0;
      }
# elif defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
  return OS::thr_getspecific_native (key, data);
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_TSS_EMULATION */
#else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

#if !(defined (ACE_VXWORKS) && !defined (ACE_HAS_PTHREADS))
ACE_INLINE int
OS::thr_join (ACE_hthread_t thr_handle,
                  ACE_THR_FUNC_RETURN *status)
{
  OS_TRACE ("OS::thr_join");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_join (thr_handle, 0, status), result),
                     int, -1);
# elif defined (ACE_HAS_PTHREADS)
#   if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
  int ace_result;
#     if defined (ACE_LACKS_NULL_PTHREAD_STATUS)
  void *temp;
  OSCALL (pthread_join (thr_handle,
                              status == 0  ?  &temp  :  status),
              int, -1, ace_result);
#     else
  OSCALL (pthread_join (thr_handle, status), int, -1, ace_result);
#     endif /* ACE_LACKS_NULL_PTHREAD_STATUS */
  // Joinable threads need to be detached after joining on Pthreads
  // draft 4 (at least) to reclaim thread storage.
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
  pthread_detach (&thr_handle);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */

    return ace_result;

#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_join (thr_handle, status), result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4, 6 */
# elif defined (ACE_HAS_WTHREADS)
  ACE_THR_FUNC_RETURN local_status = 0;

  // Make sure that status is non-NULL.
  if (status == 0)
    status = &local_status;

  if (::WaitForSingleObject (thr_handle, INFINITE) == WAIT_OBJECT_0
      && ::GetExitCodeThread (thr_handle, status) != FALSE)
    {
      ::CloseHandle (thr_handle);
      return 0;
    }
  ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
# else
  ACE_UNUSED_ARG (thr_handle);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (thr_handle);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_join (ACE_thread_t waiter_id,
                  ACE_thread_t *thr_id,
                  ACE_THR_FUNC_RETURN *status)
{
  OS_TRACE ("OS::thr_join");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_join (waiter_id, thr_id, status), result),
                     int, -1);
# elif defined (ACE_HAS_PTHREADS)
  ACE_UNUSED_ARG (thr_id);
#   if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
#     if defined (ACE_LACKS_NULL_PTHREAD_STATUS)
  void *temp;
  OSCALL_RETURN (pthread_join (waiter_id,
    status == 0  ?  &temp  :  status), int, -1);
#     else
  OSCALL_RETURN (pthread_join (waiter_id, status), int, -1);
#     endif
#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_join (waiter_id, status), result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4, 6 */
# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (waiter_id);
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (status);

  // This could be implemented if the DLL-Main function or the
  // task exit base class some log the threads which have exited
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (waiter_id);
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (status);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}
#endif /* !VXWORKS */

ACE_INLINE int
OS::thr_kill (ACE_thread_t thr_id, int signum)
{
  OS_TRACE ("OS::thr_kill");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#   if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_LACKS_PTHREAD_KILL)
  ACE_UNUSED_ARG (signum);
  ACE_UNUSED_ARG (thr_id);
  ACE_NOTSUP_RETURN (-1);
#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_kill (thr_id, signum),
                                       result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_kill (thr_id, signum),
                                       result),
                     int, -1);
# elif defined (ACE_VXWORKS)
  ACE_hthread_t tid;
  OSCALL (::taskNameToId (thr_id), int, ERROR, tid);

  if (tid == ERROR)
    return -1;
  else
    OSCALL_RETURN (::kill (tid, signum), int, -1);

# else
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (signum);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (signum);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE size_t
OS::thr_min_stack (void)
{
  OS_TRACE ("OS::thr_min_stack");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
#   if defined (ACE_HAS_THR_MINSTACK)
  // Tandem did some weirdo mangling of STHREAD names...
  return ::thr_minstack ();
#   else
  return ::thr_min_stack ();
#   endif /* !ACE_HAS_THR_MINSTACK */
# elif defined (ACE_HAS_PTHREADS)
#   if defined (_SC_THREAD_STACK_MIN)
  return (size_t) OS::sysconf (_SC_THREAD_STACK_MIN);
#   elif defined (PTHREAD_STACK_MIN)
  return PTHREAD_STACK_MIN;
#   else
  ACE_NOTSUP_RETURN (0);
#   endif /* _SC_THREAD_STACK_MIN */
# elif defined (ACE_HAS_WTHREADS)
  ACE_NOTSUP_RETURN (0);
# elif defined (ACE_VXWORKS)
  TASK_DESC taskDesc;
  STATUS status;

  ACE_hthread_t tid;
  OS::thr_self (tid);

  OSCALL (ACE_ADAPT_RETVAL (::taskInfoGet (tid, &taskDesc),
                                status),
              STATUS, -1, status);
  return status == OK ? taskDesc.td_stackSize : 0;
# else /* Should not happen... */
  ACE_NOTSUP_RETURN (0);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_NOTSUP_RETURN (0);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE ACE_thread_t
OS::thr_self (void)
{
  // OS_TRACE ("OS::thr_self");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
  return pthread_self ();
# elif defined (ACE_HAS_STHREADS)
  OSCALL_RETURN (::thr_self (), int, -1);
# elif defined (ACE_HAS_WTHREADS)
  return ::GetCurrentThreadId ();
# elif defined (ACE_VXWORKS)
  return ::taskName (::taskIdSelf ());
# endif /* ACE_HAS_STHREADS */
#else
  return 1; // Might as well make it the first thread ;-)
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE void
OS::thr_self (ACE_hthread_t &self)
{
  OS_TRACE ("OS::thr_self");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
  self = pthread_self ();
# elif defined (ACE_HAS_THREAD_SELF)
  self = ::thread_self ();
# elif defined (ACE_HAS_STHREADS)
  self = ::thr_self ();
# elif defined (ACE_HAS_WTHREADS)
  self = ::GetCurrentThread ();
# elif defined (ACE_VXWORKS)
  self = ::taskIdSelf ();
# endif /* ACE_HAS_STHREADS */
#else
  self = 1; // Might as well make it the main thread ;-)
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_setcancelstate (int new_state, int *old_state)
{
  OS_TRACE ("OS::thr_setcancelstate");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  int old;
  old = pthread_setcancel (new_state);
  if (old == -1)
    return -1;
  *old_state = old;
  return 0;
#   elif defined (ACE_HAS_PTHREADS_DRAFT6)
  ACE_UNUSED_ARG (old_state);
  OSCALL_RETURN (pthread_setintr (new_state), int, -1);
#   else /* this is draft 7 or std */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setcancelstate (new_state,
                                                                 old_state),
                                       result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
# elif defined (ACE_HAS_STHREADS)
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
# else /* Could be ACE_HAS_PTHREADS && ACE_LACKS_PTHREAD_CANCEL */
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (new_state);
  ACE_UNUSED_ARG (old_state);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_setcanceltype (int new_type, int *old_type)
{
  OS_TRACE ("OS::thr_setcanceltype");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  int old;
  old = pthread_setasynccancel (new_type);
  if (old == -1)
    return -1;
  *old_type = old;
  return 0;
#   elif defined (ACE_HAS_PTHREADS_DRAFT6)
  ACE_UNUSED_ARG (old_type);
  OSCALL_RETURN (pthread_setintrtype (new_type), int, -1);
#   else /* this is draft 7 or std */
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setcanceltype (new_type,
                                                                old_type),
                                       result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
# else /* Could be ACE_HAS_PTHREADS && ACE_LACKS_PTHREAD_CANCEL */
  ACE_UNUSED_ARG (new_type);
  ACE_UNUSED_ARG (old_type);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (new_type);
  ACE_UNUSED_ARG (old_type);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_setconcurrency (int hint)
{
  OS_TRACE ("OS::thr_setconcurrency");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_setconcurrency (hint),
                                       result),
                     int, -1);
# elif defined (ACE_HAS_PTHREADS) && defined (ACE_HAS_PTHREAD_SETCONCURRENCY)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setconcurrency (hint),
                                       result),
                     int, -1);
# else
  ACE_UNUSED_ARG (hint);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (hint);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_setprio (ACE_hthread_t ht_id, int priority, int policy)
{
  OS_TRACE ("OS::thr_setprio");
  ACE_UNUSED_ARG (policy);
#if defined (ACE_HAS_THREADS)
# if (defined (ACE_HAS_PTHREADS) && \
      (!defined (ACE_LACKS_SETSCHED) || defined (ACE_HAS_PTHREAD_SCHEDPARAM)))

#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  int result;
  result = pthread_setprio (ht_id, priority);
  return (result == -1 ? -1 : 0);
#   elif defined (ACE_HAS_PTHREADS_DRAFT6)
  pthread_attr_t  attr;
  if (pthread_getschedattr (ht_id, &attr) == -1)
    return -1;
  if (pthread_attr_setprio (attr, priority) == -1)
    return -1;
  return pthread_setschedattr (ht_id, attr);
#   else
  int result;
  struct sched_param param;
  memset ((void *) &param, 0, sizeof param);

  // If <policy> is -1, we don't want to use it for
  // pthread_setschedparam().  Instead, obtain policy from
  // pthread_getschedparam().
  if (policy == -1)
    {
      OSCALL (ACE_ADAPT_RETVAL (pthread_getschedparam (ht_id, &policy, &param),
                                    result),
                  int, -1, result);
      if (result == -1)
        return result;
    }

  param.sched_priority = priority;

  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setschedparam (ht_id,
                                                                policy,
                                                                &param),
                                       result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_setprio (ht_id, priority),
                                       result),
                     int, -1);
# elif defined (ACE_HAS_WTHREADS)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::SetThreadPriority (ht_id, priority),
                                          ace_result_),
                        int, -1);
# elif defined (ACE_VXWORKS)
  OSCALL_RETURN (::taskPrioritySet (ht_id, priority), int, -1);
# else
  // For example, platforms that support Pthreads but LACK_SETSCHED.
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (ht_id);
  ACE_UNUSED_ARG (priority);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_sigsetmask (int how,
                        const sigset_t *nsm,
                        sigset_t *osm)
{
  OS_TRACE ("OS::thr_sigsetmask");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_LACKS_PTHREAD_THR_SIGSETMASK)
  // DCE threads and Solaris 2.4 have no such function.
  ACE_UNUSED_ARG (osm);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (how);

  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_HAS_SIGTHREADMASK)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::sigthreadmask (how, nsm, osm),
                                       result), int, -1);
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_sigsetmask (how, nsm, osm),
                                       result),
                     int, -1);
# elif defined (ACE_HAS_PTHREADS)
  // Draft 4 and 6 implementations will sometimes have a sigprocmask () that
  // modifies the calling thread's mask only.  If this is not so for your
  // platform, define ACE_LACKS_PTHREAD_THR_SIGSETMASK.
#   if defined (ACE_HAS_PTHREADS_DRAFT4) || \
    defined (ACE_HAS_PTHREADS_DRAFT6) || (defined (_UNICOS) && _UNICOS == 9)
  OSCALL_RETURN (::sigprocmask (how, nsm, osm), int, -1);
#   elif !defined (ACE_LACKS_PTHREAD_SIGMASK)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::pthread_sigmask (how, nsm, osm),
                                       result), int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 || _UNICOS 9 */

#if 0
  /* Don't know if any platform actually needs this... */
  // as far as I can tell, this is now pthread_sigaction() -- jwr
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_sigaction (how, nsm, osm),
                                       result), int, -1);
#endif /* 0 */

# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (osm);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (how);

  ACE_NOTSUP_RETURN (-1);
# elif defined (ACE_VXWORKS)
  int old_mask = 0;
  switch (how)
    {
    case SIG_BLOCK:
    case SIG_UNBLOCK:
      {
        // get the old mask
        old_mask = ::sigsetmask (*nsm);
        // create a new mask:  the following assumes that sigset_t is 4 bytes,
        // which it is on VxWorks 5.2, so bit operations are done simply . . .
        ::sigsetmask (how == SIG_BLOCK ? (old_mask |= *nsm) : (old_mask &= ~*nsm));
        if (osm)
          *osm = old_mask;
        break;
      }
    case SIG_SETMASK:
      old_mask = ::sigsetmask (*nsm);
      if (osm)
        *osm = old_mask;
      break;
    default:
      return -1;
    }

  return 0;
# else /* Should not happen. */
  ACE_UNUSED_ARG (how);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (osm);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_LACKS_PTHREAD_THR_SIGSETMASK */
#else
  ACE_UNUSED_ARG (how);
  ACE_UNUSED_ARG (nsm);
  ACE_UNUSED_ARG (osm);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thr_suspend (ACE_hthread_t target_thread)
{
  OS_TRACE ("OS::thr_suspend");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_suspend (target_thread), result), int, -1);
# elif defined (ACE_HAS_PTHREADS)
#  if defined (ACE_HAS_PTHREAD_SUSPEND)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_suspend (target_thread),
                                       result),
                     int, -1);
#  elif defined (ACE_HAS_PTHREAD_SUSPEND_NP)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_suspend_np (target_thread),
                                       result),
                     int, -1);
#  else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#  endif /* ACE_HAS_PTHREAD_SUSPEND */
# elif defined (ACE_HAS_WTHREADS)
  if (::SuspendThread (target_thread) != ACE_SYSCALL_FAILED)
    return 0;
  else
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */
# elif defined (ACE_VXWORKS)
  OSCALL_RETURN (::taskSuspend (target_thread), int, -1);
# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (target_thread);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE void
OS::thr_testcancel (void)
{
  OS_TRACE ("OS::thr_testcancel");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS) && !defined (ACE_LACKS_PTHREAD_CANCEL)
#if defined(ACE_HAS_PTHREADS_DRAFT6)
  pthread_testintr ();
#else /* ACE_HAS_PTHREADS_DRAFT6 */
  pthread_testcancel ();
#endif /* !ACE_HAS_PTHREADS_DRAFT6 */
# elif defined (ACE_HAS_STHREADS)
# elif defined (ACE_HAS_WTHREADS)
# elif defined (ACE_VXWORKS)
# else
  // no-op:  can't use ACE_NOTSUP_RETURN because there is no return value
# endif /* ACE_HAS_PTHREADS */
#else
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE void
OS::thr_yield (void)
{
  OS_TRACE ("OS::thr_yield");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#   if defined (ACE_HAS_PTHREADS_STD)
  // Note - this is a POSIX.4 function - not a POSIX.1c function...
  ::sched_yield ();
#   elif defined (ACE_HAS_PTHREADS_DRAFT6)
  pthread_yield (0);
#   else    /* Draft 4 and 7 */
  pthread_yield ();
#   endif  /* ACE_HAS_PTHREADS_STD */
# elif defined (ACE_HAS_STHREADS)
  ::thr_yield ();
# elif defined (ACE_HAS_WTHREADS)
  ::Sleep (0);
# elif defined (ACE_VXWORKS)
  // An argument of 0 to ::taskDelay doesn't appear to yield the
  // current thread.
  // Now, it does seem to work.  The context_switch_time test
  // works fine with task_delay set to 0.
  ::taskDelay (0);
# endif /* ACE_HAS_STHREADS */
#else
  ;
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thread_mutex_destroy (ACE_thread_mutex_t *m)
{
  OS_TRACE ("OS::thread_mutex_destroy");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ::DeleteCriticalSection (m);
  return 0;

# elif defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS) || defined (ACE_VXWORKS)
  return OS::mutex_destroy (m);

# endif /* ACE_HAS_STHREADS || ACE_HAS_PTHREADS */

#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);

#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thread_mutex_init (ACE_thread_mutex_t *m,
                           int lock_type,
                           const char *name,
                           ACE_mutexattr_t *arg)
{
  // OS_TRACE ("OS::thread_mutex_init");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);

  ACE_SEH_TRY
    {
      ::InitializeCriticalSection (m);
    }
  ACE_SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
      errno = ENOMEM;
      return -1;
    }
  return 0;

# elif defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS)
  // Force the use of USYNC_THREAD!
  return OS::mutex_init (m, USYNC_THREAD, name, arg, 0, lock_type);
# elif defined (ACE_VXWORKS)
  return mutex_init (m, lock_type, name, arg);

# endif /* ACE_HAS_STHREADS || ACE_HAS_PTHREADS */

#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);

#endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_WCHAR)
ACE_INLINE int
OS::thread_mutex_init (ACE_thread_mutex_t *m,
                           int lock_type,
                           const wchar_t *name,
                           ACE_mutexattr_t *arg)
{
  // OS_TRACE ("OS::thread_mutex_init");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);

  ACE_SEH_TRY
    {
      ::InitializeCriticalSection (m);
    }
  ACE_SEH_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
      errno = ENOMEM;
      return -1;
    }
  return 0;

# elif defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS)
  // Force the use of USYNC_THREAD!
  return OS::mutex_init (m, USYNC_THREAD, name, arg, 0, lock_type);

# elif defined (ACE_VXWORKS)
  return mutex_init (m, lock_type, name, arg);

# endif /* ACE_HAS_STHREADS || ACE_HAS_PTHREADS */

#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (lock_type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);

#endif /* ACE_HAS_THREADS */
}
#endif /* ACE_HAS_WCHAR */

ACE_INLINE int
OS::thread_mutex_lock (ACE_thread_mutex_t *m)
{
  // OS_TRACE ("OS::thread_mutex_lock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ::EnterCriticalSection (m);
  return 0;
# elif defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS) || defined (ACE_VXWORKS)
  return OS::mutex_lock (m);
# endif /* ACE_HAS_STHREADS || ACE_HAS_PTHREADS || VXWORKS */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thread_mutex_lock (ACE_thread_mutex_t *m,
                           const TimeValue &timeout)
{
  // OS_TRACE ("OS::thread_mutex_lock");

  // For all platforms, except MS Windows, this method is equivalent
  // to calling OS::mutex_lock() since ACE_thread_mutex_t and
  // ACE_mutex_t are the same type.  However, those typedefs evaluate
  // to different types on MS Windows.  The "thread mutex"
  // implementation in ACE for MS Windows cannot readily support
  // timeouts due to a lack of timeout features for this type of MS
  // Windows synchronization mechanism.

#if defined (ACE_HAS_THREADS) && !defined (ACE_HAS_WTHREADS)
# if defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS) || defined (ACE_VXWORKS)
  return OS::mutex_lock (m, timeout);
#endif /* ACE_HAS_STHREADS || ACE_HAS_PTHREADS || VXWORKS */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thread_mutex_lock (ACE_thread_mutex_t *m,
                           const TimeValue *timeout)
{
  return timeout == 0
    ? OS::thread_mutex_lock (m)
    : OS::thread_mutex_lock (m, *timeout);
}

ACE_INLINE int
OS::thread_mutex_trylock (ACE_thread_mutex_t *m)
{
  OS_TRACE ("OS::thread_mutex_trylock");

#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
#   if defined (ACE_HAS_WIN32_TRYLOCK)
  BOOL result = ::TryEnterCriticalSection (m);
  if (result == TRUE)
    return 0;
  else
    {
      errno = EBUSY;
      return -1;
    }
#   else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_WIN32_TRYLOCK */
# elif defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS) || defined (ACE_VXWORKS)
  return OS::mutex_trylock (m);
#endif /* Threads variety case */

#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

ACE_INLINE int
OS::thread_mutex_unlock (ACE_thread_mutex_t *m)
{
  OS_TRACE ("OS::thread_mutex_unlock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_WTHREADS)
  ::LeaveCriticalSection (m);
  return 0;
# elif defined (ACE_HAS_STHREADS) || defined (ACE_HAS_PTHREADS) || defined (ACE_VXWORKS)
  return OS::mutex_unlock (m);
# endif /* Threads variety case */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}


# if defined (ACE_IS_SPLITTING)
#   define ACE_SPECIAL_INLINE
# else
#   define ACE_SPECIAL_INLINE ACE_INLINE
//#   define ACE_SPECIAL_INLINE inline
# endif

/*****************************************************************************/



/*****************************************************************************/

ACE_INLINE
ACE_Thread_ID::ACE_Thread_ID (ACE_thread_t thread_id,
                              ACE_hthread_t thread_handle)
  : thread_id_ (thread_id),
    thread_handle_ (thread_handle)
{
}

ACE_INLINE
ACE_Thread_ID::ACE_Thread_ID (const ACE_Thread_ID &id)
  : thread_id_ (id.thread_id_),
    thread_handle_ (id.thread_handle_)
{
}

ACE_INLINE
ACE_Thread_ID::ACE_Thread_ID (void)
  : thread_id_ (OS::thr_self ())
{
  OS::thr_self (thread_handle_);
}

ACE_INLINE
ACE_thread_t
ACE_Thread_ID::id (void) const
{
  return this->thread_id_;
}

ACE_INLINE void
ACE_Thread_ID::id (ACE_thread_t thread_id)
{
  this->thread_id_ = thread_id;
}

ACE_INLINE ACE_hthread_t
ACE_Thread_ID::handle (void) const
{
  return this->thread_handle_;
}

ACE_INLINE void
ACE_Thread_ID::handle (ACE_hthread_t thread_handle)
{
  this->thread_handle_ = thread_handle;
}

ACE_INLINE bool
ACE_Thread_ID::operator== (const ACE_Thread_ID &rhs) const
{
  return
    OS::thr_cmp (this->thread_handle_, rhs.thread_handle_)
    && OS::thr_equal (this->thread_id_, rhs.thread_id_);
}

ACE_INLINE bool
ACE_Thread_ID::operator!= (const ACE_Thread_ID &rhs) const
{
  return !(*this == rhs);
}


void
ACE_Thread_ID::to_string (char *thr_string) const
{
  char format[128]; // Converted format string
  char *fp = 0;     // Current format pointer
  fp = format;
  *fp++ = '%';   // Copy in the %

#if defined (ACE_WIN32)
  ::strcpy (fp, "u");
  ::sprintf (thr_string,
                   format,
                   static_cast <unsigned> (thread_id_));
#elif defined (DIGITAL_UNIX)
                  OS::strcpy (fp, "u");
                  OS::sprintf (thr_string, format,
#  if defined (ACE_HAS_THREADS)
                                   thread_id_
#  else
                                   thread_id_
#  endif /* ACE_HAS_THREADS */
                                          );
#else

#  if defined (ACE_MVS) || defined (ACE_TANDEM_T1248_PTHREADS)
                  // MVS's pthread_t is a struct... yuck. So use the ACE 5.0
                  // code for it.
                  OS::strcpy (fp, "u");
                  OS::sprintf (thr_string, format, thread_handle_);
#  else
                  // Yes, this is an ugly C-style cast, but the
                  // correct C++ cast is different depending on
                  // whether the t_id is an integral type or a pointer
                  // type. FreeBSD uses a pointer type, but doesn't
                  // have a _np function to get an integral type, like
                  // the OSes above.
                  OS::strcpy (fp, "lu");
                  sprintf (thr_string,
                                   format,
                                   (unsigned long) thread_handle_);
#  endif /* ACE_MVS || ACE_TANDEM_T1248_PTHREADS */

#endif /* ACE_WIN32 */
}

/*****************************************************************************/

#if defined (ACE_WIN32) || defined (ACE_HAS_TSS_EMULATION)

#if defined (ACE_HAS_TSS_EMULATION)
u_int ACE_TSS_Emulation::total_keys_ = 0;

ACE_TSS_Keys* ACE_TSS_Emulation::tss_keys_used_ = 0;

ACE_TSS_Emulation::ACE_TSS_DESTRUCTOR
ACE_TSS_Emulation::tss_destructor_[ACE_TSS_Emulation::ACE_TSS_THREAD_KEYS_MAX]
 = { 0 };

#  if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)

int ACE_TSS_Emulation::key_created_ = 0;

OS_thread_key_t ACE_TSS_Emulation::native_tss_key_;

/* static */
#    if defined (ACE_HAS_THR_C_FUNC)
extern "C"
void
ACE_TSS_Emulation_cleanup (void *ptr)
{
   ACE_UNUSED_ARG (ptr);
   // Really this must be used for ACE_TSS_Emulation code to make the TSS
   // cleanup
}
#    else
void
ACE_TSS_Emulation_cleanup (void *ptr)
{
   ACE_UNUSED_ARG (ptr);
   // Really this must be used for ACE_TSS_Emulation code to make the TSS
   // cleanup
}
#    endif /* ACE_HAS_THR_C_FUNC */

void **
ACE_TSS_Emulation::tss_base (void* ts_storage[], u_int *ts_created)
{
  // TSS Singleton implementation.

  // Create the one native TSS key, if necessary.
  if (key_created_ == 0)
    {
      // Double-checked lock . . .
      ACE_TSS_BASE_GUARD

      if (key_created_ == 0)
        {
          ACE_NO_HEAP_CHECK;
          if (OS::thr_keycreate_native (&native_tss_key_,
                                     &ACE_TSS_Emulation_cleanup) != 0)
            {
              ACE_ASSERT (0);
              return 0; // Major problems, this should *never* happen!
            }
          key_created_ = 1;
        }
    }

  void **old_ts_storage = 0;

  // Get the tss_storage from thread-OS specific storage.
  if (OS::thr_getspecific_native (native_tss_key_,
                               (void **) &old_ts_storage) == -1)
    {
      ACE_ASSERT (false);
      return 0; // This should not happen!
    }

  // Check to see if this is the first time in for this thread.
  // This block can also be entered after a fork () in the child process,
  // at least on Pthreads Draft 4 platforms.
  if (old_ts_storage == 0)
    {
      if (ts_created)
        *ts_created = 1u;

      // Use the ts_storage passed as argument, if non-zero.  It is
      // possible that this has been implemented in the stack. At the
      // moment, this is unknown.  The cleanup must not do nothing.
      // If ts_storage is zero, allocate (and eventually leak) the
      // storage array.
      if (ts_storage == 0)
        {
          ACE_NO_HEAP_CHECK;

          ACE_NEW_RETURN (ts_storage,
                          void*[ACE_TSS_THREAD_KEYS_MAX],
                          0);

          // Zero the entire TSS array.  Do it manually instead of
          // using memset, for optimum speed.  Though, memset may be
          // faster :-)
          void **tss_base_p = ts_storage;

          for (u_int i = 0;
               i < ACE_TSS_THREAD_KEYS_MAX;
               ++i)
            *tss_base_p++ = 0;
        }

       // Store the pointer in thread-specific storage.  It gets
       // deleted via the ACE_TSS_Emulation_cleanup function when the
       // thread terminates.
       if (OS::thr_setspecific_native (native_tss_key_,
                                    (void *) ts_storage) != 0)
          {
            ACE_ASSERT (false);
            return 0; // This should not happen!
          }
    }
  else
    if (ts_created)
      ts_created = 0;

  return ts_storage  ?  ts_storage  :  old_ts_storage;
}
#  endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */

u_int
ACE_TSS_Emulation::total_keys ()
{
  OS_Recursive_Thread_Mutex_Guard (
    *static_cast <ACE_recursive_thread_mutex_t *>
                      (OS_Object_Manager::preallocated_object[
                        OS_Object_Manager::ACE_TSS_KEY_LOCK]));

  return total_keys_;
}

int
ACE_TSS_Emulation::next_key (ACE_thread_key_t &key)
{
  OS_Recursive_Thread_Mutex_Guard (
    *static_cast <ACE_recursive_thread_mutex_t *>
                      (OS_Object_Manager::preallocated_object[
                        OS_Object_Manager::ACE_TSS_KEY_LOCK]));

  // Initialize the tss_keys_used_ pointer on first use.
  if (tss_keys_used_ == 0)
    {
      ACE_NEW_RETURN (tss_keys_used_, ACE_TSS_Keys, -1);
    }

  if (total_keys_ < ACE_TSS_THREAD_KEYS_MAX)
    {
       u_int counter = 0;
       // Loop through all possible keys and check whether a key is free
       for ( ;counter < ACE_TSS_THREAD_KEYS_MAX; counter++)
         {
            ACE_thread_key_t localkey;
#  if defined (ACE_HAS_NONSCALAR_THREAD_KEY_T)
              OS::memset (&localkey, 0, sizeof (ACE_thread_key_t));
              OS::memcpy (&localkey, &counter_, sizeof (u_int));
#  else
              localkey = counter;
#  endif /* ACE_HAS_NONSCALAR_THREAD_KEY_T */
            // If the key is not set as used, we can give out this key, if not
            // we have to search further
            if (tss_keys_used_->is_set(localkey) == 0)
            {
               tss_keys_used_->test_and_set(localkey);
               key = localkey;
               break;
            }
         }

      ++total_keys_;
      return 0;
    }
  else
    {
      key = OS::NULL_key;
      return -1;
    }
}

int
ACE_TSS_Emulation::release_key (ACE_thread_key_t key)
{
  OS_Recursive_Thread_Mutex_Guard (
    *static_cast <ACE_recursive_thread_mutex_t *>
                      (OS_Object_Manager::preallocated_object[
                        OS_Object_Manager::ACE_TSS_KEY_LOCK]));

  if (tss_keys_used_ != 0 &&
      tss_keys_used_->test_and_clear (key) == 0)
  {
    --total_keys_;
    return 0;
  }
  return 1;
}

int
ACE_TSS_Emulation::is_key (ACE_thread_key_t key)
{
  OS_Recursive_Thread_Mutex_Guard (
    *static_cast <ACE_recursive_thread_mutex_t *>
                      (OS_Object_Manager::preallocated_object[
                        OS_Object_Manager::ACE_TSS_KEY_LOCK]));

  if (tss_keys_used_ != 0 &&
      tss_keys_used_->is_set (key) == 1)
  {
    return 1;
  }
  return 0;
}

void *
ACE_TSS_Emulation::tss_open (void *ts_storage[ACE_TSS_THREAD_KEYS_MAX])
{
#    if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
        // On VxWorks, in particular, don't check to see if the field
        // is 0.  It isn't always, specifically, when a program is run
        // directly by the shell (without spawning a new task) after
        // another program has been run.

  u_int ts_created = 0;
  tss_base (ts_storage, &ts_created);
  if (ts_created)
    {
#    else  /* ! ACE_HAS_THREAD_SPECIFIC_STORAGE */
      tss_base () = ts_storage;
#    endif

      // Zero the entire TSS array.  Do it manually instead of using
      // memset, for optimum speed.  Though, memset may be faster :-)
      void **tss_base_p = tss_base ();
      for (u_int i = 0; i < ACE_TSS_THREAD_KEYS_MAX; ++i, ++tss_base_p)
        {
          *tss_base_p = 0;
        }

      return tss_base ();
#    if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
    }
  else
    {
      return 0;
    }
#    endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */
}

void
ACE_TSS_Emulation::tss_close ()
{
#if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
  OS::thr_keyfree_native (native_tss_key_);
#endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */
}

#endif /* ACE_HAS_TSS_EMULATION */

#endif /* WIN32 || ACE_HAS_TSS_EMULATION */

/*****************************************************************************/

#if defined (ACE_WIN32) || defined (ACE_HAS_TSS_EMULATION)

// Moved class ACE_TSS_Ref declaration to OS.h so it can be visible to
// the single file of template instantiations.

ACE_TSS_Ref::ACE_TSS_Ref (ACE_thread_t id)
  : tid_(id)
{
  OS_TRACE ("ACE_TSS_Ref::ACE_TSS_Ref");
}

ACE_TSS_Ref::ACE_TSS_Ref (void)
{
  OS_TRACE ("ACE_TSS_Ref::ACE_TSS_Ref");
}

// Check for equality.
bool
ACE_TSS_Ref::operator== (const ACE_TSS_Ref &info) const
{
  OS_TRACE ("ACE_TSS_Ref::operator==");

  return this->tid_ == info.tid_;
}

// Check for inequality.
ACE_SPECIAL_INLINE
bool
ACE_TSS_Ref::operator != (const ACE_TSS_Ref &tss_ref) const
{
  OS_TRACE ("ACE_TSS_Ref::operator !=");

  return !(*this == tss_ref);
}

// moved class ACE_TSS_Info declaration
// to OS.h so it can be visible to the
// single file of template instantiations

ACE_TSS_Info::ACE_TSS_Info (ACE_thread_key_t key,
                            ACE_TSS_Info::Destructor dest)
  : key_ (key),
    destructor_ (dest),
    thread_count_ (-1)
{
  OS_TRACE ("ACE_TSS_Info::ACE_TSS_Info");
}

ACE_TSS_Info::ACE_TSS_Info (void)
  : key_ (OS::NULL_key),
    destructor_ (0),
    thread_count_ (-1)
{
  OS_TRACE ("ACE_TSS_Info::ACE_TSS_Info");
}

# if defined (ACE_HAS_NONSCALAR_THREAD_KEY_T)
static inline bool operator== (const ACE_thread_key_t &lhs,
                               const ACE_thread_key_t &rhs)
{
  return ! OS::memcmp (&lhs, &rhs, sizeof (ACE_thread_key_t));
}

static inline bool operator!= (const ACE_thread_key_t &lhs,
                               const ACE_thread_key_t &rhs)
{
  return ! (lhs == rhs);
}
# endif /* ACE_HAS_NONSCALAR_THREAD_KEY_T */

// Check for equality.
bool
ACE_TSS_Info::operator== (const ACE_TSS_Info &info) const
{
  OS_TRACE ("ACE_TSS_Info::operator==");

  return this->key_ == info.key_;
}

// Check for inequality.
bool
ACE_TSS_Info::operator != (const ACE_TSS_Info &info) const
{
  OS_TRACE ("ACE_TSS_Info::operator !=");

  return !(*this == info);
}

void
ACE_TSS_Info::dump (void)
{
# if defined (ACE_HAS_DUMP)
  //  OS_TRACE ("ACE_TSS_Info::dump");

#   if 0
  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("key_ = %u\n"), this->key_));
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("destructor_ = %u\n"), this->destructor_));
  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
#   endif /* 0 */
# endif /* ACE_HAS_DUMP */
}

// Moved class ACE_TSS_Keys declaration to OS.h so it can be visible
// to the single file of template instantiations.

ACE_TSS_Keys::ACE_TSS_Keys (void)
{
  for (u_int i = 0; i < ACE_WORDS; ++i)
    {
      key_bit_words_[i] = 0;
    }
}

ACE_SPECIAL_INLINE
void
ACE_TSS_Keys::find (const u_int key, u_int &word, u_int &bit)
{
  word = key / ACE_BITS_PER_WORD;
  bit = key % ACE_BITS_PER_WORD;
}

int
ACE_TSS_Keys::test_and_set (const ACE_thread_key_t key)
{
  ACE_KEY_INDEX (key_index, key);
  u_int word, bit;
  find (key_index, word, bit);

  if (ACE_BIT_ENABLED (key_bit_words_[word], 1 << bit))
    {
      return 1;
    }
  else
    {
      ACE_SET_BITS (key_bit_words_[word], 1 << bit);
      return 0;
    }
}

int
ACE_TSS_Keys::test_and_clear (const ACE_thread_key_t key)
{
  ACE_KEY_INDEX (key_index, key);
  u_int word, bit;
  find (key_index, word, bit);

  if (word < ACE_WORDS && ACE_BIT_ENABLED (key_bit_words_[word], 1 << bit))
    {
      ACE_CLR_BITS (key_bit_words_[word], 1 << bit);
      return 0;
    }
  else
    {
      return 1;
    }
}

int
ACE_TSS_Keys::is_set (const ACE_thread_key_t key) const
{
  ACE_KEY_INDEX (key_index, key);
  u_int word, bit;
  find (key_index, word, bit);

  return word < ACE_WORDS ? ACE_BIT_ENABLED (key_bit_words_[word], 1 << bit) : 0;
}

#endif /* ACE_WIN32 || ACE_HAS_TSS_EMULATION */

/*****************************************************************************/

// = Static initialization.

// This is necessary to deal with POSIX pthreads insanity.  This
// guarantees that we've got a "zero'd" thread id even when
// ACE_thread_t, ACE_hthread_t, and ACE_thread_key_t are implemented
// as structures...  Under no circumstances should these be given
// initial values.
// Note: these three objects require static construction.
ACE_thread_t OS::NULL_thread;
ACE_hthread_t OS::NULL_hthread;
#if defined (ACE_HAS_TSS_EMULATION)
  ACE_thread_key_t OS::NULL_key = static_cast <ACE_thread_key_t> (-1);
#else  /* ! ACE_HAS_TSS_EMULATION */
  ACE_thread_key_t OS::NULL_key;
#endif /* ! ACE_HAS_TSS_EMULATION */

/*****************************************************************************/


/*****************************************************************************/
// CONDITIONS BEGIN
/*****************************************************************************/

#if defined (ACE_LACKS_COND_T)
// NOTE: The OS::cond_* functions for some non-Unix platforms are
// defined here either because they're too big to be inlined, or
// to avoid use before definition if they were inline.

// @@ The following functions could be inlined if i could figure where
// to put it among the #ifdefs!
int
OS::condattr_init (ACE_condattr_t &attributes,
                       int type)
{
  attributes.type = type;
  return 0;
}

int
OS::condattr_destroy (ACE_condattr_t &)
{
  return 0;
}

int
OS::cond_broadcast (ACE_cond_t *cv)
{
  OS_TRACE ("OS::cond_broadcast");
# if defined (ACE_HAS_THREADS)
  // The <external_mutex> must be locked before this call is made.

  // This is needed to ensure that <waiters_> and <was_broadcast_> are
  // consistent relative to each other.
  OS::thread_mutex_lock (&cv->waiters_lock_);
  int have_waiters = 0;

  if (cv->waiters_ > 0)
    {
      // We are broadcasting, even if there is just one waiter...
      // Record the fact that we are broadcasting.  This helps the
      // cond_wait() method know how to optimize itself.  Be sure to
      // set this with the <waiters_lock_> held.
      cv->was_broadcast_ = 1;
      have_waiters = 1;
    }
  OS::thread_mutex_unlock (&cv->waiters_lock_);
  int result = 0;
  if (have_waiters)
    {
      // Wake up all the waiters.
      if (OS::sema_post (&cv->sema_, cv->waiters_) == -1)
        result = -1;
      // Wait for all the awakened threads to acquire their part of
      // the counting semaphore.
#   if defined (ACE_VXWORKS)
      else if (OS::sema_wait (&cv->waiters_done_) == -1)
#   else
      else if (OS::event_wait (&cv->waiters_done_) == -1)
#   endif /* ACE_VXWORKS */
        result = -1;
      // This is okay, even without the <waiters_lock_> held because
      // no other waiter threads can wake up to access it.
      cv->was_broadcast_ = 0;
    }
  return result;
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

int
OS::cond_destroy (ACE_cond_t *cv)
{
  OS_TRACE ("OS::cond_destroy");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_WTHREADS)
  OS::event_destroy (&cv->waiters_done_);
#   elif defined (ACE_VXWORKS)
  OS::sema_destroy (&cv->waiters_done_);
#   endif /* ACE_VXWORKS */
  OS::thread_mutex_destroy (&cv->waiters_lock_);
  return OS::sema_destroy (&cv->sema_);
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

int
OS::cond_init (ACE_cond_t *cv,
                   ACE_condattr_t &attributes,
                   const char *name, void *arg)
{
  return
    OS::cond_init (cv, static_cast<short> (attributes.type), name, arg);
}

# if defined (ACE_HAS_WCHAR)
int
OS::cond_init (ACE_cond_t *cv,
                   ACE_condattr_t &attributes,
                   const wchar_t *name, void *arg)
{
  return
    OS::cond_init (cv, static_cast<short> (attributes.type), name, arg);
}
# endif /* ACE_HAS_WCHAR */

int
OS::cond_init (ACE_cond_t *cv, short type, const char *name, void *arg)
{
  OS_TRACE ("OS::cond_init");
# if defined (ACE_HAS_THREADS)
  cv->waiters_ = 0;
  cv->was_broadcast_ = 0;

  int result = 0;
  if (OS::sema_init (&cv->sema_, 0, type, name, arg) == -1)
    result = -1;
  else if (OS::thread_mutex_init (&cv->waiters_lock_) == -1)
    result = -1;
#   if defined (ACE_VXWORKS)
  else if (OS::sema_init (&cv->waiters_done_, 0, type) == -1)
#   else
  else if (OS::event_init (&cv->waiters_done_) == -1)
#   endif /* ACE_VXWORKS */
    result = -1;
  return result;
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

# if defined (ACE_HAS_WCHAR)
int
OS::cond_init (ACE_cond_t *cv, short type, const wchar_t *name, void *arg)
{
  OS_TRACE ("OS::cond_init");
#   if defined (ACE_HAS_THREADS)
  cv->waiters_ = 0;
  cv->was_broadcast_ = 0;

  int result = 0;
  if (OS::sema_init (&cv->sema_, 0, type, name, arg) == -1)
    result = -1;
  else if (OS::thread_mutex_init (&cv->waiters_lock_) == -1)
    result = -1;
#     if defined (ACE_VXWORKS)
  else if (OS::sema_init (&cv->waiters_done_, 0, type) == -1)
#     else
  else if (OS::event_init (&cv->waiters_done_) == -1)
#     endif /* ACE_VXWORKS */
    result = -1;
  return result;
#   else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_THREADS */
}
# endif /* ACE_HAS_WCHAR */

int
OS::cond_signal (ACE_cond_t *cv)
{
  OS_TRACE ("OS::cond_signal");
# if defined (ACE_HAS_THREADS)
  // If there aren't any waiters, then this is a no-op.  Note that
  // this function *must* be called with the <external_mutex> held
  // since other wise there is a race condition that can lead to the
  // lost wakeup bug...  This is needed to ensure that the <waiters_>
  // value is not in an inconsistent internal state while being
  // updated by another thread.
  OS::thread_mutex_lock (&cv->waiters_lock_);
  int have_waiters = cv->waiters_ > 0;
  OS::thread_mutex_unlock (&cv->waiters_lock_);

  if (have_waiters != 0)
    return OS::sema_post (&cv->sema_);
  else
    return 0; // No-op
# else
  ACE_UNUSED_ARG (cv);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

int
OS::cond_wait (ACE_cond_t *cv,
                   ACE_mutex_t *external_mutex)
{
  OS_TRACE ("OS::cond_wait");
# if defined (ACE_HAS_THREADS)
  // Prevent race conditions on the <waiters_> count.
  OS::thread_mutex_lock (&cv->waiters_lock_);
  ++cv->waiters_;
  OS::thread_mutex_unlock (&cv->waiters_lock_);

  int result = 0;

#   if defined (ACE_HAS_SIGNAL_OBJECT_AND_WAIT)
  if (external_mutex->type_ == USYNC_PROCESS)
    // This call will automatically release the mutex and wait on the semaphore.
    ACE_WIN32CALL (ACE_ADAPT_RETVAL (::SignalObjectAndWait (external_mutex->proc_mutex_,
                                                            cv->sema_, INFINITE, FALSE),
                                     result),
                   int, -1, result);
  else
#   endif /* ACE_HAS_SIGNAL_OBJECT_AND_WAIT */
    {
      // We keep the lock held just long enough to increment the count of
      // waiters by one.  Note that we can't keep it held across the call
      // to OS::sema_wait() since that will deadlock other calls to
      // OS::cond_signal().
      if (OS::mutex_unlock (external_mutex) != 0)
        return -1;

      // Wait to be awakened by a OS::cond_signal() or
      // OS::cond_broadcast().
      result = OS::sema_wait (&cv->sema_);
    }

  // Reacquire lock to avoid race conditions on the <waiters_> count.
  OS::thread_mutex_lock (&cv->waiters_lock_);

  // We're ready to return, so there's one less waiter.
  --cv->waiters_;

  int last_waiter = cv->was_broadcast_ && cv->waiters_ == 0;

  // Release the lock so that other collaborating threads can make
  // progress.
  OS::thread_mutex_unlock (&cv->waiters_lock_);

  if (result == -1)
    // Bad things happened, so let's just return below.
    /* NOOP */;
#   if defined (ACE_HAS_SIGNAL_OBJECT_AND_WAIT)
  else if (external_mutex->type_ == USYNC_PROCESS)
    {
      if (last_waiter)

        // This call atomically signals the <waiters_done_> event and
        // waits until it can acquire the mutex.  This is important to
        // prevent unfairness.
        ACE_WIN32CALL (ACE_ADAPT_RETVAL (::SignalObjectAndWait (cv->waiters_done_,
                                                                external_mutex->proc_mutex_,
                                                                INFINITE, FALSE),
                                         result),
                       int, -1, result);
      else
        // We must always regain the <external_mutex>, even when
        // errors occur because that's the guarantee that we give to
        // our callers.
        OS::mutex_lock (external_mutex);

      return result;
      /* NOTREACHED */
    }
#   endif /* ACE_HAS_SIGNAL_OBJECT_AND_WAIT */
  // If we're the last waiter thread during this particular broadcast
  // then let all the other threads proceed.
  else if (last_waiter)
#   if defined (ACE_VXWORKS)
    OS::sema_post (&cv->waiters_done_);
#   else
    OS::event_signal (&cv->waiters_done_);
#   endif /* ACE_VXWORKS */

  // We must always regain the <external_mutex>, even when errors
  // occur because that's the guarantee that we give to our callers.
  OS::mutex_lock (external_mutex);

  return result;
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (external_mutex);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

int
OS::cond_timedwait (ACE_cond_t *cv,
                        ACE_mutex_t *external_mutex,
                        TimeValue *timeout)
{
  OS_TRACE ("OS::cond_timedwait");
# if defined (ACE_HAS_THREADS)
  // Handle the easy case first.
  if (timeout == 0)
    return OS::cond_wait (cv, external_mutex);
#   if defined (ACE_HAS_WTHREADS) || defined (ACE_VXWORKS)

  // Prevent race conditions on the <waiters_> count.
  OS::thread_mutex_lock (&cv->waiters_lock_);
  cv->waiters_++;
  OS::thread_mutex_unlock (&cv->waiters_lock_);

  int result = 0;
  ACE_Errno_Guard error (errno, 0);
  int msec_timeout;

  if (timeout->sec () == 0 && timeout->usec () == 0)
    msec_timeout = 0; // Do a "poll."
  else
    {
      // Note that we must convert between absolute time (which is
      // passed as a parameter) and relative time (which is what
      // WaitForSingleObjects() expects).
      TimeValue relative_time (*timeout - OS::gettimeofday ());

      // Watchout for situations where a context switch has caused the
      // current time to be > the timeout.
      if (relative_time < TimeValue::zero)
        msec_timeout = 0;
      else
        msec_timeout = relative_time.msec ();
    }

#     if defined (ACE_HAS_SIGNAL_OBJECT_AND_WAIT)
  if (external_mutex->type_ == USYNC_PROCESS)
    // This call will automatically release the mutex and wait on the
    // semaphore.
    result = ::SignalObjectAndWait (external_mutex->proc_mutex_,
                                    cv->sema_,
                                    msec_timeout,
                                    FALSE);
  else
#     endif /* ACE_HAS_SIGNAL_OBJECT_AND_WAIT */
    {
      // We keep the lock held just long enough to increment the count
      // of waiters by one.  Note that we can't keep it held across
      // the call to WaitForSingleObject since that will deadlock
      // other calls to OS::cond_signal().
      if (OS::mutex_unlock (external_mutex) != 0)
        return -1;

      // Wait to be awakened by a OS::signal() or
      // OS::broadcast().
#     if defined (ACE_WIN32)
#       if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
      result = ::WaitForSingleObject (cv->sema_, msec_timeout);
#       else /* ACE_USES_WINCE_SEMA_SIMULATION */
      // Can't use Win32 API on our simulated semaphores.
      result = OS::sema_wait (&cv->sema_,
                                  timeout);
#       endif /* ACE_USES_WINCE_SEMA_SIMULATION */
#     elif defined (ACE_VXWORKS)
      // Inline the call to OS::sema_wait () because it takes an
      // TimeValue argument.  Avoid the cost of that conversion . . .
      int ticks_per_sec = ::sysClkRateGet ();
      int ticks = msec_timeout * ticks_per_sec / ACE_ONE_SECOND_IN_MSECS;
      result = ::semTake (cv->sema_.sema_, ticks);
#     endif /* ACE_WIN32 || VXWORKS */
    }

  // Reacquire lock to avoid race conditions.
  OS::thread_mutex_lock (&cv->waiters_lock_);
  cv->waiters_--;

  int last_waiter = cv->was_broadcast_ && cv->waiters_ == 0;

  OS::thread_mutex_unlock (&cv->waiters_lock_);

#     if defined (ACE_WIN32)
  if (result != WAIT_OBJECT_0)
    {
      switch (result)
        {
        case WAIT_TIMEOUT:
          error = ETIME;
          break;
        default:
          error = ::GetLastError ();
          break;
        }
      result = -1;
    }
#     elif defined (ACE_VXWORKS)
  if (result == ERROR)
    {
      switch (errno)
        {
        case S_objLib_OBJ_TIMEOUT:
          error = ETIME;
          break;
        case S_objLib_OBJ_UNAVAILABLE:
          if (msec_timeout == 0)
            error = ETIME;
          break;
        default:
          error = errno;
          break;
        }
      result = -1;
    }
#     endif /* ACE_WIN32 || VXWORKS */
#     if defined (ACE_HAS_SIGNAL_OBJECT_AND_WAIT)
  if (external_mutex->type_ == USYNC_PROCESS)
    {
      if (last_waiter)
        // This call atomically signals the <waiters_done_> event and
        // waits until it can acquire the mutex.  This is important to
        // prevent unfairness.
        ACE_WIN32CALL (ACE_ADAPT_RETVAL (::SignalObjectAndWait (cv->waiters_done_,
                                                                external_mutex->proc_mutex_,
                                                                INFINITE, FALSE),
                                         result),
                       int, -1, result);
      else
        // We must always regain the <external_Mutex>, even when
        // errors occur because that's the guarantee that we give to
        // our callers.
        OS::mutex_lock (external_mutex);

      return result;
      /* NOTREACHED */
    }
#     endif /* ACE_HAS_SIGNAL_OBJECT_AND_WAIT */
  // Note that this *must* be an "if" statement rather than an "else
  // if" statement since the caller may have timed out and hence the
  // result would have been -1 above.
  if (last_waiter)
    // Release the signaler/broadcaster if we're the last waiter.
#     if defined (ACE_WIN32)
    OS::event_signal (&cv->waiters_done_);
#     else
    OS::sema_post (&cv->waiters_done_);
#     endif /* ACE_WIN32 */

  // We must always regain the <external_mutex>, even when errors
  // occur because that's the guarantee that we give to our callers.
  OS::mutex_lock (external_mutex);

  return result;
#   endif /* ACE_HAS_WTHREADS || ACE_HAS_VXWORKS */
# else
  ACE_UNUSED_ARG (cv);
  ACE_UNUSED_ARG (external_mutex);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

# if defined (ACE_HAS_WTHREADS)
int
OS::cond_timedwait (ACE_cond_t *cv,
                        ACE_thread_mutex_t *external_mutex,
                        TimeValue *timeout)
{
  OS_TRACE ("OS::cond_timedwait");
#   if defined (ACE_HAS_THREADS)
  // Handle the easy case first.
  if (timeout == 0)
    return OS::cond_wait (cv, external_mutex);

  // Prevent race conditions on the <waiters_> count.
  OS::thread_mutex_lock (&cv->waiters_lock_);
  cv->waiters_++;
  OS::thread_mutex_unlock (&cv->waiters_lock_);

  int result = 0;
  int error = 0;
  int msec_timeout;

  if (timeout->sec () == 0 && timeout->usec () == 0)
    msec_timeout = 0; // Do a "poll."
  else
    {
      // Note that we must convert between absolute time (which is
      // passed as a parameter) and relative time (which is what
      // WaitForSingleObjects() expects).
      TimeValue relative_time (*timeout - OS::gettimeofday ());

      // Watchout for situations where a context switch has caused the
      // current time to be > the timeout.
      if (relative_time < TimeValue::zero)
        msec_timeout = 0;
      else
        msec_timeout = relative_time.msec ();
    }

  // We keep the lock held just long enough to increment the count of
  // waiters by one.  Note that we can't keep it held across the call
  // to WaitForSingleObject since that will deadlock other calls to
  // OS::cond_signal().
  if (OS::thread_mutex_unlock (external_mutex) != 0)
    return -1;

  // Wait to be awakened by a OS::signal() or OS::broadcast().
#     if defined (ACE_USES_WINCE_SEMA_SIMULATION)
  // Can't use Win32 API on simulated semaphores.
  result = OS::sema_wait (&cv->sema_,
                              timeout);

  if (result == -1 && errno == ETIME)
    result = WAIT_TIMEOUT;
#     else
  result = ::WaitForSingleObject (cv->sema_, msec_timeout);
#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */

  // Reacquire lock to avoid race conditions.
  OS::thread_mutex_lock (&cv->waiters_lock_);

  cv->waiters_--;

  int last_waiter = cv->was_broadcast_ && cv->waiters_ == 0;

  OS::thread_mutex_unlock (&cv->waiters_lock_);

  if (result != WAIT_OBJECT_0)
    {
      switch (result)
        {
        case WAIT_TIMEOUT:
          error = ETIME;
          break;
        default:
          error = ::GetLastError ();
          break;
        }
      result = -1;
    }

  if (last_waiter)
    // Release the signaler/broadcaster if we're the last waiter.
    OS::event_signal (&cv->waiters_done_);

  // We must always regain the <external_mutex>, even when errors
  // occur because that's the guarantee that we give to our callers.
  OS::thread_mutex_lock (external_mutex);
  errno = error;
  return result;
#   else
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_THREADS */
}

int
OS::cond_wait (ACE_cond_t *cv,
                   ACE_thread_mutex_t *external_mutex)
{
  OS_TRACE ("OS::cond_wait");
#   if defined (ACE_HAS_THREADS)
  OS::thread_mutex_lock (&cv->waiters_lock_);
  cv->waiters_++;
  OS::thread_mutex_unlock (&cv->waiters_lock_);

  int result = 0;
  int error = 0;

  // We keep the lock held just long enough to increment the count of
  // waiters by one.  Note that we can't keep it held across the call
  // to OS::sema_wait() since that will deadlock other calls to
  // OS::cond_signal().
  if (OS::thread_mutex_unlock (external_mutex) != 0)
    return -1;

  // Wait to be awakened by a OS::cond_signal() or
  // OS::cond_broadcast().
#     if !defined (ACE_USES_WINCE_SEMA_SIMULATION)
  result = ::WaitForSingleObject (cv->sema_, INFINITE);
#     else
  // Can't use Win32 API on simulated semaphores.
  result = OS::sema_wait (&cv->sema_);

  if (result != WAIT_OBJECT_0 && errno == ETIME)
    result = WAIT_TIMEOUT;

#     endif /* ACE_USES_WINCE_SEMA_SIMULATION */

  // Reacquire lock to avoid race conditions.
  OS::thread_mutex_lock (&cv->waiters_lock_);

  cv->waiters_--;

  int last_waiter = cv->was_broadcast_ && cv->waiters_ == 0;

  OS::thread_mutex_unlock (&cv->waiters_lock_);

  if (result != WAIT_OBJECT_0)
    {
      switch (result)
        {
        case WAIT_TIMEOUT:
          error = ETIME;
          break;
        default:
          error = ::GetLastError ();
          break;
        }
    }
  else if (last_waiter)
    // Release the signaler/broadcaster if we're the last waiter.
    OS::event_signal (&cv->waiters_done_);

  // We must always regain the <external_mutex>, even when errors
  // occur because that's the guarantee that we give to our callers.
  OS::thread_mutex_lock (external_mutex);

  // Reset errno in case mutex_lock() also fails...
  errno = error;
  return result;
#   else
  ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_THREADS */
}
# endif /* ACE_HAS_WTHREADS */
#else
int
OS::cond_init (ACE_cond_t *cv, short type, const char *name, void *arg)
{
  ACE_condattr_t attributes;
  if (OS::condattr_init (attributes, type) == 0
      && OS::cond_init (cv, attributes, name, arg) == 0)
    {
      (void) OS::condattr_destroy (attributes);
      return 0;
    }
  return -1;
}
#endif /* ACE_LACKS_COND_T */

/*****************************************************************************/
// CONDITIONS END
/*****************************************************************************/

/*****************************************************************************/
// MUTEXES BEGIN
/*****************************************************************************/

int
OS::mutex_init (ACE_mutex_t *m,
                    int lock_scope,
                    const char *name,
                    ACE_mutexattr_t *attributes,
                    LPSECURITY_ATTRIBUTES sa,
                    int lock_type)
{
  // OS_TRACE ("OS::mutex_init");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (sa);

# if defined (ACE_VXWORKS) && (ACE_VXWORKS >= 0x600) && (ACE_VXWORKS <= 0x620)
  /* Tests show that VxWorks 6.x pthread lib does not only
   * require zeroing of mutex/condition objects to function correctly
   * but also of the attribute objects.
   */
  pthread_mutexattr_t l_attributes = {0};
# else
  pthread_mutexattr_t l_attributes;
# endif

  if (attributes == 0)
    attributes = &l_attributes;
  int result = 0;
  int attr_init = 0;  // have we initialized the local attributes.

  // Only do these initializations if the <attributes> parameter
  // wasn't originally set.
  if (attributes == &l_attributes)
  {
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
      if (::pthread_mutexattr_create (attributes) == 0)
#   elif defined (ACE_HAS_PTHREADS_DRAFT7) || defined (ACE_HAS_PTHREADS_STD)
      if (ACE_ADAPT_RETVAL (::pthread_mutexattr_init (attributes), result) == 0)
#   else /* draft 6 */
      if (::pthread_mutexattr_init (attributes) == 0)
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
{
  result = 0;
  attr_init = 1; // we have initialized these attributes
}
      else
        result = -1;        // ACE_ADAPT_RETVAL used it for intermediate status
  }

  if (result == 0 && lock_scope != 0)
{
#   if defined (ACE_HAS_PTHREADS_DRAFT7) || defined (ACE_HAS_PTHREADS_STD)
#     if defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_MUTEXATTR_PSHARED)
      (void) ACE_ADAPT_RETVAL (::pthread_mutexattr_setpshared (attributes,
                                                               lock_scope),
                               result);
#     endif /* _POSIX_THREAD_PROCESS_SHARED && !ACE_LACKS_MUTEXATTR_PSHARED */
#   else /* Pthreads draft 6 */
#     if !defined (ACE_LACKS_MUTEXATTR_PSHARED)
      if (::pthread_mutexattr_setpshared (attributes, lock_scope) != 0)
        result = -1;
#     endif /* ACE_LACKS_MUTEXATTR_PSHARED */
#   endif /* ACE_HAS_PTHREADS_DRAFT7 || ACE_HAS_PTHREADS_STD */
}

  if (result == 0 && lock_type != 0)
{
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
#     if defined (ACE_HAS_PTHREAD_MUTEXATTR_SETKIND_NP)
      if (::pthread_mutexattr_setkind_np (attributes, lock_type) != 0)
        result = -1;
#     endif /* ACE_HAS_PTHREAD_MUTEXATTR_SETKIND_NP */
#   elif defined (ACE_HAS_RECURSIVE_MUTEXES)
      (void) ACE_ADAPT_RETVAL (::pthread_mutexattr_settype (attributes,
                                                            lock_type),
                               result);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
}

  if (result == 0)
{
#   if defined (ACE_VXWORKS)&& (ACE_VXWORKS >= 0x600) && (ACE_VXWORKS <= 0x620)
      /* VxWorks 6.x API reference states:
       * If the memory for the mutex variable object has been allocated
       *   dynamically, it is a good policy to always zero out the
       *   block of memory so as to avoid spurious EBUSY return code
       *   when calling this routine.
       * Tests shows this to be necessary.
       */
      OS::memset (m, 0, sizeof (*m));
#   endif
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
      if (::pthread_mutex_init (m, *attributes) == 0)
#   elif defined (ACE_HAS_PTHREADS_DRAFT7) || defined (ACE_HAS_PTHREADS_STD)
      if (ACE_ADAPT_RETVAL (::pthread_mutex_init (m, attributes), result) == 0)
#   else
      if (::pthread_mutex_init (m, attributes) == 0)
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
        result = 0;
      else
        result = -1;        // ACE_ADAPT_RETVAL used it for intermediate status
}

  // Only do the deletions if the <attributes> parameter wasn't
  // originally set.
  if (attributes == &l_attributes && attr_init)
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  ::pthread_mutexattr_delete (&l_attributes);
#   else
  ::pthread_mutexattr_destroy (&l_attributes);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */

  return result;
# elif defined (ACE_HAS_STHREADS)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (sa);
  ACE_UNUSED_ARG (lock_type);
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::mutex_init (m,
                     lock_scope,
                     attributes),
  result),
  int, -1);
# elif defined (ACE_HAS_WTHREADS)
  m->type_ = lock_scope;

  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  switch (lock_scope)
{
  case USYNC_PROCESS:
#   if defined (ACE_HAS_WINCE)
      // @@todo (brunsch) This idea should be moved into OS_Win32.
      m->proc_mutex_ =
  ::CreateMutexW (OS::default_win32_security_attributes_r
                          (sa, &sa_buffer, &sd_buffer),
                        FALSE,
                        ACE_Ascii_To_Wide (name).wchar_rep ());
#   else /* ACE_HAS_WINCE */
      m->proc_mutex_ =
  ::CreateMutexA (OS::default_win32_security_attributes_r
                          (sa, &sa_buffer, &sd_buffer),
                        FALSE,
                        name);
#   endif /* ACE_HAS_WINCE */
      if (m->proc_mutex_ == 0)
        ACE_FAIL_RETURN (-1);
      else
      {
          // Make sure to set errno to ERROR_ALREADY_EXISTS if necessary.
        OS::set_errno_to_last_error ();
        return 0;
      }
  case USYNC_THREAD:
    return OS::thread_mutex_init (&m->thr_mutex_,
                                       lock_type,
                                       name,
                                       attributes);
  default:
    errno = EINVAL;
    return -1;
}
  /* NOTREACHED */

# elif defined (ACE_VXWORKS)
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (sa);
  ACE_UNUSED_ARG (lock_type);

  return (*m = ::semMCreate (lock_scope)) == 0 ? -1 : 0;
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (lock_scope);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (attributes);
  ACE_UNUSED_ARG (sa);
  ACE_UNUSED_ARG (lock_type);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

int
OS::mutex_destroy (ACE_mutex_t *m)
{
  OS_TRACE ("OS::mutex_destroy");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
#   if (defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6))
  OSCALL_RETURN (::pthread_mutex_destroy (m), int, -1);
#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::pthread_mutex_destroy (m),
                     result), int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6*/
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::mutex_destroy (m), result), int, -1);
# elif defined (ACE_HAS_WTHREADS)
  switch (m->type_)
{
  case USYNC_PROCESS:
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CloseHandle (m->proc_mutex_),
                          ace_result_),
    int, -1);
  case USYNC_THREAD:
    return OS::thread_mutex_destroy (&m->thr_mutex_);
  default:
    errno = EINVAL;
    return -1;
}
  /* NOTREACHED */
# elif defined (ACE_VXWORKS)
  return ::semDelete (*m) == OK ? 0 : -1;
# endif /* Threads variety case */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_WCHAR)
int
OS::mutex_init (ACE_mutex_t *m,
                    int lock_scope,
                    const wchar_t *name,
                    ACE_mutexattr_t *attributes,
                    LPSECURITY_ATTRIBUTES sa,
                    int lock_type)
{
#if defined (ACE_HAS_THREADS) && defined (ACE_HAS_WTHREADS)
  m->type_ = lock_scope;
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
  switch (lock_scope)
  {
    case USYNC_PROCESS:
      m->proc_mutex_ =
      ::CreateMutexW (OS::default_win32_security_attributes_r
          (sa, &sa_buffer, &sd_buffer),
      FALSE,
      name);
      if (m->proc_mutex_ == 0)
        ACE_FAIL_RETURN (-1);
      else
        return 0;
    case USYNC_THREAD:
      return OS::thread_mutex_init (&m->thr_mutex_,
                                         lock_type,
                                         name,
                                         attributes);
  }

  errno = EINVAL;
  return -1;
#else /* ACE_HAS_THREADS && ACE_HAS_WTHREADS */
  return OS::mutex_init (m,
                             lock_scope,
                             ACE_Wide_To_Ascii (name).char_rep (),
                             attributes,
                             sa,
                             lock_type);
#endif /* ACE_HAS_THREADS && ACE_HAS_WTHREADS */
}
#endif /* ACE_HAS_WCHAR */

int
OS::mutex_lock (ACE_mutex_t *m)
{
  // OS_TRACE ("OS::mutex_lock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
#   if (defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6))
  OSCALL_RETURN (pthread_mutex_lock (m), int, -1);
#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_mutex_lock (m), result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::mutex_lock (m), result), int, -1);
# elif defined (ACE_HAS_WTHREADS)
  switch (m->type_)
{
  case USYNC_PROCESS:
    switch (::WaitForSingleObject (m->proc_mutex_, INFINITE))
      {
        //
        // Timeout can't occur, so don't bother checking...
        //
      case WAIT_OBJECT_0:
      case WAIT_ABANDONED:
        // We will ignore abandonments in this method
        // Note that we still hold the lock
        return 0;
      default:
        // This is a hack, we need to find an appropriate mapping...
        OS::set_errno_to_last_error ();
        return -1;
      }
  case USYNC_THREAD:
    return OS::thread_mutex_lock (&m->thr_mutex_);
  default:
    errno = EINVAL;
    return -1;
}
  /* NOTREACHED */
# elif defined (ACE_VXWORKS)
  return ::semTake (*m, WAIT_FOREVER) == OK ? 0 : -1;
# endif /* Threads variety case */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

int
OS::mutex_lock (ACE_mutex_t *m,
                    int &abandoned)
{
  OS_TRACE ("OS::mutex_lock");
#if defined (ACE_HAS_THREADS) && defined (ACE_HAS_WTHREADS)
  abandoned = 0;
  switch (m->type_)
  {
    case USYNC_PROCESS:
      switch (::WaitForSingleObject (m->proc_mutex_, INFINITE))
      {
        //
          // Timeout can't occur, so don't bother checking...
        //
        case WAIT_OBJECT_0:
          return 0;
        case WAIT_ABANDONED:
          abandoned = 1;
          return 0;  // something goofed, but we hold the lock ...
        default:
          // This is a hack, we need to find an appropriate mapping...
          OS::set_errno_to_last_error ();
          return -1;
      }
    case USYNC_THREAD:
      return OS::thread_mutex_lock (&m->thr_mutex_);
    default:
      errno = EINVAL;
      return -1;
  }
  /* NOTREACHED */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (abandoned);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS and ACE_HAS_WTHREADS */
}

int
OS::mutex_lock (ACE_mutex_t *m,
                    const TimeValue &timeout)
{
#if defined (ACE_HAS_THREADS) && defined (ACE_HAS_MUTEX_TIMEOUTS)

#  if defined (ACE_HAS_PTHREADS)
  int result;

  // "timeout" should be an absolute time.

  timespec_t ts = timeout;  // Calls TimeValue::operator timespec_t().

  // Note that the mutex should not be a recursive one, i.e., it
  // should only be a standard mutex or an error checking mutex.

  OSCALL (ACE_ADAPT_RETVAL (::pthread_mutex_timedlock (m, &ts), result), int, -1, result);

  // We need to adjust this to make the errno values consistent.
  if (result == -1 && errno == ETIMEDOUT)
    errno = ETIME;
  return result;

#  elif defined (ACE_HAS_WTHREADS)
  // Note that we must convert between absolute time (which is passed
  // as a parameter) and relative time (which is what the system call
  // expects).
  TimeValue relative_time (timeout - OS::gettimeofday ());

  switch (m->type_)
  {
    case USYNC_PROCESS:
      switch (::WaitForSingleObject (m->proc_mutex_,
                relative_time.msec ()))
      {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
          // We will ignore abandonments in this method
          // Note that we still hold the lock
          return 0;
        case WAIT_TIMEOUT:
          errno = ETIME;
          return -1;
        default:
          // This is a hack, we need to find an appropriate mapping...
          OS::set_errno_to_last_error ();
          return -1;
      }
    case USYNC_THREAD:
      ACE_NOTSUP_RETURN (-1);
    default:
      errno = EINVAL;
      return -1;
  }
  /* NOTREACHED */

#  elif defined (ACE_VXWORKS)

  // Note that we must convert between absolute time (which is passed
  // as a parameter) and relative time (which is what the system call
  // expects).
  TimeValue relative_time (timeout - OS::gettimeofday ());

  int ticks_per_sec = ::sysClkRateGet ();

  int ticks = relative_time.sec() * ticks_per_sec +
      relative_time.usec () * ticks_per_sec / ACE_ONE_SECOND_IN_USECS;
  if (::semTake (*m, ticks) == ERROR)
  {
    if (errno == S_objLib_OBJ_TIMEOUT)
        // Convert the VxWorks errno to one that's common for to ACE
        // platforms.
      errno = ETIME;
    else if (errno == S_objLib_OBJ_UNAVAILABLE)
      errno = EBUSY;
    return -1;
  }
  else
    return 0;
#  endif /* ACE_HAS_PTHREADS */

#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (timeout);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS && ACE_HAS_MUTEX_TIMEOUTS */
}

int
OS::mutex_trylock (ACE_mutex_t *m)
{
  OS_TRACE ("OS::mutex_trylock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
#   if (defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6))
  int status = pthread_mutex_trylock (m);
  if (status == 1)
    status = 0;
  else if (status == 0) {
    status = -1;
    errno = EBUSY;
  }
  return status;
#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_mutex_trylock (m), result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::mutex_trylock (m), result), int, -1);
# elif defined (ACE_HAS_WTHREADS)
  switch (m->type_)
{
  case USYNC_PROCESS:
  {
        // Try for 0 milliseconds - i.e. nonblocking.
    switch (::WaitForSingleObject (m->proc_mutex_, 0))
    {
      case WAIT_OBJECT_0:
        return 0;
      case WAIT_ABANDONED:
            // We will ignore abandonments in this method.  Note that
            // we still hold the lock.
        return 0;
      case WAIT_TIMEOUT:
        errno = EBUSY;
        return -1;
      default:
        OS::set_errno_to_last_error ();
        return -1;
    }
  }
  case USYNC_THREAD:
    return OS::thread_mutex_trylock (&m->thr_mutex_);
  default:
    errno = EINVAL;
    return -1;
}
  /* NOTREACHED */
# elif defined (ACE_VXWORKS)
  if (::semTake (*m, NO_WAIT) == ERROR)
    if (errno == S_objLib_OBJ_UNAVAILABLE)
{
        // couldn't get the semaphore
  errno = EBUSY;
  return -1;
}
    else
      // error
      return -1;
    else
    // got the semaphore
      return 0;
# endif /* Threads variety case */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

int
OS::mutex_trylock (ACE_mutex_t *m, int &abandoned)
{
#if defined (ACE_HAS_THREADS) && defined (ACE_HAS_WTHREADS)
  abandoned = 0;
  switch (m->type_)
  {
    case USYNC_PROCESS:
    {
        // Try for 0 milliseconds - i.e. nonblocking.
      switch (::WaitForSingleObject (m->proc_mutex_, 0))
      {
        case WAIT_OBJECT_0:
          return 0;
        case WAIT_ABANDONED:
          abandoned = 1;
          return 0;  // something goofed, but we hold the lock ...
        case WAIT_TIMEOUT:
          errno = EBUSY;
          return -1;
        default:
          OS::set_errno_to_last_error ();
          return -1;
      }
    }
    case USYNC_THREAD:
      return OS::thread_mutex_trylock (&m->thr_mutex_);
    default:
      errno = EINVAL;
      return -1;
  }
  /* NOTREACHED */
#else
  ACE_UNUSED_ARG (m);
  ACE_UNUSED_ARG (abandoned);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS and ACE_HAS_WTHREADS */
}

int
OS::mutex_unlock (ACE_mutex_t *m)
{
  OS_TRACE ("OS::mutex_unlock");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  // Note, don't use "::" here since the following call is often a macro.
#   if (defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6))
  OSCALL_RETURN (pthread_mutex_unlock (m), int, -1);
#   else
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_mutex_unlock (m), result),
                     int, -1);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 || ACE_HAS_PTHREADS_DRAFT6 */
# elif defined (ACE_HAS_STHREADS)
  int result;
  OSCALL_RETURN (ACE_ADAPT_RETVAL (::mutex_unlock (m), result), int, -1);
# elif defined (ACE_HAS_WTHREADS)
  switch (m->type_)
{
  case USYNC_PROCESS:
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::ReleaseMutex (m->proc_mutex_),
                          ace_result_),
    int, -1);
  case USYNC_THREAD:
    return OS::thread_mutex_unlock (&m->thr_mutex_);
  default:
    errno = EINVAL;
    return -1;
}
  /* NOTREACHED */
# elif defined (ACE_VXWORKS)
  return ::semGive (*m) == OK ? 0 : -1;
# endif /* Threads variety case */
#else
  ACE_UNUSED_ARG (m);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

void
OS::mutex_lock_cleanup (void *mutex)
{
  OS_TRACE ("OS::mutex_lock_cleanup");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
  ACE_mutex_t *p_lock = (ACE_mutex_t *) mutex;
  OS::mutex_unlock (p_lock);
# else
  ACE_UNUSED_ARG (mutex);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (mutex);
#endif /* ACE_HAS_THREADS */
}

/*****************************************************************************/
// MUTEXES END
/*****************************************************************************/

/*****************************************************************************/
// EVENTS BEGIN
/*****************************************************************************/
/*****************************************************************************/
// MUTEXES END
/*****************************************************************************/

/*****************************************************************************/
// EVENTS BEGIN
/*****************************************************************************/

int
OS::event_destroy (ACE_event_t *event)
{
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::CloseHandle (*event), ace_result_), int, -1);
#elif defined (ACE_HAS_THREADS)
  if (event->eventdata_)
    {
      // mutex_destroy()/cond_destroy() are called in a loop if the object
      // is BUSY.  This avoids conditions where we fail to destroy these
      // objects because at time of destroy they were just being used in
      // another thread possibly causing deadlocks later on if they keep
      // being used after we're gone.

      if (event->eventdata_->type_ == USYNC_PROCESS)
        {
          if (event->name_)
            {
              // Only destroy the event data if we're the ones who initialized
              // it.

              int r1, r2;
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
              // First destroy the mutex so locking after this will return
              // errors.
              while ((r1 = OS::mutex_destroy (&event->eventdata_->lock_)) == -1
                     && errno == EBUSY)
                {
                  OS::thr_yield ();
                }
# else
              r1 = OS::sema_destroy(&event->lock_);
# endif

# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
              // Now fix event to manual reset, raise signal and broadcast
              // until is's possible to destroy the condition.
              event->eventdata_->manual_reset_ = 1;
              while ((r2 = OS::cond_destroy (&event->eventdata_->condition_)) == -1
                     && errno == EBUSY)
                {
                  event->eventdata_->is_signaled_ = 1;
                  OS::cond_broadcast (&event->eventdata_->condition_);
                  OS::thr_yield ();
                }
# else
              r2 = OS::sema_destroy(&event->semaphore_);
# endif
              /*OS::munmap (event->eventdata_,
                              sizeof (ACE_eventdata_t));
              OS::shm_unlink (ACE_TEXT_CHAR_TO_TCHAR(event->name_)); *///comment by qinghua
              OS::free (event->name_);
              return r1 != 0 || r2 != 0 ? -1 : 0;
            }
          else
            {/*
              OS::munmap (event->eventdata_,
                              sizeof (ACE_eventdata_t)); *///comment by qinghua
# if (!defined (ACE_HAS_PTHREADS) || !defined (_POSIX_THREAD_PROCESS_SHARED) || \
        (defined (ACE_LACKS_MUTEXATTR_PSHARED) && defined (ACE_LACKS_CONDATTR_PSHARED))) && \
     (defined (ACE_USES_FIFO_SEM) || \
        (defined (ACE_HAS_POSIX_SEM) && defined (ACE_HAS_POSIX_SEM_TIMEOUT) && defined (ACE_LACKS_NAMED_POSIX_SEM)))
              OS::sema_destroy(&event->lock_);
# endif
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
              return 0;
# else
              return OS::sema_destroy(&event->semaphore_);
# endif
            }
        }
      else
        {
          int r1, r2;
          // First destroy the mutex so locking after this will return errors.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
      // first destroy the mutex so locking after this will return errors
          while ((r1 = OS::mutex_destroy (&event->eventdata_->lock_)) == -1
                 && errno == EBUSY)
            {
              OS::thr_yield ();
            }
# else
          r1 = OS::sema_destroy(&event->lock_);
# endif

# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
          // Now fix event to manual reset, raise signal and broadcast until
          // it's possible to destroy the condition.
          event->eventdata_->manual_reset_ = 1;
          while ((r2 = OS::cond_destroy (&event->eventdata_->condition_)) == -1
                 && errno == EBUSY)
            {
              event->eventdata_->is_signaled_ = 1;
              OS::cond_broadcast (&event->eventdata_->condition_);
              OS::thr_yield ();
            }
# else
          r2 = OS::sema_destroy(&event->semaphore_);
# endif
          delete event->eventdata_;
          return r1 != 0 || r2 != 0 ? -1 : 0;
        }
    }

  return 0;
#else
  ACE_UNUSED_ARG (event);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

int
OS::event_init (ACE_event_t *event,
                    int manual_reset,
                    int initial_state,
                    int type,
                    const char *name,
                    void *arg,
                    LPSECURITY_ATTRIBUTES sa)
{
#if defined (ACE_WIN32)
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (arg);
  SECURITY_ATTRIBUTES sa_buffer;
  SECURITY_DESCRIPTOR sd_buffer;
# if defined (ACE_HAS_WINCE)
  // @@todo (brunsch) This idea should be moved into OS_Win32.
  *event = ::CreateEventW (OS::default_win32_security_attributes_r
                             (sa, &sa_buffer, &sd_buffer),
                           manual_reset,
                           initial_state,
                           ACE_Ascii_To_Wide (name).wchar_rep ());
# else /* ACE_HAS_WINCE */
  *event = ::CreateEventA (OS::default_win32_security_attributes_r
                             (sa, &sa_buffer, &sd_buffer),
                           manual_reset,
                           initial_state,
                           name);
# endif /* ACE_HAS_WINCE */
  if (*event == 0)
    ACE_FAIL_RETURN (-1);
  else
    return 0;
#elif defined (ACE_HAS_THREADS)
  ACE_UNUSED_ARG (sa);
  event->eventdata_ = 0;
  ACE_eventdata_t* evtdata;

  if (type == USYNC_PROCESS)
    {
	  return -1;
    }
  else
    {
      ACE_NEW_RETURN (evtdata, ACE_eventdata_t, -1);
      event->name_ = 0;
      event->eventdata_ = evtdata;
      event->eventdata_->type_ = type;
      event->eventdata_->manual_reset_ = manual_reset;
      event->eventdata_->is_signaled_ = initial_state;
      event->eventdata_->auto_event_signaled_ = false;
      event->eventdata_->waiting_threads_ = 0;
      event->eventdata_->signal_count_ = 0;

# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
      int result = OS::cond_init (&event->eventdata_->condition_,
                                      static_cast<short> (type),
                                      name,
                                      arg);
# else
      int result = OS::sema_init (&event->semaphore_,
                                      0,
                                      type,
                                      name,
                                      arg);
# endif
      if (result == 0)
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        result = OS::mutex_init (&event->eventdata_->lock_,
                                     type,
                                     name,
                                     (ACE_mutexattr_t *) arg);
# else
      result = OS::sema_init (&event->lock_,
                                  0,
                                  type,
                                  name,
                                  arg);
      if (result == 0)
        result = OS::sema_post(&event->lock_);    /* initially unlock */
# endif

      return result;
    }
#else
  ACE_UNUSED_ARG (event);
  ACE_UNUSED_ARG (manual_reset);
  ACE_UNUSED_ARG (initial_state);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_UNUSED_ARG (sa);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

int
OS::event_pulse (ACE_event_t *event)
{
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::PulseEvent (*event), ace_result_), int, -1);
#elif defined (ACE_HAS_THREADS)
  int result = 0;
  int error = 0;

  // grab the lock first
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
  if (OS::mutex_lock (&event->eventdata_->lock_) == 0)
# else
  if (OS::sema_wait (&event->lock_) == 0)
# endif
  {
    if (event->eventdata_->waiting_threads_ > 0)
    {
      // Manual-reset event.
      if (event->eventdata_->manual_reset_ == 1)
      {
        // Wakeup all waiters.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::cond_broadcast (&event->eventdata_->condition_) != 0)
        {
          result = -1;
          error = errno;
        }
        if (result == 0)
          event->eventdata_->signal_count_ = event->eventdata_->waiting_threads_;
# else
        event->eventdata_->signal_count_ = event->eventdata_->waiting_threads_;
        for (unsigned long i=0; i<event->eventdata_->signal_count_ ;++i)
          if (OS::sema_post(&event->semaphore_) != 0)
          {
            event->eventdata_->signal_count_ = 0;
            result = -1;
            error = errno;
          }

        if (result == 0)
          while(event->eventdata_->signal_count_!=0 && event->eventdata_->waiting_threads_!=0)
            OS::thr_yield ();
# endif
      }
      // Auto-reset event: wakeup one waiter.
      else
      {
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::cond_signal (&event->eventdata_->condition_) != 0)
# else
        if (OS::sema_post(&event->semaphore_) != 0)
# endif
        {
          result = -1;
          error = errno;
        }

        event->eventdata_->auto_event_signaled_ = true;
      }
    }

    // Reset event.
    event->eventdata_->is_signaled_ = 0;

    // Now we can let go of the lock.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
    OS::mutex_unlock (&event->eventdata_->lock_);
# else
    OS::sema_post (&event->lock_);
# endif
    if (result == -1)
      // Reset errno in case mutex_unlock() also fails...
      errno = error;
  }
  else
    result = -1;
  return result;
#else
  ACE_UNUSED_ARG (event);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

int
OS::event_reset (ACE_event_t *event)
{
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::ResetEvent (*event), ace_result_), int, -1);
#elif defined (ACE_HAS_THREADS)
  int result = 0;

  // Grab the lock first.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
  if (OS::mutex_lock (&event->eventdata_->lock_) == 0)
# else
  if (OS::sema_wait (&event->lock_) == 0)
# endif
  {
    // Reset event.
    event->eventdata_->is_signaled_ = 0;
    event->eventdata_->auto_event_signaled_ = false;

    // Now we can let go of the lock.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
    OS::mutex_unlock (&event->eventdata_->lock_);
# else
    OS::sema_post (&event->lock_);
# endif
  }
  else
    result = -1;
  return result;
#else
  ACE_UNUSED_ARG (event);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

int
OS::event_signal (ACE_event_t *event)
{
#if defined (ACE_WIN32)
  ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::SetEvent (*event), ace_result_), int, -1);
#elif defined (ACE_HAS_THREADS)
  int result = 0;
  int error = 0;

  // grab the lock first
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
  if (OS::mutex_lock (&event->eventdata_->lock_) == 0)
# else
  if (OS::sema_wait (&event->lock_) == 0)
# endif
  {
    // Manual-reset event.
    if (event->eventdata_->manual_reset_ == 1)
    {
      // wakeup all
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
      if (OS::cond_broadcast (&event->eventdata_->condition_) != 0)
      {
        result = -1;
        error = errno;
      }
# else
      if (OS::sema_post(&event->semaphore_) != 0)
      {
        result = -1;
        error = errno;
      }
# endif

      if (result == 0)
        // signal event
        event->eventdata_->is_signaled_ = 1;
    }
    // Auto-reset event
    else
    {
      if (event->eventdata_->waiting_threads_ == 0)
        // No waiters: signal event.
        event->eventdata_->is_signaled_ = 1;
      // Waiters: wakeup one waiter.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
      else if (OS::cond_signal (&event->eventdata_->condition_) != 0)
# else
      else if (OS::sema_post(&event->semaphore_) != 0)
# endif
      {
        result = -1;
        error = errno;
      }

      event->eventdata_->auto_event_signaled_ = true;
    }

    // Now we can let go of the lock.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
    OS::mutex_unlock (&event->eventdata_->lock_);
# else
    OS::sema_post (&event->lock_);
# endif

    if (result == -1)
      // Reset errno in case mutex_unlock() also fails...
      errno = error;
  }
  else
    result = -1;

  return result;
#else
  ACE_UNUSED_ARG (event);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

int
OS::event_timedwait (ACE_event_t *event,
                         TimeValue *timeout,
                         int use_absolute_time)
{
#if defined (ACE_WIN32)
  DWORD result;

  if (timeout == 0)
    // Wait forever
    result = ::WaitForSingleObject (*event, INFINITE);
  else if (timeout->sec () == 0 && timeout->usec () == 0)
    // Do a "poll".
    result = ::WaitForSingleObject (*event, 0);
  else
  {
      // Wait for upto <relative_time> number of milliseconds.  Note
      // that we must convert between absolute time (which is passed
      // as a parameter) and relative time (which is what
      // WaitForSingleObjects() expects).
      // <timeout> parameter is given in absolute or relative value
      // depending on parameter <use_absolute_time>.
    int msec_timeout;
    if (use_absolute_time)
    {
          // Time is given in absolute time, we should use
          // gettimeofday() to calculate relative time
      TimeValue relative_time (*timeout - OS::gettimeofday ());

          // Watchout for situations where a context switch has caused
          // the current time to be > the timeout.  Thanks to Norbert
          // Rapp <NRapp@nexus-informatics.de> for pointing this.
      if (relative_time < TimeValue::zero)
        msec_timeout = 0;
      else
        msec_timeout = relative_time.msec ();
    }
    else
         // time is given in relative time, just convert it into
         // milliseconds and use it
      msec_timeout = timeout->msec ();
    result = ::WaitForSingleObject (*event, msec_timeout);
  }

  switch (result)
  {
    case WAIT_OBJECT_0:
      return 0;
    case WAIT_TIMEOUT:
      errno = ETIME;
      return -1;
    default:
      // This is a hack, we need to find an appropriate mapping...
      OS::set_errno_to_last_error ();
      return -1;
  }
#elif defined (ACE_HAS_THREADS)
  int result = 0;
  int error = 0;

  // grab the lock first
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
  if (OS::mutex_lock (&event->eventdata_->lock_) == 0)
# else
  if (OS::sema_wait (&event->lock_) == 0)
# endif
  {
    if (event->eventdata_->is_signaled_ == 1)
    // event is currently signaled
    {
      if (event->eventdata_->manual_reset_ == 0)
      {
        // AUTO: reset state
        event->eventdata_->is_signaled_ = 0;
        event->eventdata_->auto_event_signaled_ = false;
      }
    }
    else
    // event is currently not signaled
    {
      event->eventdata_->waiting_threads_++;

      TimeValue absolute_timeout = *timeout;

      // cond_timewait() expects absolute time, check
      // <use_absolute_time> flag.
      if (use_absolute_time == 0)
        absolute_timeout += OS::gettimeofday ();

      while (event->eventdata_->is_signaled_ == 0 &&
             event->eventdata_->auto_event_signaled_ == false)
      {
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::cond_timedwait (&event->eventdata_->condition_,
                                    &event->eventdata_->lock_,
                                    &absolute_timeout) != 0)
        {
          result = -1;
          error = errno;
          break;
        }

        if (event->eventdata_->signal_count_ > 0)
        {
          event->eventdata_->signal_count_--;
          break;
        }
# else
#   if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_MUTEXATTR_PSHARED)) || \
      (!defined (ACE_USES_FIFO_SEM) && (!defined (ACE_HAS_POSIX_SEM) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::mutex_unlock (&event->eventdata_->lock_) != 0)
#   else
        if (OS::sema_post (&event->lock_) != 0)
#   endif
        {
          event->eventdata_->waiting_threads_--;
          return -1;
        }

        if (OS::sema_wait(&event->semaphore_, absolute_timeout) !=0)
        {
          result = -1;
          if (errno == ETIMEDOUT) // Semaphores time out with ETIMEDOUT (POSIX)
            error = ETIME;
          else
            error = errno;
        }

        bool signalled = false;
        if (result == 0 && event->eventdata_->signal_count_ > 0)
        {
          event->eventdata_->signal_count_--;
          signalled = true;
        }

#   if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_MUTEXATTR_PSHARED)) || \
      (!defined (ACE_USES_FIFO_SEM) && (!defined (ACE_HAS_POSIX_SEM) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::mutex_lock (&event->eventdata_->lock_) != 0)
#   else
        if (OS::sema_wait (&event->lock_) != 0)
#   endif
        {
          event->eventdata_->waiting_threads_--;  // yes, I know it's not save
          return -1;
        }

        if (result)
          break;

        if (event->eventdata_->manual_reset_ == 1 && event->eventdata_->is_signaled_ == 1)
          if (OS::sema_post(&event->semaphore_) != 0)
          {
            result = -1;
            error = errno;
            break;
          }

        if (signalled)
          break;
# endif
      }

      // Reset the auto_event_signaled_ to false now that we have
      // woken up.
      if (event->eventdata_->auto_event_signaled_ == true)
        event->eventdata_->auto_event_signaled_ = false;

      event->eventdata_->waiting_threads_--;
    }

    // Now we can let go of the lock.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
    OS::mutex_unlock (&event->eventdata_->lock_);
# else
    OS::sema_post (&event->lock_);
# endif

    if (result == -1)
      // Reset errno in case mutex_unlock() also fails...
      errno = error;
  }
  else
    result = -1;
  return result;
#else
  ACE_UNUSED_ARG (event);
  ACE_UNUSED_ARG (timeout);
  ACE_UNUSED_ARG (use_absolute_time);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

int
OS::event_wait (ACE_event_t *event)
{
#if defined (ACE_WIN32)
  switch (::WaitForSingleObject (*event, INFINITE))
{
  case WAIT_OBJECT_0:
    return 0;
  default:
    OS::set_errno_to_last_error ();
    return -1;
}
#elif defined (ACE_HAS_THREADS)
  int result = 0;
  int error = 0;

  // grab the lock first
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
  if (OS::mutex_lock (&event->eventdata_->lock_) == 0)
# else
  if (OS::sema_wait (&event->lock_) == 0)
# endif
  {
    if (event->eventdata_->is_signaled_ == 1)
    // Event is currently signaled.
    {
      if (event->eventdata_->manual_reset_ == 0)
        // AUTO: reset state
        event->eventdata_->is_signaled_ = 0;
    }
    else // event is currently not signaled
    {
      event->eventdata_->waiting_threads_++;

      while (event->eventdata_->is_signaled_ == 0 &&
             event->eventdata_->auto_event_signaled_ == false)
      {
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_CONDATTR_PSHARED)) || \
    (!defined (ACE_USES_FIFO_SEM) && \
      (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::cond_wait (&event->eventdata_->condition_,
                               &event->eventdata_->lock_) != 0)
        {
          result = -1;
          error = errno;
          // Something went wrong...
          break;
        }
        if (event->eventdata_->signal_count_ > 0)
        {
          event->eventdata_->signal_count_--;
          break;
        }
# else
#   if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_MUTEXATTR_PSHARED)) || \
      (!defined (ACE_USES_FIFO_SEM) && (!defined (ACE_HAS_POSIX_SEM) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::mutex_unlock (&event->eventdata_->lock_) != 0)
#   else
        if (OS::sema_post (&event->lock_) != 0)
#   endif
        {
          event->eventdata_->waiting_threads_--;
          return -1;
        }

        if (OS::sema_wait(&event->semaphore_) !=0)
        {
          result = -1;
          error = errno;
        }

        bool signalled = false;
        if (result == 0 && event->eventdata_->signal_count_ > 0)
        {
          event->eventdata_->signal_count_--;
          signalled = true;
        }

#   if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && !defined (ACE_LACKS_MUTEXATTR_PSHARED)) || \
      (!defined (ACE_USES_FIFO_SEM) && (!defined (ACE_HAS_POSIX_SEM) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
        if (OS::mutex_lock (&event->eventdata_->lock_) != 0)
#   else
        if (OS::sema_wait (&event->lock_) != 0)
#   endif
        {
          event->eventdata_->waiting_threads_--;
          return -1;
        }

        if (result)
          break;

        if (event->eventdata_->manual_reset_ == 1 && event->eventdata_->is_signaled_ == 1)
          if (OS::sema_post(&event->semaphore_) != 0)
          {
            result = -1;
            error = errno;
            break;
          }

        if (signalled)
          break;
# endif
    }

    // Reset it since we have woken up.
    if (event->eventdata_->auto_event_signaled_ == true)
      event->eventdata_->auto_event_signaled_ = false;

    event->eventdata_->waiting_threads_--;
  }

  // Now we can let go of the lock.
# if (defined (ACE_HAS_PTHREADS) && defined (_POSIX_THREAD_PROCESS_SHARED) && \
        (!defined (ACE_LACKS_MUTEXATTR_PSHARED) || !defined (ACE_LACKS_CONDATTR_PSHARED))) || \
     (!defined (ACE_USES_FIFO_SEM) && \
        (!defined (ACE_HAS_POSIX_SEM) || !defined (ACE_HAS_POSIX_SEM_TIMEOUT) || defined (ACE_LACKS_NAMED_POSIX_SEM)))
    OS::mutex_unlock (&event->eventdata_->lock_);
# else
    OS::sema_post (&event->lock_);
# endif

  if (result == -1)
    // Reset errno in case mutex_unlock() also fails...
    errno = error;
}
  else
    result = -1;
  return result;
#else
  ACE_UNUSED_ARG (event);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_WIN32 */
}

/*****************************************************************************/
// EVENTS END
/*****************************************************************************/

#if !defined (ACE_HAS_THREADS) || (defined (ACE_LACKS_RWLOCK_T) && \
                                   !defined (ACE_HAS_PTHREADS_UNIX98_EXT))
int
OS::rwlock_init (ACE_rwlock_t *rw,
                     int type,
                     const ACE_TCHAR *name,
                     void *arg)
{
  // OS_TRACE ("OS::rwlock_init");
# if defined (ACE_HAS_THREADS) && defined (ACE_LACKS_RWLOCK_T)
  // NT, POSIX, and VxWorks don't support this natively.
  ACE_UNUSED_ARG (name);
  int result = -1;

  // Since we cannot use the user specified name for all three
  // objects, we will create three completely new names.
  ACE_TCHAR name1[ACE_UNIQUE_NAME_LEN];
  ACE_TCHAR name2[ACE_UNIQUE_NAME_LEN];
  ACE_TCHAR name3[ACE_UNIQUE_NAME_LEN];
  ACE_TCHAR name4[ACE_UNIQUE_NAME_LEN];

  OS::unique_name ((const void *) &rw->lock_,
                       name1,
                       ACE_UNIQUE_NAME_LEN);
  OS::unique_name ((const void *) &rw->waiting_readers_,
                       name2,
                       ACE_UNIQUE_NAME_LEN);
  OS::unique_name ((const void *) &rw->waiting_writers_,
                       name3,
                       ACE_UNIQUE_NAME_LEN);
  OS::unique_name ((const void *) &rw->waiting_important_writer_,
                       name4,
                       ACE_UNIQUE_NAME_LEN);

  ACE_condattr_t attributes;
  if (OS::condattr_init (attributes, type) == 0)
    {
      if (OS::mutex_init (&rw->lock_, type, name1,
                              (ACE_mutexattr_t *) arg) == 0
          && OS::cond_init (&rw->waiting_readers_,
                                attributes, name2, arg) == 0
          && OS::cond_init (&rw->waiting_writers_,
                                attributes, name3, arg) == 0
          && OS::cond_init (&rw->waiting_important_writer_,
                                attributes, name4, arg) == 0)
        {
          // Success!
          rw->ref_count_ = 0;
          rw->num_waiting_writers_ = 0;
          rw->num_waiting_readers_ = 0;
          rw->important_writer_ = 0;
          result = 0;
        }
      OS::condattr_destroy (attributes);
    }

  if (result == -1)
    {
      // Save/restore errno.
      ACE_Errno_Guard error (errno);
      OS::mutex_destroy (&rw->lock_);
      OS::cond_destroy (&rw->waiting_readers_);
      OS::cond_destroy (&rw->waiting_writers_);
      OS::cond_destroy (&rw->waiting_important_writer_);
    }
  return result;
# else
  ACE_UNUSED_ARG (rw);
  ACE_UNUSED_ARG (type);
  ACE_UNUSED_ARG (name);
  ACE_UNUSED_ARG (arg);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}
#endif /* ! ACE_HAS_THREADS || ACE_LACKS_RWLOCK_T */


int
OS::thr_create (ACE_THR_FUNC func,
                    void *args,
                    long flags,
                    ACE_thread_t *&thr_id,
                    ACE_hthread_t *&thr_handle,
                    long priority,
                    void *stack,
                    size_t stacksize)
{
  OS_TRACE ("OS::thr_create");

  if (ACE_BIT_DISABLED (flags, THR_DETACHED) &&
      ACE_BIT_DISABLED (flags, THR_JOINABLE))
    ACE_SET_BITS (flags, THR_JOINABLE);

#if defined (ACE_NO_THREAD_ADAPTER)
# define  ACE_THREAD_FUNCTION  func
# define  ACE_THREAD_ARGUMENT  args
#else /* ! defined (ACE_NO_THREAD_ADAPTER) */
# define  ACE_THREAD_FUNCTION  thread_args->entry_point ()
# define  ACE_THREAD_ARGUMENT  thread_args
#endif /* ! defined (ACE_NO_THREAD_ADAPTER) */

/*
  ACE_Base_Thread_Adapter *thread_args = 0;
  if (thread_adapter == 0)
#if defined (ACE_HAS_WIN32_STRUCTURAL_EXCEPTIONS)
    ACE_NEW_RETURN (thread_args,
                    OS_Thread_Adapter (func, args,
                                           (ACE_THR_C_FUNC) ACE_THREAD_ADAPTER_NAME,
                                           OS_Object_Manager::seh_except_selector(),
                                           OS_Object_Manager::seh_except_handler()),
                    -1);
#else
  ACE_NEW_RETURN (thread_args,
                  OS_Thread_Adapter (func, args,
                                         (ACE_THR_C_FUNC) ACE_THREAD_ADAPTER_NAME),
                  -1);

#endif
  else
    thread_args = thread_adapter;

  auto_ptr <ACE_Base_Thread_Adapter> auto_thread_args;

  if (thread_adapter == 0)
    ACE_AUTO_PTR_RESET (auto_thread_args,
                        thread_args,
                        ACE_Base_Thread_Adapter);
*/
#if defined (ACE_HAS_THREADS)

  // *** Set Stack Size
# if defined (ACE_NEEDS_HUGE_THREAD_STACKSIZE)
  if (stacksize < ACE_NEEDS_HUGE_THREAD_STACKSIZE)
    stacksize = ACE_NEEDS_HUGE_THREAD_STACKSIZE;
# endif /* ACE_NEEDS_HUGE_THREAD_STACKSIZE */

# if !(defined (ACE_VXWORKS) && !defined (ACE_HAS_PTHREADS))
  // On VxWorks, using the task API, the OS will provide a task name if
  // the user doesn't. So, we don't need to create a tmp_thr.  If the
  // caller of this member function is the Thread_Manager, than thr_id
  // will be non-zero anyways.
  ACE_thread_t tmp_thr;

  if (thr_id == 0)
    thr_id = &tmp_thr;
# endif /* !(ACE_VXWORKS && !ACE_HAS_PTHREADS) */

  ACE_hthread_t tmp_handle;
  if (thr_handle == 0)
    thr_handle = &tmp_handle;

# if defined (ACE_HAS_PTHREADS)

  int result;
# if defined (ACE_VXWORKS) && (ACE_VXWORKS >= 0x600) && (ACE_VXWORKS <= 0x620)
  /* Tests show that VxWorks 6.x pthread lib does not only
   * require zeroing of mutex/condition objects to function correctly
   * but also of the attribute objects.
   */
  pthread_attr_t attr = {0};
#   else
  pthread_attr_t attr;
#   endif
#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  if (ACE_ADAPT_RETVAL(::pthread_attr_create (&attr), result) != 0)
#   else /* ACE_HAS_PTHREADS_DRAFT4 */
  if (ACE_ADAPT_RETVAL(::pthread_attr_init(&attr), result) != 0)
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */
      return -1;

  if (stacksize != 0)
    {
      size_t size = stacksize;

#   if defined (PTHREAD_STACK_MIN)
      if (size < static_cast <size_t> (PTHREAD_STACK_MIN))
        size = PTHREAD_STACK_MIN;
#   endif /* PTHREAD_STACK_MIN */

#   if !defined (ACE_LACKS_PTHREAD_ATTR_SETSTACKSIZE)
#     if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
      if (::pthread_attr_setstacksize (&attr, size) != 0)
#     else
#       if !defined (ACE_LACKS_PTHREAD_ATTR_SETSTACK)
        int result;
        if (stack != 0)
          result = ACE_ADAPT_RETVAL (pthread_attr_setstack (&attr, stack, size), result);
        else
          result = ACE_ADAPT_RETVAL (pthread_attr_setstacksize (&attr, size), result);
        if (result == -1)
#       else
        if (ACE_ADAPT_RETVAL (pthread_attr_setstacksize (&attr, size), result) == -1)
#       endif /* !ACE_LACKS_PTHREAD_ATTR_SETSTACK */
#     endif /* ACE_HAS_PTHREADS_DRAFT4, 6 */
          {
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
            ::pthread_attr_delete (&attr);
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
            ::pthread_attr_destroy (&attr);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
            return -1;
          }
#   else
      ACE_UNUSED_ARG (size);
#   endif /* !ACE_LACKS_PTHREAD_ATTR_SETSTACKSIZE */
    }

  // *** Set Stack Address
#   if defined (ACE_LACKS_PTHREAD_ATTR_SETSTACK)
#     if !defined (ACE_LACKS_PTHREAD_ATTR_SETSTACKADDR)
  if (stack != 0)
    {
      if (ACE_ADAPT_RETVAL(::pthread_attr_setstackaddr (&attr, stack), result) != 0)
        {
#       if defined (ACE_HAS_PTHREADS_DRAFT4)
          ::pthread_attr_delete (&attr);
#       else /* ACE_HAS_PTHREADS_DRAFT4 */
          ::pthread_attr_destroy (&attr);
#       endif /* ACE_HAS_PTHREADS_DRAFT4 */
          return -1;
        }
    }
#     else
  ACE_UNUSED_ARG (stack);
#     endif /* !ACE_LACKS_PTHREAD_ATTR_SETSTACKADDR */
#   endif /* ACE_LACKS_PTHREAD_ATTR_SETSTACK */

  // *** Deal with various attributes
  if (flags != 0)
    {
      // *** Set Detach state
#   if !defined (ACE_LACKS_SETDETACH)
      if (ACE_BIT_ENABLED (flags, THR_DETACHED)
          || ACE_BIT_ENABLED (flags, THR_JOINABLE))
        {
          int dstate = PTHREAD_CREATE_JOINABLE;

          if (ACE_BIT_ENABLED (flags, THR_DETACHED))
            dstate = PTHREAD_CREATE_DETACHED;

#     if defined (ACE_HAS_PTHREADS_DRAFT4)
          if (::pthread_attr_setdetach_np (&attr, dstate) != 0)
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
#       if defined (ACE_HAS_PTHREADS_DRAFT6)
            if (::pthread_attr_setdetachstate (&attr, &dstate) != 0)
#       else
              if (ACE_ADAPT_RETVAL(::pthread_attr_setdetachstate (&attr, dstate),
                                   result) != 0)
#       endif /* ACE_HAS_PTHREADS_DRAFT6 */
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
                {
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
                  ::pthread_attr_delete (&attr);
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
                  ::pthread_attr_destroy (&attr);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
                  return -1;
                }
        }

      // Note: if ACE_LACKS_SETDETACH and THR_DETACHED is enabled, we
      // call ::pthread_detach () below.  If THR_DETACHED is not
      // enabled, we call ::pthread_detach () in the Thread_Manager,
      // after joining with the thread.
#   endif /* ACE_LACKS_SETDETACH */

      // *** Set Policy
#   if !defined (ACE_LACKS_SETSCHED) || defined (ACE_HAS_PTHREAD_SCHEDPARAM)
      // If we wish to set the priority explicitly, we have to enable
      // explicit scheduling, and a policy, too.
      if (priority != ACE_DEFAULT_THREAD_PRIORITY)
        {
          ACE_SET_BITS (flags, THR_EXPLICIT_SCHED);
          if (ACE_BIT_DISABLED (flags, THR_SCHED_FIFO)
              && ACE_BIT_DISABLED (flags, THR_SCHED_RR)
              && ACE_BIT_DISABLED (flags, THR_SCHED_DEFAULT))
            ACE_SET_BITS (flags, THR_SCHED_DEFAULT);
        }

      if (ACE_BIT_ENABLED (flags, THR_SCHED_FIFO)
          || ACE_BIT_ENABLED (flags, THR_SCHED_RR)
          || ACE_BIT_ENABLED (flags, THR_SCHED_DEFAULT))
        {
          int spolicy;

#     if defined (ACE_HAS_ONLY_SCHED_OTHER)
          // SunOS, thru version 5.6, only supports SCHED_OTHER.
          spolicy = SCHED_OTHER;
#     elif defined (ACE_HAS_ONLY_SCHED_FIFO)
          // NonStop OSS standard pthread supports only SCHED_FIFO.
          spolicy = SCHED_FIFO;
#     else
          // Make sure to enable explicit scheduling, in case we didn't
          // enable it above (for non-default priority).
          ACE_SET_BITS (flags, THR_EXPLICIT_SCHED);

          if (ACE_BIT_ENABLED (flags, THR_SCHED_DEFAULT))
            spolicy = SCHED_OTHER;
          else if (ACE_BIT_ENABLED (flags, THR_SCHED_FIFO))
            spolicy = SCHED_FIFO;
#       if defined (SCHED_IO)
          else if (ACE_BIT_ENABLED (flags, THR_SCHED_IO))
            spolicy = SCHED_IO;
#       else
          else if (ACE_BIT_ENABLED (flags, THR_SCHED_IO))
            {
              errno = ENOSYS;
              return -1;
            }
#       endif /* SCHED_IO */
          else
            spolicy = SCHED_RR;

#     endif /* ACE_HAS_ONLY_SCHED_OTHER */

#     if defined (ACE_HAS_PTHREADS_DRAFT4)
          result = ::pthread_attr_setsched (&attr, spolicy);
#     elif defined (ACE_HAS_PTHREADS_DRAFT6)
          result = ::pthread_attr_setschedpolicy (&attr, spolicy);
#     else  /* draft 7 or std */
          (void) ACE_ADAPT_RETVAL(::pthread_attr_setschedpolicy (&attr, spolicy),
                           result);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
          if (result != 0)
            {
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
              ::pthread_attr_delete (&attr);
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
              ::pthread_attr_destroy (&attr);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
              return -1;
            }
        }

      // *** Set Priority (use reasonable default priorities)
#     if defined(ACE_HAS_PTHREADS_STD)
      // If we wish to explicitly set a scheduling policy, we also
      // have to specify a priority.  We choose a "middle" priority as
      // default.  Maybe this is also necessary on other POSIX'ish
      // implementations?
      if ((ACE_BIT_ENABLED (flags, THR_SCHED_FIFO)
           || ACE_BIT_ENABLED (flags, THR_SCHED_RR)
           || ACE_BIT_ENABLED (flags, THR_SCHED_DEFAULT))
          && priority == ACE_DEFAULT_THREAD_PRIORITY)
        {
          if (ACE_BIT_ENABLED (flags, THR_SCHED_FIFO))
            priority = ACE_THR_PRI_FIFO_DEF;
          else if (ACE_BIT_ENABLED (flags, THR_SCHED_RR))
            priority = ACE_THR_PRI_RR_DEF;
          else // THR_SCHED_DEFAULT
            priority = ACE_THR_PRI_OTHER_DEF;
        }
#     endif /* ACE_HAS_PTHREADS_STD */
      if (priority != ACE_DEFAULT_THREAD_PRIORITY)
        {
          struct sched_param sparam;
          OS::memset ((void *) &sparam, 0, sizeof sparam);

#     if defined (ACE_HAS_IRIX62_THREADS)
          sparam.sched_priority = ACE_MIN (priority,
                                           (long) PTHREAD_MAX_PRIORITY);
#     elif defined (PTHREAD_MAX_PRIORITY) && !defined(ACE_HAS_PTHREADS_STD)
          /* For MIT pthreads... */
          sparam.prio = ACE_MIN (priority, PTHREAD_MAX_PRIORITY);
#     elif defined(ACE_HAS_PTHREADS_STD) && !defined (ACE_HAS_STHREADS)
          // The following code forces priority into range.
          if (ACE_BIT_ENABLED (flags, THR_SCHED_FIFO))
            sparam.sched_priority =
              ACE_MIN (ACE_THR_PRI_FIFO_MAX,
                       ACE_MAX (ACE_THR_PRI_FIFO_MIN, priority));
          else if (ACE_BIT_ENABLED(flags, THR_SCHED_RR))
            sparam.sched_priority =
              ACE_MIN (ACE_THR_PRI_RR_MAX,
                       ACE_MAX (ACE_THR_PRI_RR_MIN, priority));
          else // Default policy, whether set or not
            sparam.sched_priority =
              ACE_MIN (ACE_THR_PRI_OTHER_MAX,
                       ACE_MAX (ACE_THR_PRI_OTHER_MIN, priority));
#     elif defined (PRIORITY_MAX)
          sparam.sched_priority = ACE_MIN (priority,
                                           (long) PRIORITY_MAX);
#     else
          sparam.sched_priority = priority;
#     endif /* ACE_HAS_IRIX62_THREADS */

          {
#       if defined (sun)  &&  defined (ACE_HAS_ONLY_SCHED_OTHER)
            // SunOS, through 5.6, POSIX only allows priorities > 0 to
            // ::pthread_attr_setschedparam.  If a priority of 0 was
            // requested, set the thread priority after creating it, below.
            if (priority > 0)
#       endif /* sun && ACE_HAS_ONLY_SCHED_OTHER */
              {
#       if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
                result = ::pthread_attr_setprio (&attr,
                                                 sparam.sched_priority);
#       else /* this is draft 7 or std */
                (void) ACE_ADAPT_RETVAL(::pthread_attr_setschedparam (&attr, &sparam),
                                        result);
#       endif /* ACE_HAS_PTHREADS_DRAFT4, 6 */
                if (result != 0)
                  {
#       if defined (ACE_HAS_PTHREADS_DRAFT4)
                    ::pthread_attr_delete (&attr);
#       else /* ACE_HAS_PTHREADS_DRAFT4 */
                    ::pthread_attr_destroy (&attr);
#       endif /* ACE_HAS_PTHREADS_DRAFT4 */
                    return -1;
                  }
              }
          }
        }

      // *** Set scheduling explicit or inherited
      if (ACE_BIT_ENABLED (flags, THR_INHERIT_SCHED)
          || ACE_BIT_ENABLED (flags, THR_EXPLICIT_SCHED))
        {
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
          int sched = PTHREAD_DEFAULT_SCHED;
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
          int sched = PTHREAD_EXPLICIT_SCHED;
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
          if (ACE_BIT_ENABLED (flags, THR_INHERIT_SCHED))
            sched = PTHREAD_INHERIT_SCHED;
          if (ACE_ADAPT_RETVAL(::pthread_attr_setinheritsched (&attr, sched), result) != 0)
            {
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
              ::pthread_attr_delete (&attr);
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
              ::pthread_attr_destroy (&attr);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
              return -1;
            }
        }
#   else /* ACE_LACKS_SETSCHED */
      ACE_UNUSED_ARG (priority);
#   endif /* ACE_LACKS_SETSCHED */

      // *** Set Scope
#   if !defined (ACE_LACKS_THREAD_PROCESS_SCOPING)
      if (ACE_BIT_ENABLED (flags, THR_SCOPE_SYSTEM)
          || ACE_BIT_ENABLED (flags, THR_SCOPE_PROCESS))
        {
#     if defined (ACE_CONFIG_LINUX_H) || defined (HPUX) || defined (ACE_VXWORKS)
          // LinuxThreads do not have support for PTHREAD_SCOPE_PROCESS.
          // Neither does HPUX (up to HP-UX 11.00, as far as I know).
          // Also VxWorks only delivers scope system
          int scope = PTHREAD_SCOPE_SYSTEM;
#     else /* ACE_CONFIG_LINUX_H */
          int scope = PTHREAD_SCOPE_PROCESS;
#     endif /* ACE_CONFIG_LINUX_H */
          if (ACE_BIT_ENABLED (flags, THR_SCOPE_SYSTEM))
            scope = PTHREAD_SCOPE_SYSTEM;

          if (ACE_ADAPT_RETVAL(::pthread_attr_setscope (&attr, scope), result) != 0)
            {
#     if defined (ACE_HAS_PTHREADS_DRAFT4)
              ::pthread_attr_delete (&attr);
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
              ::pthread_attr_destroy (&attr);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
              return -1;
            }
        }
#   endif /* !ACE_LACKS_THREAD_PROCESS_SCOPING */

#   ifdef ACE_HAS_PTHREAD_ATTR_SETCREATESUSPEND_NP
      if (ACE_BIT_ENABLED (flags, THR_SUSPENDED))
        {
           if (ACE_ADAPT_RETVAL(::pthread_attr_setcreatesuspend_np(&attr), result) != 0)
            {

#     if defined (ACE_HAS_PTHREADS_DRAFT4)
              ::pthread_attr_delete (&attr);
#     else /* ACE_HAS_PTHREADS_DRAFT4 */
              ::pthread_attr_destroy (&attr);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
              return -1;
            }
        }
#   endif /* !ACE_HAS_PTHREAD_ATTR_SETCREATESUSPEND_NP */

#   if ! defined(ACE_LACKS_THR_CONCURRENCY_FUNCS)
      if (ACE_BIT_ENABLED (flags, THR_NEW_LWP))
        {
          // Increment the number of LWPs by one to emulate the
          // SunOS semantics.
          int lwps = OS::thr_getconcurrency ();
          if (lwps == -1)
            {
              if (errno == ENOTSUP)
                // Suppress the ENOTSUP because it's harmless.
                errno = 0;
              else
                // This should never happen on SunOS:
                // ::thr_getconcurrency () should always succeed.
                return -1;
            }
          else if (OS::thr_setconcurrency (lwps + 1) == -1)
            {
              if (errno == ENOTSUP)
                {
                  // Unlikely: ::thr_getconcurrency () is supported
                  // but ::thr_setconcurrency () is not?
                }
              else
                return -1;
            }
        }
#   endif /* ! ACE_LACKS_THR_CONCURRENCY_FUNCS */
    }

#   if defined (ACE_HAS_PTHREADS_DRAFT4)
  OSCALL (::pthread_create (thr_id, attr,
                                thread_args->entry_point (),
                                thread_args),
              int, -1, result);

#     if defined (ACE_LACKS_SETDETACH)
  if (ACE_BIT_ENABLED (flags, THR_DETACHED))
    {
      ::pthread_detach (thr_id);
    }
#     endif /* ACE_LACKS_SETDETACH */

  ::pthread_attr_delete (&attr);

#   elif defined (ACE_HAS_PTHREADS_DRAFT6)
  OSCALL (::pthread_create (thr_id, &attr,
                                thread_args->entry_point (),
                                thread_args),
              int, -1, result);
  ::pthread_attr_destroy (&attr);

#   else /* this is draft 7 or std */
  OSCALL (ACE_ADAPT_RETVAL (::pthread_create (thr_id,
                                                  &attr,
                                                  func,
                                                  args),
                                result),
              int, -1, result);
  ::pthread_attr_destroy (&attr);
#   endif /* ACE_HAS_PTHREADS_DRAFT4 */

  // This is a SunOS or POSIX implementation of pthreads, where we
  // assume that ACE_thread_t and ACE_hthread_t are the same.  If this
  // *isn't* correct on some platform, please let us know.
  if (result != -1)
    *thr_handle = *thr_id;

#   if defined (sun)  &&  defined (ACE_HAS_ONLY_SCHED_OTHER)
  // SunOS prior to 5.7:

  // If the priority is 0, then we might have to set it now because we
  // couldn't set it with ::pthread_attr_setschedparam, as noted
  // above.  This doesn't provide strictly correct behavior, because
  // the thread was created (above) with the priority of its parent.
  // (That applies regardless of the inherit_sched attribute: if it
  // was PTHREAD_INHERIT_SCHED, then it certainly inherited its
  // parent's priority.  If it was PTHREAD_EXPLICIT_SCHED, then "attr"
  // was initialized by the SunOS ::pthread_attr_init () to contain
  // NULL for the priority, which indicated to SunOS ::pthread_create
  // () to inherit the parent priority.)
  if (priority == 0)
    {
      // Check the priority of this thread, which is the parent
      // of the newly created thread.  If it is 0, then the
      // newly created thread will have inherited the priority
      // of 0, so there's no need to explicitly set it.
      struct sched_param sparam;
      int policy = 0;
      OSCALL (ACE_ADAPT_RETVAL (::pthread_getschedparam (thr_self (),
                                                             &policy,
                                                             &sparam),
                                    result), int,
                  -1, result);

      // The only policy supported by by SunOS, thru version 5.6,
      // is SCHED_OTHER, so that's hard-coded here.
      policy = ACE_SCHED_OTHER;

      if (sparam.sched_priority != 0)
        {
          OS::memset ((void *) &sparam, 0, sizeof sparam);
          // The memset to 0 sets the priority to 0, so we don't need
          // to explicitly set sparam.sched_priority.

          OSCALL_RETURN (ACE_ADAPT_RETVAL (::pthread_setschedparam (*thr_id,
                                                                        policy,
                                                                        &sparam),
                                               result),
                             int, -1);
        }
    }

#     if defined (ACE_NEEDS_LWP_PRIO_SET)
#       if 0
  // It would be useful if we could make this work.  But, it requires
  // a mechanism for determining the ID of an LWP to which another
  // thread is bound.  Is there a way to do that?  Instead, just rely
  // on the code in ACE_Thread_Adapter::invoke () to set the LWP
  // priority.

  // If the thread is bound, then set the priority on its LWP.
  if (ACE_BIT_ENABLED (flags, THR_BOUND))
    {
      ACE_Sched_Params sched_params (ACE_BIT_ENABLED (flags, THR_SCHED_FIFO) ||
                                     ACE_BIT_ENABLED (flags, THR_SCHED_RR)  ?
                                     ACE_SCHED_FIFO  :
                                     ACE_SCHED_OTHER,
                                     priority);
      result = OS::lwp_setparams (sched_params,
                                      /* ? How do we find the ID of the LWP
                                         to which *thr_id is bound? */);
    }
#       endif /* 0 */
#     endif /* ACE_NEEDS_LWP_PRIO_SET */

#   endif /* sun && ACE_HAS_ONLY_SCHED_OTHER */
  //auto_thread_args.release ();
  return result;
# elif defined (ACE_HAS_STHREADS)
  int result;
  int start_suspended = ACE_BIT_ENABLED (flags, THR_SUSPENDED);

  if (priority != ACE_DEFAULT_THREAD_PRIORITY)
    // If we need to set the priority, then we need to start the
    // thread in a suspended mode.
    ACE_SET_BITS (flags, THR_SUSPENDED);

  OSCALL (ACE_ADAPT_RETVAL (::thr_create (stack, stacksize,
                                              thread_args->entry_point (),
                                              thread_args,
                                              flags, thr_id), result),
              int, -1, result);

  if (result != -1)
    {
      // With SunOS threads, ACE_thread_t and ACE_hthread_t are the same.
      *thr_handle = *thr_id;

      if (priority != ACE_DEFAULT_THREAD_PRIORITY)
        {
          // Set the priority of the new thread and then let it
          // continue, but only if the user didn't start it suspended
          // in the first place!
          result = OS::thr_setprio (*thr_id, priority);
          if (result != 0)
            {
              errno = result;
              return -1;
            }

          if (start_suspended == 0)
            {
              result = OS::thr_continue (*thr_id);
              if (result != 0)
                {
                  errno = result;
                  return -1;
                }
            }
        }
    }
  auto_thread_args.release ();
  return result;
# elif defined (ACE_HAS_WTHREADS)
  ACE_UNUSED_ARG (stack);
#   if defined (ACE_HAS_MFC) && (ACE_HAS_MFC != 0)
  if (ACE_BIT_ENABLED (flags, THR_USE_AFX))
    {
      CWinThread *cwin_thread =
        ::AfxBeginThread ((AFX_THREADPROC) thread_args->entry_point (),
                          thread_args,
                          priority,
                          0,
                          flags | THR_SUSPENDED);
      // Have to duplicate the handle because
      // CWinThread::~CWinThread() closes the original handle.
#     if !defined (ACE_HAS_WINCE)
      (void) ::DuplicateHandle (::GetCurrentProcess (),
                                cwin_thread->m_hThread,
                                ::GetCurrentProcess (),
                                thr_handle,
                                0,
                                TRUE,
                                DUPLICATE_SAME_ACCESS);
#     endif /* ! ACE_HAS_WINCE */
      *thr_id = cwin_thread->m_nThreadID;

      if (ACE_BIT_ENABLED (flags, THR_SUSPENDED) == 0)
        cwin_thread->ResumeThread ();
      // cwin_thread will be deleted in AfxThreadExit()
      // Warning: If AfxThreadExit() is called from within the
      // thread, ACE_TSS_Cleanup->thread_exit() never gets called !
    }
  else
#   endif /* ACE_HAS_MFC */
    {
      int start_suspended = ACE_BIT_ENABLED (flags, THR_SUSPENDED);

      if (priority != ACE_DEFAULT_THREAD_PRIORITY)
        // If we need to set the priority, then we need to start the
        // thread in a suspended mode.
        ACE_SET_BITS (flags, THR_SUSPENDED);

      *thr_handle = (void *) ACE_BEGINTHREADEX (0,
                                                static_cast <u_int> (stacksize),
                                                func,
                                                args,
                                                flags,
                                                thr_id);

      if (priority != ACE_DEFAULT_THREAD_PRIORITY && *thr_handle != 0)
        {
          // Set the priority of the new thread and then let it
          // continue, but only if the user didn't start it suspended
          // in the first place!
          OS::thr_setprio (*thr_handle, priority);

          if (start_suspended == 0)
            OS::thr_continue (*thr_handle);
        }
    }
#   if 0
  *thr_handle = ::CreateThread
    (0,
     stacksize,
     LPTHREAD_START_ROUTINE (thread_args->entry_point ()),
     thread_args,
     flags,
     thr_id);
#   endif /* 0 */

  // Close down the handle if no one wants to use it.
  if (thr_handle == &tmp_handle && tmp_handle != 0)
    ::CloseHandle (tmp_handle);

  if (*thr_handle != 0)
    {
      //auto_thread_args.release ();
      return 0;
    }
  else
    ACE_FAIL_RETURN (-1);
  /* NOTREACHED */

# elif defined (ACE_VXWORKS)
  // The hard-coded values below are what ::sp () would use.  (::sp ()
  // hardcodes priority to 100, flags to VX_FP_TASK, and stacksize to
  // 20,000.)  stacksize should be an even integer.  If a stack is not
  // specified, ::taskSpawn () is used so that we can set the
  // priority, flags, and stacksize.  If a stack is specified,
  // ::taskInit ()/::taskActivate() are used.

  // If called with thr_create() defaults, use same default values as ::sp ():
  if (priority == ACE_DEFAULT_THREAD_PRIORITY) priority = 100;
  // Assumes that there is a floating point coprocessor.  As noted
  // above, ::sp () hardcodes this, so we should be safe with it.
  if (flags == 0) flags = VX_FP_TASK;
  if (stacksize == 0) stacksize = 20000;

  const u_int thr_id_provided =
    thr_id  &&  *thr_id  &&  (*thr_id)[0] != ACE_THR_ID_ALLOCATED;

  ACE_hthread_t tid;
#   if 0 /* Don't support setting of stack, because it doesn't seem to work. */
  if (stack == 0)
    {
#   else
      ACE_UNUSED_ARG (stack);
#   endif /* 0 */
      // The call below to ::taskSpawn () causes VxWorks to assign a
      // unique task name of the form: "t" + an integer, because the
      // first argument is 0.
      tid = ::taskSpawn (thr_id_provided  ?  *thr_id  :  0,
                         priority,
                         (int) flags,
                         (int) stacksize,
                         thread_args->entry_point (),
                         (int) thread_args,
                         0, 0, 0, 0, 0, 0, 0, 0, 0);
#   if 0 /* Don't support setting of stack, because it doesn't seem to work. */
    }
  else
    {
      // If a task name (thr_id) was not supplied, then the task will
      // not have a unique name.  That's VxWorks' behavior.

      // Carve out a TCB at the beginning of the stack space.  The TCB
      // occupies 400 bytes with VxWorks 5.3.1/I386.
      WIND_TCB *tcb = (WIND_TCB *) stack;

      // The TID is defined to be the address of the TCB.
      int status = ::taskInit (tcb,
                               thr_id_provided  ?  *thr_id  :  0,
                               priority,
                               (int) flags,
                               (char *) stack + sizeof (WIND_TCB),
                               (int) (stacksize - sizeof (WIND_TCB)),
                               thread_args->entry_point (),
                               (int) thread_args,
                               0, 0, 0, 0, 0, 0, 0, 0, 0);

      if (status == OK)
        {
          // The task was successfully initialized, now activate it.
          status = ::taskActivate ((ACE_hthread_t) tcb);
        }

      tid = status == OK  ?  (ACE_hthread_t) tcb  :  ERROR;
    }
#   endif /* 0 */

  if (tid == ERROR)
    return -1;
  else
    {
      if (! thr_id_provided && thr_id)
        {
          if (*thr_id && (*thr_id)[0] == ACE_THR_ID_ALLOCATED)
            // *thr_id was allocated by the Thread_Manager.  ::taskTcb
            // (int tid) returns the address of the WIND_TCB (task
            // control block).  According to the ::taskSpawn()
            // documentation, the name of the new task is stored at
            // pStackBase, but is that of the current task?  If so, it
            // might be a bit quicker than this extraction of the tcb
            // . . .
            OS::strsncpy (*thr_id + 1, ::taskName (tid), 10);
          else
            // *thr_id was not allocated by the Thread_Manager.
            // Pass back the task name in the location pointed to
            // by thr_id.
            *thr_id = ::taskName (tid);
        }
      // else if the thr_id was provided, there's no need to overwrite
      // it with the same value (string).  If thr_id is 0, then we can't
      // pass the task name back.

      if (thr_handle)
        *thr_handle = tid;

      auto_thread_args.release ();
      return 0;
    }

# endif /* ACE_HAS_STHREADS */
#else
  ACE_UNUSED_ARG (func);
  ACE_UNUSED_ARG (args);
  ACE_UNUSED_ARG (flags);
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (thr_handle);
  ACE_UNUSED_ARG (priority);
  ACE_UNUSED_ARG (stack);
  ACE_UNUSED_ARG (stacksize);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_THREADS */
}

void
OS::thr_exit (ACE_THR_FUNC_RETURN status)
{
  OS_TRACE ("OS::thr_exit");
#if defined (ACE_HAS_THREADS)
# if defined (ACE_HAS_PTHREADS)
    ::pthread_exit (status);
# elif defined (ACE_HAS_STHREADS)
    ::thr_exit (status);
# elif defined (ACE_HAS_WTHREADS)
    // Can't call it here because on NT, the thread is exited
    // directly by ACE_Thread_Adapter::invoke ().
    //   ACE_TSS_Cleanup::instance ()->thread_exit (status);

#   if defined (ACE_HAS_MFC) && (ACE_HAS_MFC != 0)
    int using_afx = -1;
    // An ACE_Thread_Descriptor really is an OS_Thread_Descriptor.
    // But without #including ace/Thread_Manager.h, we don't know that.
    OS_Thread_Descriptor *td =
      ACE_Base_Thread_Adapter::thr_desc_log_msg ();
    if (td)
      using_afx = ACE_BIT_ENABLED (td->flags (), THR_USE_AFX);
#   endif /* ACE_HAS_MFC && (ACE_HAS_MFC != 0) */

    // Call TSS destructors.
    //OS::cleanup_tss (0 /* not main thread */);

    // Exit the thread.
    // Allow CWinThread-destructor to be invoked from AfxEndThread.
    // _endthreadex will be called from AfxEndThread so don't exit the
    // thread now if we are running an MFC thread.
#   if defined (ACE_HAS_MFC) && (ACE_HAS_MFC != 0)
    if (using_afx != -1)
      {
        if (using_afx)
          ::AfxEndThread (status);
        else
          ACE_ENDTHREADEX (status);
      }
    else
      {
        // Not spawned by ACE_Thread_Manager, use the old buggy
        // version.  You should seriously consider using
        // ACE_Thread_Manager to spawn threads.  The following code is
        // know to cause some problem.
        CWinThread *pThread = ::AfxGetThread ();
        if (!pThread || pThread->m_nThreadID != OS::thr_self ())
          ACE_ENDTHREADEX (status);
        else
          ::AfxEndThread (status);
      }
#   else
    ACE_ENDTHREADEX (status);
#   endif /* ACE_HAS_MFC && ACE_HAS_MFS != 0*/

# elif defined (ACE_VXWORKS)
    ACE_hthread_t tid;
    OS::thr_self (tid);
    *((int *) status) = ::taskDelete (tid);
# endif /* ACE_HAS_PTHREADS */
#else
  ACE_UNUSED_ARG (status);
#endif /* ACE_HAS_THREADS */
}

#if defined (ACE_VXWORKS) && !defined (ACE_HAS_PTHREADS)
// Leave this in the global scope to allow
// users to adjust the delay value.
int ACE_THR_JOIN_DELAY = 5;

int
OS::thr_join (ACE_hthread_t thr_handle,
                  ACE_THR_FUNC_RETURN *status)
{
  // We can't get the status of the thread
  if (status != 0)
    {
      *status = 0;
    }

  // This method can not support joining all threads
  if (OS::thr_cmp (thr_handle, OS::NULL_hthread))
    {
      ACE_NOTSUP_RETURN (-1);
    }

  int retval = ESRCH;
  ACE_hthread_t current;
  OS::thr_self (current);

  // Make sure we are not joining ourself
  if (OS::thr_cmp (thr_handle, current))
    {
      retval = EDEADLK;
    }
  else
    {
      // Whether the task exists or not
      // we will return a successful value
      retval = 0;

      // Verify that the task id still exists
      while (taskIdVerify (thr_handle) == OK)
        {
          // Wait a bit to see if the task is still active.
          OS::sleep (ACE_THR_JOIN_DELAY);
        }
    }

  // Adapt the return value into errno and return value.
  // The ACE_ADAPT_RETVAL macro doesn't exactly do what
  // we need to do here, so we do it manually.
  if (retval != 0)
    {
      errno = retval;
      retval = -1;
    }

  return retval;
}

int
OS::thr_join (ACE_thread_t waiter_id,
                  ACE_thread_t *thr_id,
                  ACE_THR_FUNC_RETURN *status)
{
  thr_id = 0;
  return OS::thr_join (taskNameToId (waiter_id), status);
}
#endif /* ACE_VXWORKS */

int
OS::thr_key_detach (ACE_thread_key_t key, void *)
{
#if defined (ACE_HAS_WTHREADS) || defined (ACE_HAS_TSS_EMULATION)
 /* TSS_Cleanup_Instance cleanup;
  if (cleanup.valid ())
    {
      return cleanup->thread_detach_key (key);
    }
  else
    {
      return -1;
    }
  */ //by qinghua
  return 0;
#else
  ACE_UNUSED_ARG (key);
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_WTHREADS || ACE_HAS_TSS_EMULATION */
}

int
OS::thr_get_affinity (ACE_hthread_t thr_id,
                          size_t cpu_set_size,
                          cpu_set_t * cpu_mask)
{
#if defined (ACE_HAS_PTHREAD_GETAFFINITY_NP)
  // Handle of the thread, which is NPTL thread-id, normally a big number
  if (::pthread_getaffinity_np (thr_id, cpu_set_size, cpu_mask) != 0)
    {
      return -1;
    }
  return 0;
#elif defined (ACE_HAS_2_PARAM_SCHED_GETAFFINITY)
  // The process-id is expected as <thr_id>, which can be a thread-id of
  // linux-thread, thus making binding to cpu of that particular thread only.
  // If you are using this flag for NPTL-threads, however, please pass as a
  // thr_id process id obtained by OS::getpid ()
  ACE_UNUSED_ARG (cpu_set_size);
  if (::sched_getaffinity(thr_id, cpu_mask) == -1)
    {
      return -1;
    }
  return 0;
#elif defined (ACE_HAS_SCHED_GETAFFINITY)
  // The process-id is expected as <thr_id>, which can be a thread-id of
  // linux-thread, thus making binding to cpu of that particular thread only.
  // If you are using this flag for NPTL-threads, however, please pass as a
  // thr_id process id obtained by OS::getpid ()
  if (::sched_getaffinity(thr_id, cpu_set_size, cpu_mask) == -1)
    {
      return -1;
    }
  return 0;
#else
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (cpu_set_size);
  ACE_UNUSED_ARG (cpu_mask);
  ACE_NOTSUP_RETURN (-1);
#endif
}

int
OS::thr_set_affinity (ACE_hthread_t thr_id,
                          size_t cpu_set_size,
                          const cpu_set_t * cpu_mask)
{
#if defined (ACE_HAS_PTHREAD_SETAFFINITY_NP)
  if (::pthread_setaffinity_np (thr_id, cpu_set_size, cpu_mask) != 0)
    {
      return -1;
    }
  return 0;
#elif defined (ACE_HAS_2_PARAM_SCHED_SETAFFINITY)
  // The process-id is expected as <thr_id>, which can be a thread-id of
  // linux-thread, thus making binding to cpu of that particular thread only.
  // If you are using this flag for NPTL-threads, however, please pass as a
  // thr_id process id obtained by OS::getpid (), but whole process will bind your CPUs
  //
  ACE_UNUSED_ARG (cpu_set_size);
  if (::sched_setaffinity (thr_id, cpu_mask) == -1)
    {
      return -1;
    }
  return 0;
#elif defined (ACE_HAS_SCHED_SETAFFINITY)
  // The process-id is expected as <thr_id>, which can be a thread-id of
  // linux-thread, thus making binding to cpu of that particular thread only.
  // If you are using this flag for NPTL-threads, however, please pass as a
  // thr_id process id obtained by OS::getpid (), but whole process will bind your CPUs
  //
  if (::sched_setaffinity (thr_id, cpu_set_size, cpu_mask) == -1)
    {
      return -1;
    }
  return 0;
#else
  ACE_UNUSED_ARG (thr_id);
  ACE_UNUSED_ARG (cpu_set_size);
  ACE_UNUSED_ARG (cpu_mask);
  ACE_NOTSUP_RETURN (-1);
#endif
}

int
OS::thr_key_used (ACE_thread_key_t key)
{
  return -1; //by qinghua
}

#if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
int
OS::thr_keycreate_native (OS_thread_key_t *key,
# if defined (ACE_HAS_THR_C_DEST)
                       ACE_THR_C_DEST dest
# else
                       ACE_THR_DEST dest
# endif /* ACE_HAS_THR_C_DEST */
                       )
{
  // can't trace here. Trace uses TSS
  // OS_TRACE ("OS::thr_keycreate_native");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS)

#     if defined (ACE_HAS_PTHREADS_DRAFT4)
#       if defined (ACE_HAS_STDARG_THR_DEST)
    OSCALL_RETURN (::pthread_keycreate (key, (void (*)(...)) dest), int, -1);
#       else  /* ! ACE_HAS_STDARG_THR_DEST */
    OSCALL_RETURN (::pthread_keycreate (key, dest), int, -1);
#       endif /* ! ACE_HAS_STDARG_THR_DEST */
#     elif defined (ACE_HAS_PTHREADS_DRAFT6)
    OSCALL_RETURN (::pthread_key_create (key, dest), int, -1);
#     else
    int result;
    OSCALL_RETURN (ACE_ADAPT_RETVAL (::pthread_key_create (key, dest),
                                         result),
                       int, -1);
#     endif /* ACE_HAS_PTHREADS_DRAFT4 */
#   elif defined (ACE_HAS_STHREADS)
    int result;
    OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_keycreate (key, dest),
                                         result),
                       int, -1);
#   elif defined (ACE_HAS_WTHREADS)
    ACE_UNUSED_ARG (dest);
    *key = ::TlsAlloc ();

    if (*key == ACE_SYSCALL_FAILED)
      ACE_FAIL_RETURN (-1);
    return 0;
#   endif /* ACE_HAS_STHREADS */
# else
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (dest);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}
#endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */

int
OS::thr_keycreate (ACE_thread_key_t *key,
# if defined (ACE_HAS_THR_C_DEST)
                       ACE_THR_C_DEST dest,
# else
                       ACE_THR_DEST dest,
# endif /* ACE_HAS_THR_C_DEST */
                       void *)
{
  // OS_TRACE ("OS::thr_keycreate");
#if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_TSS_EMULATION)
    if (ACE_TSS_Emulation::next_key (*key) == 0)
      {
        ACE_TSS_Emulation::tss_destructor (*key, dest);

        // Extract out the thread-specific table instance and stash away
        // the key and destructor so that we can free it up later on...
        TSS_Cleanup_Instance cleanup (TSS_Cleanup_Instance::CREATE);
        if (cleanup.valid ())
          {
            return cleanup->insert (*key, dest);
          }
        else
          {
            return -1;
          }
      }
    else
      return -1;
#   elif defined (ACE_HAS_WTHREADS)
    if (OS::thr_keycreate_native (key, dest) == 0)
      {
        // Extract out the thread-specific table instance and stash away
        // the key and destructor so that we can free it up later on...
       /* TSS_Cleanup_Instance cleanup (TSS_Cleanup_Instance::CREATE);
        if (cleanup.valid ())
          {
            return cleanup->insert (*key, dest);
          }
        else
          {
            return -1;
          }*/ //by qinghua
        return 0; //qinghua
      }
    else
      return -1;
      /* NOTREACHED */
#   elif defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
    return  OS::thr_keycreate_native (key, dest);
#   else
    ACE_UNUSED_ARG (key);
    ACE_UNUSED_ARG (dest);
    ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_TSS_EMULATION */
# else /* ACE_HAS_THREADS */
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (dest);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

#if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
int
OS::thr_keyfree_native (OS_thread_key_t key)
{
  OS_TRACE ("OS::thr_keyfree_native");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
    ACE_UNUSED_ARG (key);
    ACE_NOTSUP_RETURN (-1);
#   elif defined (ACE_HAS_PTHREADS)
    return ::pthread_key_delete (key);
#   elif defined (ACE_HAS_THR_KEYDELETE)
    return ::thr_keydelete (key);
#   elif defined (ACE_HAS_STHREADS)
    ACE_UNUSED_ARG (key);
    ACE_NOTSUP_RETURN (-1);
#   elif defined (ACE_HAS_WTHREADS)
    ACE_WIN32CALL_RETURN (ACE_ADAPT_RETVAL (::TlsFree (key), ace_result_), int, -1);
#   else
    ACE_UNUSED_ARG (key);
    ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_PTHREADS */
# else
  ACE_UNUSED_ARG (key);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}
#endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */

int
OS::thr_keyfree (ACE_thread_key_t key)
{
  OS_TRACE ("OS::thr_keyfree");
# if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_TSS_EMULATION)
    // Release the key in the TSS_Emulation administration
    ACE_TSS_Emulation::release_key (key);
    TSS_Cleanup_Instance cleanup;
    if (cleanup.valid ())
      {
        return cleanup->free_key (key);
      }
    return -1;
#   elif defined (ACE_HAS_WTHREADS)
    // Extract out the thread-specific table instance and free up
    // the key and destructor.
  /* by qinghua
    TSS_Cleanup_Instance cleanup;
    if (cleanup.valid ())
      {
        return cleanup->free_key (key);
      }*/
    return -1;
#   elif defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
    return OS::thr_keyfree_native (key);
#   else
    ACE_UNUSED_ARG (key);
    ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_TSS_EMULATION */
# else /* ACE_HAS_THREADS */
  ACE_UNUSED_ARG (key);
  ACE_NOTSUP_RETURN (-1);
  return 0;
# endif /* ACE_HAS_THREADS */
}

int
OS::thr_setprio (const ACE_Sched_Priority prio)
{
  // Set the thread priority on the current thread.
  ACE_hthread_t my_thread_id;
  OS::thr_self (my_thread_id);

  int const status = OS::thr_setprio (my_thread_id, prio);

#if defined (ACE_NEEDS_LWP_PRIO_SET)
  // If the thread is in the RT class, then set the priority on its
  // LWP.  (Instead of doing this if the thread is in the RT class, it
  // should be done for all bound threads.  But, there doesn't appear
  // to be an easy way to determine if the thread is bound.)

  if (status == 0)
    {
      // Find what scheduling class the thread's LWP is in.
      ACE_Sched_Params sched_params (ACE_SCHED_OTHER, 0);
      if (OS::lwp_getparams (sched_params) == -1)
        {
          return -1;
        }
      else if (sched_params.policy () == ACE_SCHED_FIFO  ||
               sched_params.policy () == ACE_SCHED_RR)
        {
          // This thread's LWP is in the RT class, so we need to set
          // its priority.
          sched_params.priority (prio);
          return OS::lwp_setparams (sched_params);
        }
      // else this is not an RT thread.  Nothing more needs to be
      // done.
    }
#endif /* ACE_NEEDS_LWP_PRIO_SET */

  return status;
}

# if defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
int
OS::thr_setspecific_native (OS_thread_key_t key, void *data)
{
  // OS_TRACE ("OS::thr_setspecific_native");
#   if defined (ACE_HAS_THREADS)
#     if defined (ACE_HAS_PTHREADS)
#      if defined (ACE_HAS_PTHREADS_DRAFT4) || defined (ACE_HAS_PTHREADS_DRAFT6)
    OSCALL_RETURN (::pthread_setspecific (key, data), int, -1);
#       else
    int result;
    OSCALL_RETURN (ACE_ADAPT_RETVAL (pthread_setspecific (key, data),
                                         result),
                       int, -1);
#       endif /* ACE_HAS_PTHREADS_DRAFT4, 6 */

#     elif defined (ACE_HAS_STHREADS)
      int result;
      OSCALL_RETURN (ACE_ADAPT_RETVAL (::thr_setspecific (key, data), result), int, -1);
#     elif defined (ACE_HAS_WTHREADS)
      ::TlsSetValue (key, data);
      return 0;
#     else /* ACE_HAS_STHREADS */
      ACE_UNUSED_ARG (key);
      ACE_UNUSED_ARG (data);
      ACE_NOTSUP_RETURN (-1);
#     endif /* ACE_HAS_STHREADS */
#   else
    ACE_UNUSED_ARG (key);
    ACE_UNUSED_ARG (data);
    ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_THREADS */
}
# endif /* ACE_HAS_THREAD_SPECIFIC_STORAGE */

int
OS::thr_setspecific (ACE_thread_key_t key, void *data)
{
  // OS_TRACE ("OS::thr_setspecific");
#if defined (ACE_HAS_THREADS)
#   if defined (ACE_HAS_TSS_EMULATION)
    if (ACE_TSS_Emulation::is_key (key) == 0)
      {
        errno = EINVAL;
        data = 0;
        return -1;
      }
    else
      {
        ACE_TSS_Emulation::ts_object (key) = data;
        TSS_Cleanup_Instance cleanup;
        if (cleanup.valid ())
          {
            cleanup->thread_use_key (key);
            // for TSS_Cleanup purposes treat stetting data to zero
            // like detaching.  This is a consequence of POSIX allowing
            // deletion of a "used" key.
            if (data == 0)
              {
                cleanup->thread_detach_key (key);
              }
            return 0;
          }
        else
          {
            return -1;
          }
      }
#   elif defined (ACE_HAS_WTHREADS)
    if (OS::thr_setspecific_native (key, data) == 0)
      {/* by qinghua
        TSS_Cleanup_Instance cleanup;
        if (cleanup.valid ())
          {
            cleanup->thread_use_key (key);
            // for TSS_Cleanup purposes treat stetting data to zero
            // like detaching.  This is a consequence of POSIX allowing
            // deletion of a "used" key.
            if (data == 0)
              {
                cleanup->thread_detach_key (key);
              }
            return 0;
          }
        return -1;*/
        return 0;
      }
    return -1;
#   elif defined (ACE_HAS_THREAD_SPECIFIC_STORAGE)
      return OS::thr_setspecific_native (key, data);
#   else /* ACE_HAS_TSS_EMULATION */
      ACE_UNUSED_ARG (key);
      ACE_UNUSED_ARG (data);
      ACE_NOTSUP_RETURN (-1);
#   endif /* ACE_HAS_TSS_EMULATION */
# else /* ACE_HAS_THREADS */
  ACE_UNUSED_ARG (key);
  ACE_UNUSED_ARG (data);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_THREADS */
}

void
OS::unique_name (const void *object,
                     char *name,
                     size_t length)
{
  // The process ID will provide uniqueness between processes on the
  // same machine. The "this" pointer of the <object> will provide
  // uniqueness between other "live" objects in the same process. The
  // uniqueness of this name is therefore only valid for the life of
  // <object>.
  char temp_name[ACE_UNIQUE_NAME_LEN];
  OS::sprintf (temp_name,
                   "%p%d",
                   object,
                   static_cast <int> (OS::getpid ()));
  OS::strsncpy (name,
                    temp_name,
                    length);
}

#if defined (ACE_USES_WCHAR)
void
OS::unique_name (const void *object,
                     wchar_t *name,
                     size_t length)
{
  // The process ID will provide uniqueness between processes on the
  // same machine. The "this" pointer of the <object> will provide
  // uniqueness between other "live" objects in the same process. The
  // uniqueness of this name is therefore only valid for the life of
  // <object>.
  wchar_t temp_name[ACE_UNIQUE_NAME_LEN];
  OS::sprintf (temp_name,
                   ACE_TEXT ("%p%d"),
                   object,
                   static_cast <int> (OS::getpid ()));
  OS::strsncpy (name,
                    temp_name,
                    length);
}
#endif



#if defined (ACE_VXWORKS) && !defined (__RTP__)
# include /**/ <usrLib.h>   /* for ::sp() */
# include /**/ <sysLib.h>   /* for ::sysClkRateGet() */

// This global function can be used from the VxWorks shell to pass
// arguments to a C main () function.
//
// usage: -> spa main, "arg1", "arg2"
//
// All arguments must be quoted, even numbers.
int
spa (FUNCPTR entry, ...)
{
  static const unsigned int ACE_MAX_ARGS = 10;
  static char *argv[ACE_MAX_ARGS];
  va_list pvar;
  unsigned int argc;

  // Hardcode a program name because the real one isn't available
  // through the VxWorks shell.
  argv[0] = "ace_main";

  // Peel off arguments to spa () and put into argv.  va_arg () isn't
  // necessarily supposed to return 0 when done, though since the
  // VxWorks shell uses a fixed number (10) of arguments, it might 0
  // the unused ones.  This function could be used to increase that
  // limit, but then it couldn't depend on the trailing 0.  So, the
  // number of arguments would have to be passed.
  va_start (pvar, entry);

  for (argc = 1; argc <= ACE_MAX_ARGS; ++argc)
    {
      argv[argc] = va_arg (pvar, char *);

      if (argv[argc] == 0)
        break;
    }

  if (argc > ACE_MAX_ARGS  &&  argv[argc-1] != 0)
    {
      // try to read another arg, and warn user if the limit was exceeded
      if (va_arg (pvar, char *) != 0)
        OS::fprintf (stderr, "spa(): number of arguments limited to %d\n",
                         ACE_MAX_ARGS);
    }
  else
    {
      // fill unused argv slots with 0 to get rid of leftovers
      // from previous invocations
      for (unsigned int i = argc; i <= ACE_MAX_ARGS; ++i)
        argv[i] = 0;
    }

  // The hard-coded options are what ::sp () uses, except for the
  // larger stack size (instead of ::sp ()'s 20000).
  const int ret = ::taskSpawn (argv[0],    // task name
                               100,        // task priority
                               VX_FP_TASK, // task options
                               ACE_NEEDS_HUGE_THREAD_STACKSIZE, // stack size
                               entry,      // entry point
                               argc,       // first argument to main ()
                               (int) argv, // second argument to main ()
                               0, 0, 0, 0, 0, 0, 0, 0);
  va_end (pvar);

  // ::taskSpawn () returns the taskID on success: return 0 instead if
  // successful
  return ret > 0 ? 0 : ret;
}

// A helper function for the extended spa functions
static void
add_to_argv (int& argc, char** argv, int max_args, char* string)
{
  char indouble   = 0;
  size_t previous = 0;
  size_t length   = OS::strlen (string);

  // We use <= to make sure that we get the last argument
  for (size_t i = 0; i <= length; i++)
    {
      // Is it a double quote that hasn't been escaped?
      if (string[i] == '\"' && (i == 0 || string[i - 1] != '\\'))
        {
          indouble ^= 1;
          if (indouble)
            {
              // We have just entered a double quoted string, so
              // save the starting position of the contents.
              previous = i + 1;
            }
          else
            {
              // We have just left a double quoted string, so
              // zero out the ending double quote.
              string[i] = '\0';
            }
        }
      else if (string[i] == '\\')  // Escape the next character
        {
          // The next character is automatically
          // skipped because of the strcpy
          OS::strcpy (string + i, string + i + 1);
          length--;
        }
      else if (!indouble &&
               (OS::ace_isspace (string[i]) || string[i] == '\0'))
        {
          string[i] = '\0';
          if (argc < max_args)
            {
              argv[argc] = string + previous;
              argc++;
            }
          else
            {
              OS::fprintf (stderr, "spae(): number of arguments "
                                       "limited to %d\n", max_args);
            }

          // Skip over whitespace in between arguments
          for(++i; i < length && OS::ace_isspace (string[i]); ++i)
            {
            }

          // Save the starting point for the next time around
          previous = i;

          // Make sure we don't skip over a character due
          // to the above loop to skip over whitespace
          i--;
        }
    }
}

// This global function can be used from the VxWorks shell to pass
// arguments to a C main () function.
//
// usage: -> spae main, "arg1 arg2 \"arg3 with spaces\""
//
// All arguments must be within double quotes, even numbers.
int
spae (FUNCPTR entry, ...)
{
  static const int WINDSH_ARGS = 10;
  static const int ACE_MAX_ARGS    = 128;
  static char* argv[ACE_MAX_ARGS]  = { "ace_main", 0 };
  va_list pvar;
  int argc = 1;

  // Peel off arguments to spa () and put into argv.  va_arg () isn't
  // necessarily supposed to return 0 when done, though since the
  // VxWorks shell uses a fixed number (10) of arguments, it might 0
  // the unused ones.
  va_start (pvar, entry);

  int i = 0;
  for (char* str = va_arg (pvar, char*);
       str != 0 && i < WINDSH_ARGS; str = va_arg (pvar, char*), ++i)
    {
      add_to_argv(argc, argv, ACE_MAX_ARGS, str);
    }

  // fill unused argv slots with 0 to get rid of leftovers
  // from previous invocations
  for (i = argc; i < ACE_MAX_ARGS; ++i)
    argv[i] = 0;

  // The hard-coded options are what ::sp () uses, except for the
  // larger stack size (instead of ::sp ()'s 20000).
  const int ret = ::taskSpawn (argv[0],    // task name
                               100,        // task priority
                               VX_FP_TASK, // task options
                               ACE_NEEDS_HUGE_THREAD_STACKSIZE, // stack size
                               entry,      // entry point
                               argc,       // first argument to main ()
                               (int) argv, // second argument to main ()
                               0, 0, 0, 0, 0, 0, 0, 0);
  va_end (pvar);

  // ::taskSpawn () returns the taskID on success: return 0 instead if
  // successful
  return ret > 0 ? 0 : ret;
}

// This global function can be used from the VxWorks shell to pass
// arguments to a C main () function.  The function will be run
// within the shells task.
//
// usage: -> spaef main, "arg1 arg2 \"arg3 with spaces\""
//
// All arguments must be within double quotes, even numbers.
// Unlike the spae function, this fuction executes the supplied
// routine in the foreground, rather than spawning it in a separate
// task.
int
spaef (FUNCPTR entry, ...)
{
  static const int WINDSH_ARGS = 10;
  static const int ACE_MAX_ARGS    = 128;
  static char* argv[ACE_MAX_ARGS]  = { "ace_main", 0 };
  va_list pvar;
  int argc = 1;

  // Peel off arguments to spa () and put into argv.  va_arg () isn't
  // necessarily supposed to return 0 when done, though since the
  // VxWorks shell uses a fixed number (10) of arguments, it might 0
  // the unused ones.
  va_start (pvar, entry);

  int i = 0;
  for (char* str = va_arg (pvar, char*);
       str != 0 && i < WINDSH_ARGS; str = va_arg (pvar, char*), ++i)
    {
      add_to_argv(argc, argv, ACE_MAX_ARGS, str);
    }

  // fill unused argv slots with 0 to get rid of leftovers
  // from previous invocations
  for (i = argc; i < ACE_MAX_ARGS; ++i)
    argv[i] = 0;

  int ret = entry (argc, argv);

  va_end (pvar);

  // Return the return value of the invoked ace_main routine.
  return ret;
}

// This global function can be used from the VxWorks shell to pass
// arguments to and run a main () function (i.e. ace_main).
//
// usage: -> vx_execae ace_main, "arg1 arg2 \"arg3 with spaces\"", [prio, [opt, [stacksz]]]
//
// All arguments must be within double quotes, even numbers.
// This routine spawns the main () function in a separate task and waits till the
// task has finished.
static int _vx_call_rc = 0;

static int
_vx_call_entry(FUNCPTR entry, int argc, char* argv[])
{
    _vx_call_rc = entry (argc, argv);
    return _vx_call_rc;
}

int
vx_execae (FUNCPTR entry, char* arg, int prio, int opt, int stacksz, ...)
{
  static const int ACE_MAX_ARGS    = 128;
  static char* argv[ACE_MAX_ARGS]  = { "ace_main", 0 };
  int argc = 1;

  // Peel off arguments to run_main () and put into argv.

  if (arg)
    add_to_argv(argc, argv, ACE_MAX_ARGS, arg);

  // fill unused argv slots with 0 to get rid of leftovers
  // from previous invocations
  for (int i = argc; i < ACE_MAX_ARGS; ++i)
    argv[i] = 0;

  // The hard-coded options are what ::sp () uses, except for the
  // larger stack size (instead of ::sp ()'s 20000).
  const int ret = ::taskSpawn (argv[0],    // task name
                               prio==0 ? 100 : prio,        // task priority
                               opt==0 ? VX_FP_TASK : opt, // task options
                               stacksz==0 ? ACE_NEEDS_HUGE_THREAD_STACKSIZE : stacksz, // stack size
                               (FUNCPTR)_vx_call_entry, // entrypoint caller
                               (int)entry,              // entry point
                               argc,                    // first argument to main ()
                               (int) argv,              // second argument to main ()
                               0, 0, 0, 0, 0, 0, 0);

  while( ret > 0 && ::taskIdVerify (ret) != ERROR )
    ::taskDelay (3 * ::sysClkRateGet ());

  // ::taskSpawn () returns the taskID on success: return _vx_call_rc instead if
  // successful
  return ret > 0 ? _vx_call_rc : 255;
}
#endif /* ACE_VXWORKS && !__RTP__ */
