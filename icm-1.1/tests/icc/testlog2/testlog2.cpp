
#include "icc/Log.h"
//#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

//#include <ace/Task.h>
#include "icc/Task.h"
#include "icc/ThreadManager.h"


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

class MyTask1 : public Task<MT_SYNCH>
{
public:
  virtual int svc (void)
  {
    //OS::sleep (1);
    for(int i = 0; i < 50000; ++i) {
	  LogMsg << "This Num: " << i << endl;

	  ICC_DEBUG_TEST("IcmProtocol::parseReply: request");

	  int d = 15;

	  //ACE_DEBUG ((LM_DEBUG,"%d %s\n", d, "hello"));

	  ICC_DEBUG ("%d %s\n", d, "hello");

	  Log::instance ()->priority (LM_ERROR);

	  ICC_DEBUG ("%d %s\n", d, "no display");

	  ICC_ERROR ("%s:%c\n", "this message will display", 'A');

	  //ACE_ERROR ((LM_ERROR,"%s:%c\n", "this message will display", 'A'));

	  //std::string str = Log::instance ()->m_ostringstream.str ();
    }

    return 0;
  }
};

int main (int argc, char *argv[])
{
	Log::instance ()->open ();

	  MyTask1 task1,task2,task3;

	  task1.activate();
	  task2.activate();
	  task3.activate();

	  //OS::sleep (3);
	  ThreadManager::instance ()->wait ();

	  return 0;


  // test( "%d %c %d", 123, '<', 456 );
  // test( "%s", "This is a string" );

//  Log::instance ()->open ("log.txt");
	Log::instance ()->open ();

  LogMsg << "This Num: " << 1234 << endl;

  ICC_DEBUG_TEST("IcmProtocol::parseReply: request");

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
