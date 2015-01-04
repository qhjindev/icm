#include "os/OS.h"
#include "icc/Log.h"

class TestIocp
{
public:
  void init ()
  {
    //mFile = OS::open ("test.txt", GENERIC_READ | FILE_FLAG_OVERLAPPED);
    mFile = OS::open ("test.txt", O_BINARY | O_CREAT | O_RDWR | O_TRUNC | FILE_FLAG_OVERLAPPED);
    if (mFile == ACE_INVALID_HANDLE) {
      LogMsg << "failed to open test.txt : " << perr << endl;
      return;
    }

    this->mCompletionPort = ::CreateIoCompletionPort (INVALID_HANDLE_VALUE, 0, 0, 0);
    if (this->mCompletionPort == 0) {
      LogMsg << "CreateIoCompletionPort " << perr << endl;
      return;
    }

    ACE_HANDLE h = ::CreateIoCompletionPort (mFile, mCompletionPort, 0, 0);
    if (h == 0) {
      LogMsg << "CreateIoCompletionPort " << perr << endl;
      return;
    }

    char buf[1024];
    memset (buf, 1, 1024);
    DWORD bytesWritten;
    DWORD bytesToRead = 1024;
    DWORD bytesRead;

    OVERLAPPED overlapped;
    memset (&overlapped, 0, sizeof (OVERLAPPED));
    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;
    overlapped.hEvent = 0;
    //int ret = ::ReadFile (mFile, buf, bytesToRead, &bytesRead, &overlapped);

    int ret = ::WriteFile (mFile, buf, 1024, &bytesWritten, &overlapped);
    if (ret == 0) {
      LogMsg << "WriteFile " << perr << endl;
      //return;
    }

    OVERLAPPED *poverlapped;
    DWORD numTransferred = 0;
    int r = 
    ::GetQueuedCompletionStatus (this->mCompletionPort, 
                                 &numTransferred,
                                 0,
                                 &poverlapped,
                                 INFINITE);
    if (r > 0) {

    }
  }

protected:
  ACE_HANDLE mFile;

  ACE_HANDLE mCompletionPort;

};

int
main (int argc, char* argv[])
{
  TestIocp iocp;
  iocp.init ();

  return 0;
}