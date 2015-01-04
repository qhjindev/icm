#ifndef _ICC_HANDLE_SET_H_
#define _ICC_HANDLE_SET_H_

#include "os/os_include/sys/os_select.h"
#include "os/os_include/os_limits.h"

class HandleSet
{
public:
  friend class HandleSetIterator;

  enum
  {
    MAXSIZE = 1024
  };

  HandleSet (void);

  void reset (void);

  int isSet (ACE_HANDLE handle) const;

  void setBit (ACE_HANDLE handle);

  void clrBit (ACE_HANDLE handle);

  fd_set *fdset (void);

  operator fd_set *();

  void sync (ACE_HANDLE max);

  ACE_HANDLE maxSet (void) const;

private:
    enum
  {
    WORDSIZE = NFDBITS,
#if !defined (ACE_WIN32)
    NUM_WORDS = howmany (MAXSIZE, NFDBITS),
#endif /* ACE_WIN32 */
    NBITS = 256
  };

  static int countBits (unsigned long n);
  static const char nbits[NBITS];

  void setMax (ACE_HANDLE max);

  int m_size;
  fd_set m_mask;
  ACE_HANDLE m_maxHandle;
};

class HandleSetIterator
{
public:
  /// Constructor.
  HandleSetIterator (const HandleSet &hs);

  /// Default dtor.
  ~HandleSetIterator (void);

  /// Reset the state of the iterator by reinitializing the state
  /// that we maintain.
  void resetState (void);

  /**
   * "Next" operator.  Returns the next unseen ACE_HANDLE in the
   * <Handle_Set> up to <handle_set_.max_handle_>).  When all the
   * handles have been seen returns <ACE_INVALID_HANDLE>.  Advances
   * the iterator automatically, so you need not call <operator++>
   * (which is now obsolete).
   */
  ACE_HANDLE operator () (void);

private:
  /// The <HandleSet> we are iterating through.
  const HandleSet &m_handles;

  /// Index of the bit we're examining in the current <word_num_> word.
#if defined (ACE_WIN32)
  u_int m_handleIndex;
#elif !defined (ACE_HAS_BIG_FD_SET)
  int m_handleIndex;
#endif /* ACE_WIN32 */

  /// Number of the word we're iterating over (typically between 0..7).
  int m_wordNum;

#if !defined (ACE_WIN32) && !defined (ACE_HAS_BIG_FD_SET)
  /// Value of the bits in the word we're iterating on.
  fd_mask m_wordVal;
#endif /* !ACE_WIN32 && !ACE_HAS_BIG_FD_SET */
};

#endif //_ICC_HANDLE_SET_H_
