#ifndef ICC_TIMER_HEAP_H
#define ICC_TIMER_HEAP_H

#include "TimerQueue.h"

// Forward declaration
template <class TYPE, class FUNCTOR, class LOCK>
class TimerHeapT;

/**
 * @class TimerHeapIteratorT
 *
 * @brief Iterates over an TimerHeapT.
 *
 * This is a generic iterator that can be used to visit every
 * node of a timer queue.  Be aware that it doesn't transverse
 * in the order of timeout values.
 */
template <class TYPE, class FUNCTOR, class LOCK>
class TimerHeapIteratorT : public TimerQueueIteratorT<TYPE, FUNCTOR, LOCK>
{
public:
  /// Constructor.
  TimerHeapIteratorT (TimerHeapT<TYPE, FUNCTOR, LOCK> &);

  /// Destructor.
  ~TimerHeapIteratorT (void);

  /// Positions the iterator at the earliest node in the Timer Queue
  virtual void first (void);

  /// Positions the iterator at the next node in the Timer Queue
  virtual void next (void);

  /// Returns true when there are no more nodes in the sequence
  virtual bool isdone (void) const;

  /// Returns the node at the current position in the sequence
  virtual TimerNodeT<TYPE> *item (void);

protected:
  /// Pointer to the ACE_Timer_Heap that we are iterating over.
  TimerHeapT<TYPE, FUNCTOR, LOCK> &mTimerHeap;

  /// Position in the array where the iterator is at
  size_t mPosition;
};

/**
 * @class TimerHeapT
 *
 * @brief Provides a very fast and predictable timer implementation.
 *
 * This implementation uses a heap-based callout queue of
 * absolute times.  Therefore, in the average and worst case,
 * scheduling, canceling, and expiring timers is O(log N) (where
 * N is the total number of timers).  In addition, we can also
 * preallocate as many @c ACE_Timer_Node objects as there are slots
 * in the heap.  This allows us to completely remove the need for
 * dynamic memory allocation, which is important for real-time
 * systems.
 */
template <class TYPE, class FUNCTOR, class LOCK>
class TimerHeapT : public TimerQueueT<TYPE, FUNCTOR, LOCK>
{
public:
  typedef TimerHeapIteratorT<TYPE, FUNCTOR, LOCK> HEAP_ITERATOR;
  friend class TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>;

  typedef TimerQueueT<TYPE, FUNCTOR, LOCK> INHERITED;

  // = Initialization and termination methods.
  /**
   * The Constructor creates a heap with specified number of elements.
   *
   * @param size The maximum number of timers that can be
   * inserted into the new object.
   */
  TimerHeapT (size_t size);

  /**
   * Default constructor. The default
   * size will be ACE_DEFAULT_TIMERS and there will be no preallocation.
   */
  TimerHeapT ();

  /// Destructor.
  virtual ~TimerHeapT (void);

  /// True if heap is empty, else false.
  virtual bool isEmpty (void) const;

  /// Returns the time of the earliest node in the Timer_Queue.
  /// Must be called on a non-empty queue.
  virtual const TimeValue &earliestTime (void) const;

  /**
   * Resets the interval of the timer represented by @a timer_id to
   * @a interval, which is specified in relative time to the current
   * <gettimeofday>.  If @a interval is equal to
   * TimeValue::zero, the timer will become a non-rescheduling
   * timer.  Returns 0 if successful, -1 if not.
   */
  virtual int resetInterval (long timer_id,
                              const TimeValue &interval);

  /**
   * Cancel all timers associated with @a type.  If @a dont_call_handle_close
   * is 0 then the <functor> will be invoked.  Returns number of timers
   * cancelled.
   */
  virtual int cancel (const TYPE &type,
                      int dont_call_handle_close = 1);

  /**
   * Cancel the single timer that matches the @a timer_id value (which
   * was returned from the <schedule> method).  If act is non-NULL
   * then it will be set to point to the ``magic cookie'' argument
   * passed in when the timer was registered.  This makes it possible
   * to free up the memory and avoid memory leaks. If @a dont_call_handle_close
   * is 0 then the <functor> will be invoked.  Returns 1 if cancellation
   * succeeded and 0 if the @a timer_id wasn't found.
   */
  virtual int cancel (long timer_id,
                      const void **act = 0,
                      int dont_call_handle_close = 1);

  /// Returns a pointer to this ACE_Timer_Queue's iterator.
  virtual TimerQueueIteratorT<TYPE, FUNCTOR, LOCK> &iter (void);

  /**
   * Removes the earliest node from the queue and returns it. Note that
   * the timer is removed from the heap, but is not freed, and its ID
   * is not reclaimed. The caller is responsible for calling either
   * @c reschedule() or @c free_node() after this function returns. Thus,
   * this function is for support of @c ACE_Timer_Queue::expire and
   * should not be used unadvisedly in other conditions.
   */
  TimerNodeT <TYPE> *removeFirst (void);

  /// Dump the state of an object.
  virtual void dump (void) const;

  /// Reads the earliest node from the queue and returns it.
  virtual TimerNodeT<TYPE> *getFirst (void);

protected:

