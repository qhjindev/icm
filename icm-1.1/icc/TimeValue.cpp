
#include "os/config.h"
#include "icc/TimeValue.h"

#include <limits>

#undef min
#undef max
//#include "config.h"


const TimeValue TimeValue::zero;

const TimeValue TimeValue::maxTime (
  std::numeric_limits<time_t>::max (),
  ACE_ONE_SECOND_IN_USECS - 1);

TimeValue::TimeValue (void)
{
  this->set (0, 0);
}

TimeValue::TimeValue (time_t sec, suseconds_t usec)
{
  this->set (sec, usec);
}

TimeValue::TimeValue(const struct timeval &t)
{
  this->set (t);
}

void
TimeValue::set (time_t sec, suseconds_t usec)
{
  this->tv_.tv_sec = sec;
  this->tv_.tv_usec = usec;

  this->normalize ();
}

void
TimeValue::set (const timeval &tv)
{
  this->tv_.tv_sec = tv.tv_sec;
  this->tv_.tv_usec = tv.tv_usec;

  this->normalize ();
}

void
TimeValue::set (const timespec_t &tv)
{
	this->set (tv.tv_sec, tv.tv_nsec / 1000);
}

#if defined (ACE_WIN32)

# if defined (ACE_LACKS_LONGLONG_T)
const ACE_U_LongLong ACE_Time_Value::FILETIME_to_timval_skew =
ACE_U_LongLong (0xd53e8000, 0x19db1de);
# else
const DWORDLONG TimeValue::FILETIME_to_timval_skew =
ACE_INT64_LITERAL (0x19db1ded53e8000);
# endif

TimeValue::TimeValue (const FILETIME &fileTime)
{
  // // OS_TRACE ("ACE_Time_Value::ACE_Time_Value");
  this->set (fileTime);
}

void TimeValue::set (const FILETIME &fileTime)
{
  ULARGE_INTEGER _100ns;
  _100ns.LowPart = fileTime.dwLowDateTime;
  _100ns.HighPart = fileTime.dwHighDateTime;

  _100ns.QuadPart -= TimeValue::FILETIME_to_timval_skew;

  this->tv_.tv_sec = (long) (_100ns.QuadPart / (10000 * 1000));
  this->tv_.tv_usec = (suseconds_t)((_100ns.QuadPart % (10000 * 1000)) / 10);

  this->normalize ();
}
#endif /* ACE_WIN32 */

void
TimeValue::normalize (void)
{
  if (this->tv_.tv_usec >= ACE_ONE_SECOND_IN_USECS) {
    do {
      ++this->tv_.tv_sec;
      this->tv_.tv_usec -= ACE_ONE_SECOND_IN_USECS;
    } while (this->tv_.tv_usec >= ACE_ONE_SECOND_IN_USECS);
  } else if (this->tv_.tv_usec <= -ACE_ONE_SECOND_IN_USECS) {
    do {
      --this->tv_.tv_sec;
      this->tv_.tv_usec += ACE_ONE_SECOND_IN_USECS;
    } while (this->tv_.tv_usec <= -ACE_ONE_SECOND_IN_USECS);
  }

  if (this->tv_.tv_sec >= 1 && this->tv_.tv_usec < 0) {
    --this->tv_.tv_sec;
    this->tv_.tv_usec += ACE_ONE_SECOND_IN_USECS;
  } else if (this->tv_.tv_sec < 0 && this->tv_.tv_usec > 0) {
    ++this->tv_.tv_sec;
    this->tv_.tv_usec -= ACE_ONE_SECOND_IN_USECS;
  }
}

TimeValue::operator timeval () const
{
  // OS_TRACE ("ACE_Time_Value::operator timeval");
#if defined (ACE_HAS_TIME_T_LONG_MISMATCH)
  // Recall that on some Windows we substitute another type for timeval in tv_
  TimeValue *me = const_cast<TimeValue*> (this);
  me->ext_tv_.tv_sec =  (long)(this->tv_.tv_sec);
  me->ext_tv_.tv_usec = (long)(this->tv_.tv_usec);
  return this->ext_tv_;
#else
  return this->tv_;
#endif /* ACE_HAS_TIME_T_LONG_MISMATCH */
}

TimeValue::operator const timeval * () const
{
#if defined (ACE_HAS_TIME_T_LONG_MISMATCH)
  // Recall that on some Windows we substitute another type for timeval in tv_
  TimeValue *me = const_cast<TimeValue*> (this);
  me->ext_tv_.tv_sec =  (long)(this->tv_.tv_sec);
  me->ext_tv_.tv_usec = (this->tv_.tv_usec);
  return (const timeval *) &this->ext_tv_;
#else
  return (const timeval *) &this->tv_;
#endif /* ACE_HAS_TIME_T_LONG_MISMATCH */
}

