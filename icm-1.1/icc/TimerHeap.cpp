
#ifndef TimerHeapT_CPP
#define TimerHeapT_CPP

#include "icc/TimerHeap.h"
#include "icc/Guard.h"
#include "os/OS_NS_errno.h"
#include "os/OS_NS_string.h"
//#include "icc/NumericLimits.h"
# include <limits>

#undef min
#undef max

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/*
** The TimerHeap::mMaxSize and array loops, checks, etc. are all size_t.
** The timer IDs are long, and since they are indices into the heap, we need
** to be sure that the timer heap size can fit in a long. Hence, when size
** is (re)set, limit it to the maximum long value. We use the C++ standard
** limits if available.
*/

// Define some simple inlined functions to clarify the code.
inline size_t
HEAP_PARENT (size_t X)
{
  return (X == 0 ? 0 : ((X - 1) / 2));
}

inline size_t
HEAP_LCHILD (size_t X)
{
  return X + X + 1;
}

// Constructor that takes in an <TimerHeapT> to iterate over.

template <class TYPE, class FUNCTOR, class LOCK>
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>::TimerHeapIteratorT (
  TimerHeapT<TYPE, FUNCTOR, LOCK> &heap)
  : mTimerHeap (heap)
{
  this->first ();
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>::~TimerHeapIteratorT (void)
{
}

// Positions the iterator at the first node in the heap array

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>::first (void)
{
  this->mPosition = 0;
}

// Positions the iterator at the next node in the heap array

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>::next (void)
{
  if (this->mPosition != this->mTimerHeap.mCurSize)
    ++this->mPosition;
}

// Returns true the <mPosition> is at the end of the heap array

template <class TYPE, class FUNCTOR, class LOCK> bool
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>::isdone (void) const
{
  return this->mPosition == this->mTimerHeap.mCurSize;
}

// Returns the node at the current position in the heap or 0 if at the end

template <class TYPE, class FUNCTOR, class LOCK> TimerNodeT<TYPE> *
TimerHeapIteratorT<TYPE, FUNCTOR, LOCK>::item (void)
{
  if (this->mPosition != this->mTimerHeap.mCurSize)
    return this->mTimerHeap.mHeap[this->mPosition];
  return 0;
}

// Constructor
// Note that mTimerIdsCurr and mTimerIdsMinFree both start at 0.
// Since timer IDs are assigned by first incrementing the mTimerIdsCurr
// value, the first ID assigned will be 1 (just as in the previous design).
// When it's time to wrap, the next ID given out will be 0.
template <class TYPE, class FUNCTOR, class LOCK>
TimerHeapT<TYPE, FUNCTOR, LOCK>::TimerHeapT (size_t size)
  : mMaxSize (size),
    mCurSize (0),
    mCurLimbo (0),
    mTimerIdsCurr (0),
    mTimerIdsMinFree (0)
{
  // Possibly reduce size to fit in a long.
  if (size > static_cast<size_t> (std::numeric_limits<long>::max ()))
    {
      size = static_cast<size_t> (std::numeric_limits<long>::max ());
      this->mMaxSize = size;
    }

  // Create the heap array.
  this->mHeap = new TimerNodeT<TYPE> *[size];

  // Create the parallel
  this->mTimerIds = new ssize_t[size];

  // Initialize the "freelist," which uses negative values to
  // distinguish freelist elements from "pointers" into the <mHeap>
  // array.
  for (size_t i = 0; i < size; ++i)
    this->mTimerIds[i] = -1;

  mIterator = new HEAP_ITERATOR (*this);
}

// Note that mTimerIdsCurr and mTimerIdsMinFree both start at 0.
// Since timer IDs are assigned by first incrementing the mTimerIdsCurr
// value, the first ID assigned will be 1 (just as in the previous design).
// When it's time to wrap, the next ID given out will be 0.
template <class TYPE, class FUNCTOR, class LOCK>
TimerHeapT<TYPE, FUNCTOR, LOCK>::TimerHeapT ()
  : mMaxSize (ACE_DEFAULT_TIMERS),
    mCurSize (0),
    mCurLimbo (0),
    mTimerIdsCurr (0),
    mTimerIdsMinFree (0)
{
  // Possibly reduce size to fit in a long.
  if (this->mMaxSize > static_cast<size_t> (std::numeric_limits<long>::max ()))
    this->mMaxSize = static_cast<size_t> (std::numeric_limits<long>::max ());

  // Create the heap array.
  this->mHeap = new TimerNodeT<TYPE> *[mMaxSize];

  // Create the parallel
  this->mTimerIds = new ssize_t[mMaxSize];

  // Initialize the "freelist," which uses negative values to
  // distinguish freelist elements from "pointers" into the <mHeap>
  // array.
  for (size_t i = 0; i < mMaxSize; ++i)
    this->mTimerIds[i] = -1;

  mIterator = new HEAP_ITERATOR (*this);
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerHeapT<TYPE, FUNCTOR, LOCK>::~TimerHeapT (void)
{
  delete mIterator;

  size_t currentSize =  this->mCurSize;

  // Clean up all the nodes still in the queue
  for (size_t i = 0; i < currentSize; ++i)
    {
      // Grab the event_handler and act, then delete the node before calling
      // back to the handler. Prevents a handler from trying to cancel_timer()
      // inside handle_close(), ripping the current timer node out from
      // under us.
      TYPE eh = this->mHeap[i]->type ;
      const void *act = this->mHeap[i]->act;
      this->freeNode (this->mHeap[i]);
      this->mUpcallFunctor->deletion (*this, eh, act);
    }

  delete [] this->mHeap;
  delete [] this->mTimerIds;
}

template <class TYPE, class FUNCTOR, class LOCK>
long
TimerHeapT<TYPE, FUNCTOR, LOCK>::popFreelist (void)
{

  // Scan for a free timer ID. Note that since this function is called
  // _after_ the check for a full timer heap, we are guaranteed to find
  // a free ID, even if we need to wrap around and start reusing freed IDs.
  // On entry, the curr_ index is at the previous ID given out; start
  // up where we left off last time.
  // NOTE - a mTimerIds slot with -2 is out of the heap, but not freed.
  // It must be either freed (free_node) or rescheduled (reschedule).
  ++this->mTimerIdsCurr;
  while (this->mTimerIdsCurr < this->mMaxSize &&
         (this->mTimerIds[this->mTimerIdsCurr] >= 0 ||
          this->mTimerIds[this->mTimerIdsCurr] == -2  ))
    ++this->mTimerIdsCurr;
  if (this->mTimerIdsCurr == this->mMaxSize)
    {
      //ACE_ASSERT (this->mTimerIdsMinFree < this->mMaxSize);
      this->mTimerIdsCurr = this->mTimerIdsMinFree;
      // We restarted the free search at min. Since min won't be
      // free anymore, and curr_ will just keep marching up the list
      // on each successive need for an ID, reset min_free_ to the
      // size of the list until an ID is freed that curr_ has already
      // gone past (see push_freelist).
      this->mTimerIdsMinFree = this->mMaxSize;
    }

  return static_cast<long> (this->mTimerIdsCurr);
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::pushFreelist (long old_id)
{

  // Since this ID has already been checked by one of the public
  // functions, it's safe to cast it here.
  size_t oldid = static_cast<size_t> (old_id);

  // The freelist values in the <mTimerIds> are negative, so set the
  // freed entry back to 'free'. If this is the new lowest value free
  // timer ID that curr_ won't see on it's normal march through the list,
  // remember it.
  //ACE_ASSERT (this->mTimerIds[oldid] >= 0 || this->mTimerIds[oldid] == -2);
  if (this->mTimerIds[oldid] == -2)
    --this->mCurLimbo;
  else
    --this->mCurSize;
  this->mTimerIds[oldid] = -1;
  if (oldid < this->mTimerIdsMinFree && oldid <= this->mTimerIdsCurr)
    this->mTimerIdsMinFree = oldid;
  return;
}

template <class TYPE, class FUNCTOR, class LOCK>
long
TimerHeapT<TYPE, FUNCTOR, LOCK>::timerId (void)
{

  // Return the next item off the freelist and use it as the timer id.
  return this->popFreelist ();
}

// Checks if queue is empty.

template <class TYPE, class FUNCTOR, class LOCK>
bool
TimerHeapT<TYPE, FUNCTOR, LOCK>::isEmpty (void) const
{
  return this->mCurSize == 0;
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerQueueIteratorT<TYPE, FUNCTOR, LOCK> &
TimerHeapT<TYPE, FUNCTOR, LOCK>::iter (void)
{
  this->mIterator->first ();
  return *this->mIterator;
}

// Returns earliest time in a non-empty queue.

template <class TYPE, class FUNCTOR, class LOCK> const TimeValue &
TimerHeapT<TYPE, FUNCTOR, LOCK>::earliestTime (void) const
{
  return this->mHeap[0]->timerValue;
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::dump (void) const
{
#if defined (ACE_HAS_DUMP)
  ACE_DEBUG ((LM_DEBUG, ACE_BEGIN_DUMP, this));

  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nmMaxSize = %d"), this->mMaxSize));
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nmCurSize = %d"), this->mCurSize));
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nmCurLimbo= %d"), this->mCurLimbo));
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nids_curr_ = %d"),
              this->mTimerIdsCurr));
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nmin_free_ = %d"),
              this->mTimerIdsMinFree));

  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\nheap_ =\n")));

  for (size_t i = 0; i < this->mCurSize; ++i)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%d\n"),
                  i));
      this->mHeap[i]->dump ();
    }

  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("\ntimer_ids_ =\n")));

  for (size_t j = 0; j < this->mMaxSize; ++j)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%d\t%d\n"),
                j,
                this->mTimerIds[j]));

  ACE_DEBUG ((LM_DEBUG, ACE_END_DUMP));
