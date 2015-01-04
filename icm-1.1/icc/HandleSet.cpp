//#include "os/OS.h"
#include "icc/HandleSet.h"
#include "os/Global_Macros.h"
#include "memory.h"

#if defined (ACE_WIN32)
   //  Does ACE_WIN32 have an fd_mask?
#  define ACE_MSB_MASK (~(1 << (NFDBITS - 1)))
#else  /* ! ACE_WIN32 */
#  define ACE_MSB_MASK (~((fd_mask) 1 << (NFDBITS - 1)))
#endif /* ! ACE_WIN32 */


const char HandleSet::nbits[256] =
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

 int
HandleSet::countBits (unsigned long n)
{
#if defined (ACE_HAS_HANDLE_SET_OPTIMIZED_FOR_SELECT)
  register int rval = 0;

  // Count the number of enabled bits in <n>.  This algorithm is very
  // fast, i.e., O(enabled bits in n).

  for (register u_long m = n;
       m != 0;
       m &= m - 1)
    rval++;

  return rval;
#else
   return (HandleSet::nbits[n & 0xff]
          + HandleSet::nbits[(n >> 8) & 0xff]
          + HandleSet::nbits[(n >> 16) & 0xff]
          + HandleSet::nbits[(n >> 24) & 0xff]);
#endif /* ACE_HAS_HANDLE_SET_OPTIMIZED_FOR_SELECT */
}

HandleSet::HandleSet (void)
{
  this->reset ();
}

#define ACE_DIV_BY_WORDSIZE(x) ((x) / ((int) HandleSet::WORDSIZE))
#define ACE_MULT_BY_WORDSIZE(x) ((x) * ((int) HandleSet::WORDSIZE))

void
HandleSet::sync (ACE_HANDLE max)
{
#if !defined (ACE_WIN32)
  fd_mask *maskp = (fd_mask *)(this->m_mask.fds_bits);
  this->m_size = 0;

  for (int i = ACE_DIV_BY_WORDSIZE (max - 1);
       i >= 0;
       i--)
    this->m_size += HandleSet::countBits (maskp[i]);

  this->setMax (max);
#else
  ACE_UNUSED_ARG (max);
#endif /* !ACE_WIN32 */
}

void HandleSet::reset (void)
{
  this->m_maxHandle = ACE_INVALID_HANDLE;
  this->m_size = 0;
  FD_ZERO (&this->m_mask);
}

int HandleSet::isSet (ACE_HANDLE handle) const
{
  return FD_ISSET (handle, &this->m_mask);
}

void HandleSet::setBit (ACE_HANDLE handle)
{
  if ((handle != ACE_INVALID_HANDLE) & (!this->isSet (handle))) {
#if defined (ACE_WIN32)
    FD_SET ((SOCKET) handle, &this->m_mask);
    ++this->m_size;
#else
    FD_SET (handle, &this->m_mask);
    ++this->m_size;
    if (handle > this->m_maxHandle)
      this->m_maxHandle = handle;
#endif // ACE_WIN32
  }
}

void HandleSet::clrBit (ACE_HANDLE handle)
{
  if ((handle != ACE_INVALID_HANDLE) && (this->isSet (handle))) {

    FD_CLR ((ACE_SOCKET) handle, &this->m_mask);
    --this->m_size;

#if !defined (ACE_WIN32)
      if (handle == this->m_maxHandle)
        this->setMax (this->m_maxHandle);
#endif /* !ACE_WIN32 */
  }
}

fd_set * HandleSet::fdset (void)
{
  if (this->m_size > 0)
    return (fd_set *) &this->m_mask;
  else
    return (fd_set *) 0;
}

HandleSet::operator fd_set *()
{
  if (this->m_size > 0)
    return (fd_set *) &this->m_mask;
  else
    return (fd_set *) 0;
}

ACE_HANDLE
HandleSet::maxSet (void) const
{
  return this->m_maxHandle;
}

void
HandleSet::setMax (ACE_HANDLE currentMax)
{
  #if !defined(ACE_WIN32)
    fd_mask * maskp = (fd_mask *)(this->m_mask.fds_bits);

  if (this->m_size == 0)
    this->m_maxHandle = ACE_INVALID_HANDLE;
  else
  {
    int i;

    for (i = ACE_DIV_BY_WORDSIZE (currentMax - 1);
         maskp[i] == 0;
         i--)
       continue;

       this->m_maxHandle = ACE_MULT_BY_WORDSIZE (i);
       for (fd_mask val = maskp[i];
         (val & ~1) != 0; // This obscure code is needed since "bit 0" is in location 1...
          val = (val >> 1) & ACE_MSB_MASK)
        ++this->m_maxHandle;
  }

  // Do some sanity checking...
  if (this->m_maxHandle >= HandleSet::MAXSIZE)
    this->m_maxHandle = HandleSet::MAXSIZE - 1;
  #else
    ACE_UNUSED_ARG (currentMax);
  #endif /* !ACE_WIN32 */
}