TimeValue::operator timespec_t () const
{
  timespec_t tv;
  tv.tv_sec = this->sec ();
  // Convert microseconds into nanoseconds.
  tv.tv_nsec = this->tv_.tv_usec * 1000;
  return tv;
}

unsigned long
TimeValue::msec (void) const
{
  time_t secs = this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000;
  return (long)secs;
}

void
TimeValue::msec (long milliseconds)
{
  // Convert millisecond units to seconds;
  long secs = milliseconds / 1000;
  this->tv_.tv_sec = secs;
  // Convert remainder to microseconds;
  this->tv_.tv_usec = (milliseconds - (secs * 1000)) * 1000;
}

// Returns number of seconds.

time_t
TimeValue::sec (void) const
{
  // OS_TRACE ("ACE_Time_Value::sec");
  return this->tv_.tv_sec;
}

// Sets the number of seconds.

void
TimeValue::sec (time_t sec)
{
  // OS_TRACE ("ACE_Time_Value::sec");
  this->tv_.tv_sec = sec;
}

// Returns number of micro-seconds.

suseconds_t
TimeValue::usec (void) const
{
  // OS_TRACE ("ACE_Time_Value::usec");
  return this->tv_.tv_usec;
}

// Sets the number of micro-seconds.

void
TimeValue::usec (suseconds_t usec)
{
  // OS_TRACE ("ACE_Time_Value::usec");
  this->tv_.tv_usec = usec;
}

// True if tv1 > tv2.

bool
operator > (const TimeValue &tv1,
            const TimeValue &tv2)
{
  if (tv1.sec () > tv2.sec ())
    return 1;
  else if (tv1.sec () == tv2.sec ()
           && tv1.usec () > tv2.usec ())
    return 1;
  else
    return 0;
}

// True if tv1 >= tv2.

bool
operator >= (const TimeValue &tv1,
             const TimeValue &tv2)
{
  if (tv1.sec () > tv2.sec ())
    return 1;
  else if (tv1.sec () == tv2.sec ()
           && tv1.usec () >= tv2.usec ())
    return 1;
  else
    return 0;
}

// True if tv1 < tv2.

bool
operator < (const TimeValue &tv1,
            const TimeValue &tv2)
{
  return tv2 > tv1;
}

// True if tv1 >= tv2.

bool
operator <= (const TimeValue &tv1,
             const TimeValue &tv2)
{
  return tv2 >= tv1;
}

// True if tv1 == tv2.

bool
operator == (const TimeValue &tv1,
             const TimeValue &tv2)
{
  return tv1.sec () == tv2.sec ()
    && tv1.usec () == tv2.usec ();
}

// True if tv1 != tv2.

bool
operator != (const TimeValue &tv1,
             const TimeValue &tv2)
{
  return !(tv1 == tv2);
}

// Add TV to this.

TimeValue &
TimeValue::operator+= (const TimeValue &tv)
{
  this->sec (this->sec () + tv.sec ());
  this->usec (this->usec () + tv.usec ());
  this->normalize ();
  return *this;
}

TimeValue &
TimeValue::operator+= (time_t tv)
{
  this->sec (this->sec () + tv);
  return *this;
}

TimeValue &
TimeValue::operator= (const TimeValue &tv)
{
  this->sec (tv.sec ());
  this->usec (tv.usec ());
  return *this;
}

TimeValue &
TimeValue::operator= (time_t tv)
{
  this->sec (tv);
  this->usec (0);
  return *this;
}

// Subtract TV to this.

TimeValue &
TimeValue::operator-= (const TimeValue &tv)
{
  this->sec (this->sec () - tv.sec ());
  this->usec (this->usec () - tv.usec ());
  this->normalize ();
  return *this;
}

TimeValue &
TimeValue::operator-= (time_t tv)
{
  this->sec (this->sec () - tv);
  return *this;
}

// Adds two TimeValue objects together, returns the sum.

TimeValue
operator + (const TimeValue &tv1,
            const TimeValue &tv2)
{
  TimeValue sum (tv1);
  sum += tv2;

  return sum;
}

// Subtracts two TimeValue objects, returns the difference.

TimeValue
operator - (const TimeValue &tv1,
            const TimeValue &tv2)
{
  TimeValue delta (tv1);
  delta -= tv2;

  return delta;
}
