#include "os/OS_NS_errno.h"
#include "os/OS_NS_Thread.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_stdio.h"
#include "icc/Log.h"
#include "icc/Guard.h"
#include "icc/Thread.h"
#include "icc/DateTime.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

int LogFileSize = 1024 * 1024; // 1M
int MaxLogFiles = 10;

//static initialization
int Log::keyCreated = 0;
int Log::currentFileIndex = 1;
ostream* Log::os(NULL);
ThreadMutex Log::logFileMutex;
string Log::logFileNamePref("");
LogPriority Log::priority_(LM_DEBUG);

static ACE_thread_key_t thelogMsgTssKey = 0;

ACE_thread_key_t *logMsgTssKey(void) {
  return &thelogMsgTssKey;
}

Log::Log() {
}

LogPriority Log::priority(void) const {
  return priority_;
}

void Log::priority(LogPriority lp) {
  priority_ = lp;
}

Log *
Log::instance(void) {
  if (keyCreated == 0) {
    if (OS::thr_keycreate(&thelogMsgTssKey, 0) != 0)
      return 0;

    keyCreated = 1;
  }

  Log *tssLog = 0;
  void *temp = 0;

  if (OS::thr_getspecific(thelogMsgTssKey, &temp) == -1)
    return 0;

  tssLog = static_cast<Log *>(temp);
  if (tssLog == 0) {
    tssLog = new Log;
    if (OS::thr_setspecific(thelogMsgTssKey, reinterpret_cast<void*>(tssLog)) != 0) {
      delete tssLog;
      return 0;
    }
  }

  return tssLog;
}

void Log::open(int rotationBackups, const std::string& logFileName, LogPriority priority, int maxFileSize, int rotationFiles) {
  if (logFileName != "") {
    logFileNamePref = logFileName;
    assert(os == NULL);

    size_t pos = logFileName.rfind('/');
    string path = logFileName.substr(0, pos) + "/";
    char buf[64] = {0};
    time_t now = time(0);
    strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", localtime(&now));
    string bakLogName = path + "log." + buf + ".tar.gz";
    string backupCmd = "if [ -e \"" + logFileName + "1.txt\" ]; then " +
        "tar cvf " + bakLogName + " " + logFileName + "*.txt 2>/dev/null; fi";
    string cleanCmd = "rm " + logFileName + "*.txt 2>/dev/null";
//    cout << "path: " << path << endl;
//    cout << "bakLogName: " << bakLogName << endl;
//    cout << "backupCmd: " << backupCmd << endl;
//    cout << "cleanCmd: " << cleanCmd << endl;
    OS::system(backupCmd.c_str());
    OS::system(cleanCmd.c_str());

    if (rotationBackups > 0) {
       char bufTmp[1024] = {0};
       sprintf(bufTmp, "rm `ls -t %slog.*.tar.gz |tail -n +%d` 2>/dev/null", path.c_str(), rotationBackups + 1);
       OS::system(bufTmp);
    }

    // for thread-safe open
    ACE_GUARD(ThreadMutex, guard, Log::logFileMutex);
    if (os == 0) {
      string realLogFileName = logFileName + string("1.txt");
      os = new ofstream(realLogFileName.c_str());
    }
  }

  LogFileSize = maxFileSize;
  MaxLogFiles = rotationFiles;
  priority_ = priority;
}

void Log::open(const std::string& logFileName, LogPriority priority, int maxFileSize, int rotationFiles) {
  open(0, logFileName, priority, maxFileSize, rotationFiles);
}

Log&
Log::operator <<(int i) {
  m_ostringstream << i;
  return *this;
}

Log&
Log::operator <<(long l) {
  m_ostringstream << l;
  return *this;
}

Log&
Log::operator <<(long long ll) {
  m_ostringstream << ll;
  return *this;
}

Log&
Log::operator <<(unsigned int i) {
  m_ostringstream << i;
  return *this;
}

