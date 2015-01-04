#ifndef ICC_DATE_TIME_H
#define ICC_DATE_TIME_H

class TimeValue;

class DateTime
{
public:
  /// Constructor initializes current time/date info.
  DateTime (void);

  /// Constructor initializes with the given TimeValue
  explicit DateTime (const TimeValue& timevalue);

  /// Update to the current time/date.
  void update (void);

  /// Update to the given TimeValue
  void update (const TimeValue& timevalue);

public:
  long day;
  long month;
  long year;
  long hour;
  long minute;
  long second;
  long microsec;
  long wday;
};

#endif //ICC_DATE_TIME_H
