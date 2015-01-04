#ifndef ICC_LOG_H
#define ICC_LOG_H

#include "icc/RecursiveThreadMutex.h"
#include "icc/ThreadMutex.h"

#include <string>
#include <sstream>
#include <ostream>
#include <fstream>

enum LogPriority
{
  LM_DEBUG = 01,

  LM_INFO = 02,

  LM_WARNING = 03,

  LM_ERROR = 04
};

class Log
{
	friend void endl(Log &l, const char* filename, int line);
public:
  static Log *instance (void);

  void open (const std::string& logFileName = "", LogPriority priority = LM_DEBUG, int maxFileSize = 1024 * 1024, int rotationFiles = 10);
  void open (int rotationBackups, const std::string& logFileName = "", LogPriority priority = LM_DEBUG, int maxFileSize = 1024 * 1024, int rotationFiles = 10);

  LogPriority priority (void) const;

  void priority (LogPriority lp);

  void log (const char* formatStr, ...);

  void log (const std::string& formatStr, ...);

  void log (LogPriority lp, const char* formatStr, ...);

  Log& operator << (int i);

  Log& operator << (long l);

  Log& operator << (long long ll);

  Log& operator << (unsigned int i);

  Log& operator << (unsigned long l);

  Log& operator << (unsigned long long l);

  Log& operator << (float f);

  Log& operator << (double d);

  Log& operator << (char *str);

  Log& operator << (const char *str);

  Log& operator << (const std::string& str);

  Log& operator << (void (*pfn) (Log &));

  std::ostringstream &getOutStringStream (void);
  static void setOutStream (std::ostream *os);

  std::ostringstream m_ostringstream;

//  RecursiveThreadMutex &getMutex ();

  static std::string logFileNamePref;
  static std::ostream *os;

private:
  Log ();
  static void setOutStream_i (std::ostream *os);

//  static Log *m_log;
  static int currentFileIndex;
//  RecursiveThreadMutex m_mutex;
  static ThreadMutex logFileMutex;

  static LogPriority priority_;
  static int keyCreated;
//  static int linesInCurrentLogFile;

};

void endl(Log&);
void endl (Log &l, const char* filename, int line);
void perr (Log &l);

#define LogMsg *(Log::instance ())
#define LogDebug  \
  if (Log::instance()->priority() <= LM_DEBUG) \
    *(Log::instance ()) \

#define LogInfo  \
  if (Log::instance()->priority() <= LM_INFO) \
    *(Log::instance ()) \

#define LogWarn  \
  if (Log::instance()->priority() <= LM_WARNING) \
    *(Log::instance ()) \

#define LogError  \
  if (Log::instance()->priority() <= LM_ERROR) \
    *(Log::instance ()) \

/*
void ACE_DEBUG(char* format, ...)
{
  do { 
    Log* pLog = Log::instance();
    pLog->log (LM_DEBUG, X); 
    endl (*pLog);
  } while (0);
}
*/

#define ACE_DEBUG(X) \
  do { \
    Log* pLog = Log::instance(); \
    pLog->log X ; \
  } while (0);

#define ACE_ERROR(X) \
  do { \
    Log* pLog = Log::instance(); \
    pLog->log X ; \
  } while (0);

#define ACE_ERROR_RETURN(X, Y) \
  do { \
    Log* pLog = Log::instance(); \
    pLog->log X ; \
    return Y; \
  } while (0);

#define ICC_DEBUG(X, ...) \
  do { \
    Log* pLog = Log::instance(); \
    LogPriority priority = pLog->priority (); \
    if (priority > LM_DEBUG) break; \
    pLog->log (X, ##__VA_ARGS__) ; \
    endl (*pLog, __FILE__, __LINE__); \
  } while (0);

#define ICC_INFO(X, ...) \
  do { \
    Log* pLog = Log::instance(); \
    LogPriority priority = pLog->priority (); \
    if (priority > LM_INFO) break; \
    pLog->log (X, ##__VA_ARGS__) ; \
    endl (*pLog, __FILE__, __LINE__); \
  } while (0);

#define ICC_WARN(X, ...) \
  do { \
    Log* pLog = Log::instance(); \
    LogPriority priority = pLog->priority (); \
    if (priority > LM_WARNING) break; \
    pLog->log (X, ##__VA_ARGS__) ; \
    endl (*pLog, __FILE__, __LINE__); \
  } while (0);

#define ICC_ERROR(X, ...) \
  do { \
    Log* pLog = Log::instance(); \
    LogPriority priority = pLog->priority (); \
    if (priority > LM_ERROR) break; \
    pLog->log (X, ##__VA_ARGS__) ; \
    endl (*pLog, __FILE__, __LINE__); \
  } while (0);


#define ICC_ERROR_RETURN(FORMAT, RETURN, ... ) \
  do { \
    Log* pLog = Log::instance(); \
    LogPriority priority = pLog->priority (); \
    if (priority > LM_ERROR) return RETURN; \
    pLog->log FORMAT; \
    endl (*pLog, __FILE__, __LINE__); \
    return RETURN; \
  } while (0);

#endif //ICC_LOG_H
