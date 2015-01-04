
#include "icc/Proactor.h"
#include "icc/AsynchIo.h"
#include "icc/AsynchIoImpl.h"
#include "icc/MessageBlock.h"
#include "icc/Log.h"

#if ((defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS)))
  // This only works on Win32 platforms and on Unix platforms supporting
  // POSIX aio calls.

static char *file = "testproactor1.cpp";
static char *dump_file = "simple_output";
static int initial_read_size = BUFSIZ;
static int done = 0;

class Simple_Tester : public Handler
{
  // = TITLE
  //
  //     Simple_Tester
  //
  // = DESCRIPTION
  //
  //     The class will be created by main(). This class reads a block
  // from the file and write that to the dump file.

public:
  Simple_Tester (void);
  // Constructor.

  ~Simple_Tester (void);

  int open (void);
  // Open the operations and initiate read from the file.

protected:
  // = These methods are called by the freamwork.

  virtual void handle_read_file (const AsynchReadFile::Result &result);
  // This is called when asynchronous reads from the socket complete.

  virtual void handle_write_file (const AsynchWriteFile::Result &result);
  // This is called when asynchronous writes from the socket complete.

private:
  int initiate_read_file (void);

  AsynchReadFile rf_;
  // rf (read file): for writing from the file.

  AsynchWriteFile wf_;
  // ws (write File): for writing to the file.

  ACE_HANDLE input_file_;
  // File to read from.

  ACE_HANDLE dump_file_;
  // File for dumping data.

  // u_long file_offset_;
  // Current file offset

  // u_long file_size_;
  // File size
};


Simple_Tester::Simple_Tester (void)
  : input_file_ (ACE_INVALID_HANDLE),
    dump_file_ (ACE_INVALID_HANDLE)
{
}

Simple_Tester::~Simple_Tester (void)
{
}


int
Simple_Tester::open (void)
{
  // Initialize stuff

  // Open input file (in OVERLAPPED mode)
  this->input_file_ = OS::open (file,
                                    GENERIC_READ | FILE_FLAG_OVERLAPPED);
  if (this->input_file_ == ACE_INVALID_HANDLE)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "OS::open"), -1);

/*
  // Open dump file (in OVERLAPPED mode)
  this->dump_file_ = OS::open (dump_file,
                                   O_CREAT | O_RDWR | O_TRUNC | FILE_FLAG_OVERLAPPED,
                                   0664);
*/
  // Open dump file (in OVERLAPPED mode)
  this->dump_file_ = OS::open (dump_file,
                                   O_CREAT | O_RDWR | O_TRUNC | FILE_FLAG_OVERLAPPED);

  if (this->dump_file_ == ACE_INVALID_HANDLE)
    //ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "OS::open"), -1);
    LogMsg << "OS::open: " << perr << endl;

  // Open ACE_Asynch_Read_File
  if (this->rf_.open (*this, this->input_file_) == -1)
    //ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_Asynch_Read_File::open"), -1);
    LogMsg << "ACE_Asynch_Read_File::open: " << perr << endl;

  // Open ACE_Asynch_Write_File
  if (this->wf_.open (*this, this->dump_file_) == -1)
    //ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_Asynch_Write_File::open"), -1);
    LogMsg << "ACE_Asynch_Write_File::open: " << perr << endl;

  ACE_DEBUG ((LM_DEBUG,
              "Simple_Tester::open: Files and Asynch Operations opened sucessfully\n"));


  // Start an asynchronous read file
  if (this->initiate_read_file () == -1)
    return -1;

  return 0;
}


int
Simple_Tester::initiate_read_file (void)
{
  // Create Message_Block
  MessageBlock *mb = 0;
  ACE_NEW_RETURN (mb, MessageBlock (BUFSIZ + 1), -1);

  // Inititiate an asynchronous read from the file
  if (this->rf_.read (*mb,
		      mb->size () - 1) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_Asynch_Read_File::read"), -1);

  ACE_DEBUG ((LM_DEBUG,
              "Simple_Tester:initiate_read_file: Asynch Read File issued sucessfully\n"));

  return 0;
}

void
Simple_Tester::handle_read_file (const AsynchReadFile::Result &result)
{
  ACE_DEBUG ((LM_DEBUG, "handle_read_file called\n"));

  result.message_block ().rdPtr ()[result.bytes_transferred ()] = '\0';

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_read", result.bytes_to_read ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));

  if(result.message_block().length() < ACE_MAXLOGMSGLEN)
    ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "message_block", result.message_block ().rdPtr ()));

  if (result.success ())
    {
      // Read successful: write this to the file.
      if (this->wf_.write (result.message_block (),
			   result.bytes_transferred ()) == -1)
	    {
	      ACE_ERROR ((LM_ERROR, "%p\n", "ACE_Asynch_Write_File::write"));
	      return;
	    }
    }
}

void
Simple_Tester::handle_write_file (const AsynchWriteFile::Result &result)
{
  ACE_DEBUG ((LM_DEBUG, "handle_write_File called\n"));

  // Reset pointers
  result.message_block ().rdPtr (result.message_block ().rdPtr () - result.bytes_transferred ());

  result.message_block ().rdPtr ()[result.bytes_transferred ()] = '\0';

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));

  if(result.message_block().length() < ACE_MAXLOGMSGLEN)
    ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "message_block", result.message_block ().rdPtr ()));

  done = 1;
}

/*
static int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opt (argc, argv, "f:d:");
  int c;

  while ((c = get_opt ()) != EOF)
    switch (c)
      {
      case 'f':
	file = get_opt.optarg;
	break;
      case 'd':
	dump_file = get_opt.optarg;
	break;
      default:
	ACE_ERROR ((LM_ERROR, "%p.\n",
		    "usage :\n"
                    "-d <dumpfile>\n"
		    "-f <file>\n"));
	return -1;
      }

  return 0;
}
*/

int open_a_file ()
{
#ifdef ACE_WIN32
  HANDLE hFile;

  hFile = CreateFile(TEXT("myfile.txt"),     // file to create
                     GENERIC_WRITE,          // open for writing
                     0,                      // do not share
                     NULL,                   // default security
                     CREATE_ALWAYS,          // overwrite existing
                     FILE_ATTRIBUTE_NORMAL | // normal file
                     FILE_FLAG_OVERLAPPED,   // asynchronous I/O
                     NULL);                  // no attr. template

  if (hFile == INVALID_HANDLE_VALUE)
  {
      printf("Could not open file (error %d)\n", GetLastError());
      return 0;
  }
#else
  return -1;
#endif
}

int
main (int argc, char *argv[])
{
  ::SetLastError (38);
  //DWORD err = ::GetLastError();
  LogMsg << "err: " << perr << endl;

  open_a_file ();
  //if (parse_args (argc, argv) == -1)
    //return -1;

  Simple_Tester Simple_Tester;

  if (Simple_Tester.open () == -1)
    return -1;

  int success = 1;

  while (success != -1  && !done)
    {
      // dispatch events
      success = Proactor::instance ()->handle_events ();

      if (success == 0)
        OS::sleep (5);
    }
  return 0;
}

#endif /* ACE_WIN32 && !ACE_HAS_WINCE || ACE_HAS_AIO_CALLS*/