Log&
Log::operator <<(unsigned long l) {
  m_ostringstream << l;
  return *this;
}

Log&
Log::operator <<(unsigned long long l) {
  m_ostringstream << l;
  return *this;
}

Log&
Log::operator <<(float f) {
  m_ostringstream << f;
  return *this;
}

Log&
Log::operator <<(double d) {
  m_ostringstream << d;
  return *this;
}

Log&
Log::operator <<(char *str) {
  m_ostringstream << str;
  return *this;
}

Log&
Log::operator <<(const char *str) {
  m_ostringstream << str;
  return *this;
}

Log&
Log::operator <<(const std::string& str) {
  m_ostringstream << str;
  return *this;
}

Log&
Log::operator <<(void(*pfn)(Log&)) {
  (*pfn)(*this);
  return *this;
}

void Log::log(const char* formatStr, ...)
{
  va_list argp;

  va_start (argp, formatStr);

  char buf[ACE_MAXLOGMSGLEN + 1];
  OS::vsprintf (buf, formatStr, argp);

  this->m_ostringstream << buf;

  va_end (argp);
}

void Log::log(const std::string& formatStr, ...)
{
  va_list argp;

  va_start (argp, formatStr);

  char buf[ACE_MAXLOGMSGLEN + 1];
  OS::vsprintf (buf, formatStr.c_str(), argp);

  this->m_ostringstream << buf;

  va_end (argp);
}

void Log::log(LogPriority lp, const char* formatStr, ...)
{
  if (lp < priority_)
  return;

  va_list argp;

  va_start (argp, formatStr);

  char buf[ACE_MAXLOGMSGLEN + 1];
  OS::vsprintf (buf, formatStr, argp);

  va_end (argp);

  this->m_ostringstream << buf;

  endl (*this);
}

void Log::setOutStream(std::ostream *os) {
  ACE_GUARD(ThreadMutex, guard, logFileMutex);
  setOutStream_i(os);
}
void Log::setOutStream_i(std::ostream *os) {
  if (Log::os != NULL) {
    //m_ostream->close(); // TODO: need close?
    delete Log::os;
  }

  Log::os = os;
}

std::ostringstream &
Log::getOutStringStream(void) {
  return this->m_ostringstream;
}

void endl(Log& l) {
  endl(l, 0, 0);
}

void endl(Log &l, const char* filename, int line) {
//add thread id
  ACE_thread_t tid = Thread::self();

  //add timestamp
  DateTime dt;
  char buf[128];

  bool addSoureCodeFileInfo = (filename != 0 || line != 0);
  addSoureCodeFileInfo = false;
  if (addSoureCodeFileInfo) {
    sprintf(buf, "%s:%d, %02ld/%02ld %02ld:%02ld:%02ld.%06ld@",
        filename, line, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.microsec);
  } else {
    sprintf(buf, "%02ld/%02ld %02ld:%02ld:%02ld.%06ld@", dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.microsec);
  }

  l.m_ostringstream << std::endl;
  string str = l.m_ostringstream.str();
  if (l.os != 0) {
    ACE_GUARD(ThreadMutex, guard, Log::logFileMutex);
    if (l.os->tellp() > LogFileSize) {

      if (++l.currentFileIndex > MaxLogFiles)
        l.currentFileIndex = 1;

      stringstream ss;
      ss << Log::logFileNamePref << l.currentFileIndex << string(".txt");
      string logFileName(ss.str());
      ostream* os = new ofstream(logFileName.c_str());
      l.setOutStream_i(os);
    }

    *l.os << buf << "(" << tid << ")" << str << flush;
  } else {
    cout << buf << "(" << tid << ")" << str << std::flush;
  }
  l.m_ostringstream.flush();
  l.m_ostringstream.str("");
}

void perr(Log &l) {
  char *msg = OS::strerror(OS::map_errno(OS::last_error()));
  l.m_ostringstream << msg;
}