#endif /* ACE_HAS_DUMP */
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::copy (
  size_t slot,
  TimerNodeT<TYPE> *moved_node)
{
  // Insert <moved_node> into its new location in the heap.
  this->mHeap[slot] = moved_node;

  assert (moved_node->timerId  >= 0
              && moved_node->timerId < (int) this->mMaxSize);

  // Update the corresponding slot in the parallel <mTimerIds> array.
  this->mTimerIds[moved_node->timerId] = static_cast<ssize_t> (slot);
}

// Remove the slot'th timer node from the heap, but do not reclaim its
// timer ID or change the size of this timer heap object. The caller of
// this function must call either free_node (to reclaim the timer ID
// and the timer node memory, as well as decrement the size of the queue)
// or reschedule (to reinsert the node in the heap at a new time).
template <class TYPE, class FUNCTOR, class LOCK>
TimerNodeT<TYPE> *
TimerHeapT<TYPE, FUNCTOR, LOCK>::remove (size_t slot)
{
  TimerNodeT<TYPE> *removed_node = this->mHeap[slot];

  // NOTE - the mCurSize is being decremented since the queue has one
  // less active timer in it. However, this ACE_Timer_Node is not being
  // freed, and there is still a place for it in mTimerIds (the timer ID
  // is not being relinquished). The node can still be rescheduled, or
  // it can be freed via free_node.
  --this->mCurSize;

  // Only try to reheapify if we're not deleting the last entry.

  if (slot < this->mCurSize)
    {
      TimerNodeT<TYPE> *moved_node = this->mHeap[this->mCurSize];

      // Move the end node to the location being removed and update
      // the corresponding slot in the parallel <timer_ids> array.
      this->copy (slot, moved_node);

      // If the <moved_node->time_value_> is great than or equal its
      // parent it needs be moved down the heap.
      size_t parent = HEAP_PARENT (slot);

      if (moved_node->timerValue >= this->mHeap[parent]->timerValue)
        this->reheapDown (moved_node,
                           slot,
                           HEAP_LCHILD (slot));
      else
        this->reheapUp (moved_node,
                         slot,
                         parent);
    }

  this->mTimerIds[removed_node->timerId] = -2;
  ++this->mCurLimbo;
  return removed_node;
}