ACE_HANDLE
HandleSetIterator::operator () (void)
{
#if defined (ACE_WIN32)
  if (this->m_handleIndex < this->m_handles.m_mask.fd_count)
    // Return the handle and advance the iterator.
    return (ACE_HANDLE) this->m_handles.m_mask.fd_array[this->m_handleIndex++];
  else
    return ACE_INVALID_HANDLE;

#elif !defined (ACE_HAS_BIG_FD_SET) /* !ACE_WIN32 */
  // No sense searching further than the m_maxHandle + 1;
  ACE_HANDLE maxhandlep1 = this->m_handles.m_maxHandle + 1;

  fd_mask * maskp = (fd_mask *)(this->m_handles.m_mask.fds_bits);

  if (this->m_handleIndex >= maxhandlep1)
    // We've seen all the handles we're interested in seeing for this
    // iterator.
    return ACE_INVALID_HANDLE;
  else
    {
      ACE_HANDLE result = this->m_handleIndex;

      // Increment the iterator and advance to the next bit in this
      // word.
      this->m_handleIndex++;
      this->m_wordVal = (this->m_wordVal >> 1) & ACE_MSB_MASK;

      // If we've examined all the bits in this word, we'll go onto
      // the next word.

      if (this->m_wordVal == 0)
        {
          // Start the m_handleIndex at the beginning of the next word
          // and then loop until we've found the first non-zero bit or
          // we run past the <maxhandlep1> of the bitset.

          for (this->m_handleIndex = ACE_MULT_BY_WORDSIZE(++this->m_wordNum);
               this->m_handleIndex < maxhandlep1
                 && maskp[this->m_wordNum] == 0;
               this->m_wordNum++)
            this->m_handleIndex += HandleSet::WORDSIZE;

          // If the bit index becomes >= the maxhandlep1 that means
          // there weren't any more bits set that we want to consider.
          // Therefore, we'll just store the maxhandlep1, which will
          // cause <operator()> to return <ACE_INVALID_HANDLE>
          // immediately next time it's called.
          if (this->m_handleIndex >= maxhandlep1)
            {
              this->m_handleIndex = maxhandlep1;
              return result;
            }
          else
            // Load the bits of the next word.
            this->m_wordVal = maskp[this->m_wordNum];
        }

      // Loop until we get <m_wordVal> to have its least significant
      // bit enabled, keeping track of which <handle_index> this
      // represents (this information is used by subsequent calls to
      // <operator()>).

      for (;
           ACE_BIT_DISABLED (this->m_wordVal, 1);
           this->m_handleIndex++)
        this->m_wordVal = (this->m_wordVal >> 1) & ACE_MSB_MASK;

      return result;
    }

#endif /* ACE_WIN32 */
}

HandleSetIterator::HandleSetIterator (const HandleSet &hs)
  : m_handles (hs),
#if !defined (ACE_HAS_BIG_FD_SET) || defined (ACE_WIN32)
    m_handleIndex (0),
    m_wordNum (-1)
#endif
{
#if !defined (ACE_WIN32) && !defined (ACE_HAS_BIG_FD_SET)
  // No sense searching further than the m_maxHandle + 1;
  ACE_HANDLE maxhandlep1 =
    this->m_handles.m_maxHandle + 1;

  fd_mask *maskp =
    (fd_mask *)(this->m_handles.m_mask.fds_bits);

  // Loop until we've found the first non-zero bit or we run past the
  // <maxhandlep1> of the bitset.
  while (this->m_handleIndex < maxhandlep1
         && maskp[++this->m_wordNum] == 0)
    this->m_handleIndex += HandleSet::WORDSIZE;

  // If the bit index becomes >= the maxhandlep1 that means there
  // weren't any bits set.  Therefore, we'll just store the
  // maxhandlep1, which will cause <operator()> to return
  // <ACE_INVALID_HANDLE> immediately.
  if (this->m_handleIndex >= maxhandlep1)
    this->m_handleIndex = maxhandlep1;
  else
    // Loop until we get <m_wordVal> to have its least significant bit
    // enabled, keeping track of which <handle_index> this represents
    // (this information is used by <operator()>).
    for (this->m_wordVal = maskp[this->m_wordNum];
         ACE_BIT_DISABLED (this->m_wordVal, 1)
           && this->m_handleIndex < maxhandlep1;
         this->m_handleIndex++)
      this->m_wordVal = (this->m_wordVal >> 1) & ACE_MSB_MASK;

#endif /* !ACE_WIN32 && !ACE_HAS_BIG_FD_SET */
}

HandleSetIterator::~HandleSetIterator (void)
{

}

void
HandleSetIterator::resetState (void)
{
#if !defined (ACE_HAS_BIG_FD_SET) || defined (ACE_WIN32)
  this->m_handleIndex  = 0;
  this->m_wordNum = -1;
#endif /* ACE_HAS_BIG_FD_SET */

#if !defined (ACE_WIN32) && !defined (ACE_HAS_BIG_FD_SET)
  // No sense searching further than the m_maxHandle + 1;
  ACE_HANDLE maxhandlep1 =
    this->m_handles.m_maxHandle + 1;

  fd_mask *maskp =
    (fd_mask *)(this->m_handles.m_mask.fds_bits);

  // Loop until we've found the first non-zero bit or we run past the
  // <maxhandlep1> of the bitset.
  while (this->m_handleIndex < maxhandlep1
         && maskp[++this->m_wordNum] == 0)
    this->m_handleIndex += HandleSet::WORDSIZE;

  // If the bit index becomes >= the maxhandlep1 that means there
  // weren't any bits set.  Therefore, we'll just store the
  // maxhandlep1, which will cause <operator()> to return
  // <ACE_INVALID_HANDLE> immediately.
  if (this->m_handleIndex >= maxhandlep1)
    this->m_handleIndex = maxhandlep1;
  else
    // Loop until we get <m_wordVal> to have its least significant bit
    // enabled, keeping track of which <handle_index> this represents
    // (this information is used by <operator()>).

    for (this->m_wordVal = maskp[this->m_wordNum];
         ACE_BIT_DISABLED (this->m_wordVal, 1)
           && this->m_handleIndex < maxhandlep1;
         this->m_handleIndex++)
      this->m_wordVal = (this->m_wordVal >> 1) & ACE_MSB_MASK;

#endif /* !ACE_WIN32 && !ACE_HAS_BIG_FD_SET */
}
