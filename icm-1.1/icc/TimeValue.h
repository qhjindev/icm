#ifndef ICC_TIME_VALUE_H
#define ICC_TIME_VALUE_H

#include "os/os_include/os_time.h"
#include "os/Basic_Types.h"

// Define some helpful constants.
// Not type-safe, and signed.  For backward compatibility.
#define ACE_ONE_SECOND_IN_MSECS 1000L
suseconds_t const ACE_ONE_SECOND_IN_USECS = 1000000;
#define ACE_ONE_SECOND_IN_NSECS 1000000000L

class TimeValue
{
public:
  static const TimeValue zero;

  static const TimeValue maxTime;

  // Default Constructor.
  TimeValue (void);

  // Constructor
  explicit TimeValue (time_t sec, suseconds_t usec = 0);

  // Construct the TimeValue from a timeval.
  explicit TimeValue (const struct timeval &t);

  // Initializes the TimeValue from seconds and useconds.
  void set (time_t sec, suseconds_t usec);

  // Initializes the TimeValue from a timeval.
  void set (const timeval &t);

  void set (const timespec_t &tv);

# if defined (ACE_WIN32)
  explicit TimeValue (const FILETIME &ft);

  // Initializes the TimeValue from a WIN32 FILETIME
  void set (const FILETIME &ft);
# endif

  /// Converts from TimeValue format into milli-seconds format.
  /**
   * @return Sum of second field (in milliseconds) and microsecond field
   *         (in milliseconds).  Note that this method can overflow if
   *         the second and microsecond field values are large, so use
   *         the msec (ACE_UINT64 &ms) method instead.
   *
   * @note The semantics of this method differs from the sec() and
   *       usec() methods.  There is no analogous "millisecond"
   *       component in an ACE_Time_Value.
   */
  unsigned long msec (void) const;

  /// Converts from milli-seconds format into TimeValue format.
  /**
   * @note The semantics of this method differs from the sec() and
   *       usec() methods.  There is no analogous "millisecond"
   *       component in an TimeValue.
   */
  void msec (long);

  /// Returns the value of the object as a timespec_t.
  operator timespec_t () const;

  /// Returns the value of the object as a timeval.
  operator timeval () const;

  /// Returns a pointer to the object as a timeval.
  operator const timeval *() const;

  time_t sec (void) const;
  void sec (time_t sec);

  suseconds_t usec (void) const;
  void usec (suseconds_t usec);

  /// Add tv to this.
  TimeValue &operator += (const TimeValue &tv);

  /// Add  tv to this.
  TimeValue &operator += (time_t tv);

  /// Assign  tv to this
  TimeValue &operator = (const TimeValue &tv);

  /// Assign  tv to this
  TimeValue &operator = (time_t tv);

  /// Subtract  tv to this.
  TimeValue &operator -= (const TimeValue &tv);

  /// Substract  tv to this.
  TimeValue &operator -= (time_t tv);

  /// Increment microseconds as postfix.
  TimeValue operator++ (int);

  TimeValue &operator++ (void);

  TimeValue operator-- (int);

  TimeValue &operator-- (void);

  /// Adds two TimeValue objects together, returns the sum.
  friend ACE_Export TimeValue operator + (const TimeValue &tv1,
                                               const TimeValue &tv2);

  /// Subtracts two TimeValue objects, returns the difference.
  friend ACE_Export TimeValue operator - (const TimeValue &tv1,
                                               const TimeValue &tv2);

  /// True if @a tv1 < @a tv2.
  friend ACE_Export bool operator < (const TimeValue &tv1,
                                     const TimeValue &tv2);

  /// True if @a tv1 > @a tv2.
  friend ACE_Export bool operator > (const TimeValue &tv1,
                                     const TimeValue &tv2);

  /// True if @a tv1 <= @a tv2.
  friend ACE_Export bool operator <= (const TimeValue &tv1,
                                      const TimeValue &tv2);

  /// True if @a tv1 >= @a tv2.
  friend ACE_Export bool operator >= (const TimeValue &tv1,
                                      const TimeValue &tv2);

  /// True if @a tv1 == @a tv2.
  friend ACE_Export bool operator == (const TimeValue &tv1,
                                      const TimeValue &tv2);

  /// True if @a tv1 != @a tv2.
  friend ACE_Export bool operator != (const TimeValue &tv1,
                                      const TimeValue &tv2);

  //@{
  /// Multiplies the time value by @a d
  friend ACE_Export TimeValue operator * (double d, const TimeValue &tv);

  friend ACE_Export TimeValue operator * (const TimeValue &tv, double d);

# if defined (ACE_WIN32)
  /// Const time difference between FILETIME and POSIX time.
#  if defined (ACE_LACKS_LONGLONG_T)
  static const ACE_U_LongLong FILETIME_to_timval_skew;
#  else
  static const DWORDLONG FILETIME_to_timval_skew;
#  endif // ACE_LACKS_LONGLONG_T
# endif /* ACE_WIN32 */

private:
  // Put the timevalue into a canonical form.
  void normalize (void);

  // Store the values as a timeval.
#if defined (ACE_HAS_TIME_T_LONG_MISMATCH)
  // Windows' timeval is non-conformant, so swap in a struct that conforms
  // to the proper data types to represent the entire time range that this
  // class's API can accept.
  // Also, since this class can supply a pointer to a timeval that things
  // like select() expect, we need the OS-defined one as well. To make this
  // available, use a real timeval called ext_tv_ and set it up when needed.
  // Since this is most often for relative times that don't approach 32 bits
  // in size, reducing a time_t to fit should be no problem.
  struct {
    time_t tv_sec;
    suseconds_t tv_usec;
  } tv_;
  timeval ext_tv_;
#else
  timeval tv_;
#endif /* ACE_HAS_TIME_T_LONG_MISMATCH */
};

#endif //ICC_TIME_VALUE_H