template <class TYPE, class FUNCTOR, class LOCK> void
TimerHeapT<TYPE, FUNCTOR, LOCK>::reheapDown (
  TimerNodeT<TYPE> *moved_node,
  size_t slot,
  size_t child)
{
  // Restore the heap property after a deletion.

  while (child < this->mCurSize)
    {
      // Choose the smaller of the two children.
      if (child + 1 < this->mCurSize
          && this->mHeap[child + 1]->timerValue < this->mHeap[child]->timerValue)
        child++;

      // Perform a <copy> if the child has a larger timeout value than
      // the <moved_node>.
      if (this->mHeap[child]->timerValue < moved_node->timerValue)
        {
          this->copy (slot, this->mHeap[child]);
          slot = child;
          child = HEAP_LCHILD (child);
        }
      else
        // We've found our location in the heap.
        break;
    }

  this->copy (slot, moved_node);
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::reheapUp (
  TimerNodeT<TYPE> *moved_node,
  size_t slot,
  size_t parent)
{
  // Restore the heap property after an insertion.

  while (slot > 0)
    {
      // If the parent node is greater than the <moved_node> we need
      // to copy it down.
      if (moved_node->timerValue < this->mHeap[parent]->timerValue)
        {
          this->copy (slot, this->mHeap[parent]);
          slot = parent;
          parent = HEAP_PARENT (slot);
        }
      else
        break;
    }

  // Insert the new node into its proper resting place in the heap and
  // update the corresponding slot in the parallel <timer_ids> array.
  this->copy (slot, moved_node);
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::insert (
  TimerNodeT<TYPE> *new_node)
{
  if (this->mCurSize + this->mCurLimbo + 2 >= this->mMaxSize)
    this->growHeap ();

  this->reheapUp (new_node,
                   this->mCurSize,
                   HEAP_PARENT (this->mCurSize));
  this->mCurSize++;
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::growHeap (void)
{
  // All the containers will double in size from mMaxSize.
  size_t new_size = this->mMaxSize * 2;

#if 0
  // Yikes - there's no way to flag a failure of going out of range of
  // a 'long' - this is a problem that should be addressed at some point.
  if (new_size > ACE_Numeric_Limits<long>::max ())
    new_size = ACE_Numeric_Limits<long>::max ();

  if (new_size <= this->mMaxSize)   // We are already at the limit
    {
      errno = ENOMEM;
      return -1;
    }
#endif /* 0 */

   // First grow the heap itself.

  TimerNodeT<TYPE> **new_heap = 0;

  new_heap = new TimerNodeT<TYPE> *[new_size];

  OS::memcpy (new_heap,
                  this->mHeap,
                  this->mMaxSize * sizeof *new_heap);
  delete [] this->mHeap;
  this->mHeap = new_heap;

  // Grow the array of timer ids.

  ssize_t *new_timer_ids = 0;

  new_timer_ids = new ssize_t[new_size];

  OS::memcpy (new_timer_ids,
                  this->mTimerIds,
                  this->mMaxSize * sizeof (ssize_t));

  delete [] mTimerIds;
  this->mTimerIds = new_timer_ids;

  // And add the new elements to the end of the "freelist".
  for (size_t i = this->mMaxSize; i < new_size; ++i)
    this->mTimerIds[i] = -(static_cast<ssize_t> (i) + 1);

  this->mMaxSize = new_size;
  // Force rescan of list from beginning for a free slot (I think...)
  // This fixed Bugzilla #2447.
  this->mTimerIdsMinFree = this->mMaxSize;
}

// Reschedule a periodic timer.  This function must be called with the
// mutex lock held.

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::reschedule (
  TimerNodeT<TYPE> *expired)
{

  // If we are rescheduling, then the most recent call was to
  // removeFirst (). That called remove () to remove the node from the
  // heap, but did not free the timer ID. The ACE_Timer_Node still has
  // its assigned ID - just needs to be inserted at the new proper
  // place, and the heap restored properly.
  if (this->mTimerIds[expired->timerId] == -2)
    --this->mCurLimbo;
  this->insert (expired);
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerNodeT<TYPE> *
TimerHeapT<TYPE, FUNCTOR, LOCK>::allocNode (void)
{
  return new TimerNodeT<TYPE>;
}

template <class TYPE, class FUNCTOR, class LOCK>
void
TimerHeapT<TYPE, FUNCTOR, LOCK>::freeNode (
  TimerNodeT<TYPE> *node)
{
  // Return this timer id to the freelist.
  this->pushFreelist (node->timerId);

  delete node;
}

// Insert a new timer that expires at time future_time; if interval is
// > 0, the handler will be reinvoked periodically.

template <class TYPE, class FUNCTOR, class LOCK>
long
TimerHeapT<TYPE, FUNCTOR, LOCK>::scheduleI (
  const TYPE &type,
  const void *act,
  const TimeValue &future_time,
  const TimeValue &interval)
{

  if ((this->mCurSize + this->mCurLimbo) < this->mMaxSize)
    {
      // Obtain the next unique sequence number.
      long const timer_id = this->timerId ();

      // Obtain the memory to the new node.
      TimerNodeT<TYPE> *temp = 0;

      temp = this->allocNode ();

      temp->set (type,
                 act,
                 future_time,
                 interval,
                 0,
                 timer_id);

      this->insert (temp);
      return timer_id;
    }
  else
    return -1;
}

// Locate and remove the single timer with a value of @a timer_id from
// the timer queue.

template <class TYPE, class FUNCTOR, class LOCK>
int
TimerHeapT<TYPE, FUNCTOR, LOCK>::cancel (long timer_id,
                                                   const void **act,
                                                   int dont_call)
{

  ACE_MT (ACE_GUARD_RETURN (LOCK, ace_mon, this->mMutex, -1));

  // Locate the ACE_Timer_Node that corresponds to the timer_id.

  // Check to see if the timer_id is out of range
  if (timer_id < 0
      || (size_t) timer_id > this->mMaxSize)
    return 0;

  ssize_t timer_node_slot = this->mTimerIds[timer_id];

  // Check to see if timer_id is still valid.
  if (timer_node_slot < 0)
    return 0;

  if (timer_id != this->mHeap[timer_node_slot]->timerId)
    {
      assert (timer_id == this->mHeap[timer_node_slot]->timerId);
      return 0;
    }
  else
    {
      TimerNodeT<TYPE> *temp =
        this->remove (timer_node_slot);

      this->mUpcallFunctor->cancellation(*this, temp->type, dont_call);

      if (act != 0)
        *act = temp->act;

      this->freeNode (temp);
      return 1;
    }
}

// Locate and update the inteval on the timer_id

template <class TYPE, class FUNCTOR, class LOCK>
int
TimerHeapT<TYPE, FUNCTOR, LOCK>::resetInterval (long timer_id,
                                                           const TimeValue &interval)
{
  ACE_MT (ACE_GUARD_RETURN (LOCK, ace_mon, this->mMutex, -1));

  // Locate the ACE_Timer_Node that corresponds to the timer_id.

  // Check to see if the timer_id is out of range
  if (timer_id < 0
      || (size_t) timer_id > this->mMaxSize)
    return -1;

  ssize_t timer_node_slot = this->mTimerIds[timer_id];

  // Check to see if timer_id is still valid.
  if (timer_node_slot < 0)
    return -1;

  if (timer_id != this->mHeap[timer_node_slot]->timerId)
    {
      assert (timer_id == this->mHeap[timer_node_slot]->timerId);
      return -1;
    }
  else
    {
      // Reset the timer interval
      this->mHeap[timer_node_slot]->interval = (interval);
      return 0;
    }
}

// Locate and remove all values of @a type from the timer queue.

template <class TYPE, class FUNCTOR, class LOCK>
int
TimerHeapT<TYPE, FUNCTOR, LOCK>::cancel (const TYPE &type,
                                                   int dont_call)
{

  ACE_MT (ACE_GUARD_RETURN (LOCK, ace_mon, this->mMutex, -1));

  int number_of_cancellations = 0;

  // Try to locate the ACE_Timer_Node that matches the timer_id.

  for (size_t i = 0; i < this->mCurSize; )
    {
      if (this->mHeap[i]->type == type)
        {
          TimerNodeT<TYPE> *temp = this->remove (i);

          ++number_of_cancellations;

          this->freeNode (temp);

          // We reset to zero so that we don't miss checking any nodes
          // if a reheapify occurs when a node is removed.  There
          // may be a better fix than this, however.
          i = 0;
        }
      else
        ++i;
    }

  this->mUpcallFunctor->cancellation (*this, type, dont_call);

  return number_of_cancellations;
}

// Returns the earliest node or returns 0 if the heap is empty.

template <class TYPE, class FUNCTOR, class LOCK>
TimerNodeT <TYPE> *
TimerHeapT<TYPE, FUNCTOR, LOCK>::removeFirst (void)
{

  if (this->mCurSize == 0)
    return 0;

  return this->remove (0);
}

template <class TYPE, class FUNCTOR, class LOCK>
TimerNodeT <TYPE> *
TimerHeapT<TYPE, FUNCTOR, LOCK>::getFirst (void)
{
  return this->mCurSize == 0 ? 0 : this->mHeap[0];
}

#endif /* TimerHeapT_CPP */
