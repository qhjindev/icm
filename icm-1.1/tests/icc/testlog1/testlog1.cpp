
#include "icc/Log.h"
//#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

#define ICC_DEBUG_TEST(X, ...) \
  do { \
    Log* pLog = Log::instance(); \
    LogPriority priority = pLog->priority (); \
    if (priority > LM_DEBUG) break; \
    pLog->log (X, ##__VA_ARGS__) ; \
    endl (*pLog); \
  } while (0);

/*
void test( char * format, ... )
{
    va_list args;
    int     len;
    char    *buffer;

    // retrieve the variable arguments
    va_start( args, format );
    
    len = _vscprintf( format, args ) // _vscprintf doesn't count
                                + 1; // terminating '\0'
    
    buffer = (char*)malloc( len * sizeof(char) );

    vsprintf( buffer, format, args ); // C4996
    // Note: vsprintf is deprecated; consider using vsprintf_s instead
    puts( buffer );

    free( buffer );
}
*/

int main (int argc, char *argv[])
{
  // test( "%d %c %d", 123, '<', 456 );
  // test( "%s", "This is a string" );

  //Log::instance ()->open ("log.txt");

  LogMsg << "This Num: " << 1234 << endl;

  string vol = "volume1";
  LogDebug << "disk:" << vol << endl;

  ICC_DEBUG_TEST("IcmProtocol::parseReply: request");

  LogDebug << "abcd" << endl;

  int d = 15;

  ACE_DEBUG ((LM_DEBUG,"%d %s\n", d, "hello"));

  ICC_DEBUG ("%d %s\n", d, "hello");

  Log::instance ()->priority (LM_ERROR);

  ICC_DEBUG ("%d %s\n", d, "no display");

  ICC_ERROR ("%s:%c\n", "this message will display", 'A');

  ACE_ERROR_RETURN ((LM_ERROR,"%s:%c\n", "this message will display", 'A'), -1);

  std::string str = Log::instance ()->m_ostringstream.str ();

  return 0;
}