  /**
   * Schedule a timer that may optionally auto-reset.
   * Schedule @a type that will expire at @a future_time,
   * which is specified in absolute time.  If it expires then @a act is
   * passed in as the value to the <functor>.  If @a interval is != to
   * TimeValue::zero then it is used to reschedule the @a type
   * automatically, using relative time to the current <gettimeofday>.
   * This method returns a <timer_id> that uniquely identifies the the
   * @a type entry in an internal list.  This <timer_id> can be used to
   * cancel the timer before it expires.  The cancellation ensures
   * that <timer_ids> are unique up to values of greater than 2
   * billion timers.  As long as timers don't stay around longer than
   * this there should be no problems with accidentally deleting the
   * wrong timer.  Returns -1 on failure (which is guaranteed never to
   * be a valid <timer_id>).
   */
  virtual long scheduleI (const TYPE &type,
                           const void *act,
                           const TimeValue &future_time,
                           const TimeValue &interval);

  /// Reschedule an "interval" ACE_Timer_Node.
  virtual void reschedule (TimerNodeT<TYPE> *);

  /// Factory method that allocates a new node (uses operator new if
  /// we're *not* preallocating, otherwise uses an internal freelist).
  virtual TimerNodeT<TYPE> *allocNode (void);

  /**
   * Factory method that frees a previously allocated node (uses
   * operator delete if we're *not* preallocating, otherwise uses an
   * internal freelist).
   */
  virtual void freeNode (TimerNodeT<TYPE> *);

private:
  /// Remove and return the @a sloth ACE_Timer_Node and restore the
  /// heap property.
  TimerNodeT<TYPE> *remove (size_t slot);

  /// Insert @a new_node into the heap and restore the heap property.
  void insert (TimerNodeT<TYPE> *new_node);

  /**
   * Doubles the size of the heap and the corresponding timer_ids array.
   * If preallocation is used, will also double the size of the
   * preallocated array of ACE_Timer_Nodes.
   */
  void growHeap (void);

  /// Restore the heap property, starting at @a slot.
  void reheapUp (TimerNodeT<TYPE> *new_node,
                  size_t slot,
                  size_t parent);

  /// Restore the heap property, starting at @a slot.
  void reheapDown (TimerNodeT<TYPE> *moved_node,
                    size_t slot,
                    size_t child);

  /// Copy @a moved_node into the @a slot slot of <mHeap> and move
  /// @a slot into the corresponding slot in the <timer_id_> array.
  void copy (size_t slot, TimerNodeT<TYPE> *moved_node);

  /**
   * Returns a timer id that uniquely identifies this timer.  This id
   * can be used to cancel a timer via the <cancel (int)> method.  The
   * timer id returned from this method will never == -1 to avoid
   * conflicts with other failure return values.
   */
  long timerId (void);

  /// Pops and returns a new timer id from the freelist.
  long popFreelist (void);

  /// Pushes @a old_id onto the freelist.
  void pushFreelist (long old_id);

  /// Maximum size of the heap.
  size_t mMaxSize;

  /// Current size of the heap.
  size_t mCurSize;

  /// Number of heap entries in transition (removed from the queue, but
  /// not freed) and may be rescheduled or freed.
  size_t mCurLimbo;

  /// Iterator used to expire timers.
  HEAP_ITERATOR *mIterator;

  /**
   * Current contents of the Heap, which is organized as a "heap" of
   * ACE_Timer_Node *'s.  In this context, a heap is a "partially
   * ordered, almost complete" binary tree, which is stored in an
   * array.
   */
  TimerNodeT<TYPE> **mHeap;

  /**
   * An array of "pointers" that allows each ACE_Timer_Node in the
   * <mHeap> to be located in O(1) time.  Basically, <timer_id_[i]>
   * contains the slot in the <mHeap> array where an ACE_Timer_Node
   * * with timer id \<i\> resides.  Thus, the timer id passed back from
   * <schedule> is really a slot into the <timer_ids> array.  The
   * <mTimerIds> array serves two purposes: negative values are
   * indications of free timer IDs, whereas positive values are
   * "pointers" into the <mHeap> array for assigned timer IDs.
   */
  ssize_t *mTimerIds;

  /// "Pointer" to the element in the <mTimerIds> array that was
  /// last given out as a timer ID.
  size_t mTimerIdsCurr;

  /// Index representing the lowest timer ID that has been freed. When
  /// the timer_ids_next_ value wraps around, it starts back at this
  /// point.
  size_t mTimerIdsMinFree;

};

#if defined (ACE_TEMPLATES_REQUIRE_SOURCE)
#include "icc/TimerHeap.cpp"
#endif /* ACE_TEMPLATES_REQUIRE_SOURCE */

#if defined (ACE_TEMPLATES_REQUIRE_PRAGMA)
#pragma implementation ("TimerHeap.cpp")
#endif /* ACE_TEMPLATES_REQUIRE_PRAGMA */

typedef TimerHeapT<EventHandler *,
                   EventHandlerHandleTimeoutUpcall<SYNCH_RECURSIVE_MUTEX>,
                   SYNCH_RECURSIVE_MUTEX>
        TimerHeap;

typedef TimerHeapIteratorT<EventHandler *,
                           EventHandlerHandleTimeoutUpcall<SYNCH_RECURSIVE_MUTEX>,
                           SYNCH_RECURSIVE_MUTEX>
        TimerHeapIterator;

#endif //ICC_TIMER_HEAP_H
