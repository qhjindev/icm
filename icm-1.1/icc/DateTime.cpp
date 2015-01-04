
#include "icc/DateTime.h"
#include "os/Global_Macros.h"
#include "icc/TimeValue.h"
#include "os/OS_NS_sys_time.h"
#include "os/OS_NS_time.h"


void
DateTime::update (const TimeValue& timevalue)
{
  time_t time = timevalue.sec ();
  struct tm tm_time;
  OS::localtime_r (&time, &tm_time);
  this->day = tm_time.tm_mday;
  this->month = tm_time.tm_mon + 1;    // localtime's months are 0-11
  this->year = tm_time.tm_year + 1900; // localtime reports years since 1900
  this->hour = tm_time.tm_hour;
  this->minute = tm_time.tm_min;
  this->second = tm_time.tm_sec;
  this->microsec = timevalue.usec ();
}

void
DateTime::update (void)
{
  update(OS::gettimeofday ());
}

DateTime::DateTime (void)
{
  this->update ();
}

DateTime::DateTime (const TimeValue& timevalue)
{
  this->update (timevalue);
}
