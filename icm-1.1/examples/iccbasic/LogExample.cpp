/*
 * example usage of Log
 *
 * This example demonstrates:
 *
 * 1) How to set log destination(console or file) and log level
 *      
 * 2) How to print log with different level
 *
 * 3) Two format style for user to chose at will: C printf or C++ stream
 * 
 */

#include "icc/Log.h"
#include "os/OS_NS_string.h"

int main() {
  // set log to console, with default level LM_DEBUG 
  //Log::instance()->open(); 

  // set log to file "mylog?.txt", with level LM_ERROR
  // ? auto cycle from 1 to 10 when log file is big enough
  Log::instance()->open("mylog", LM_ERROR);

  // c printf like format message, with level LM_DEBUG
  ICC_DEBUG("debug message: %s", "testdebug"); 

  // c printf like format message, with level LM_ERROR
  ICC_ERROR("error message: %s", "testerror"); 

  // like perror(), print the string message corresponding to current errno
  ICC_ERROR("call error: %s", OS::strerror(OS::map_errno(OS::last_error()))); 

  // c++ stream like format message, with level LM_ERROR
  LogError << "call failed: " << perr << endl; 

  // c++ stream like format message, with level LM_INFO
  LogInfo << "call success" << endl; 

  return 0;
}
