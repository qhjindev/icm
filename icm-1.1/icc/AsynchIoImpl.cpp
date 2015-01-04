// Asynch_IO_Impl.cpp,v 4.5 2001/05/17 12:08:26 schmidt Exp
#include "os/OS.h"
#include "icc/AsynchIoImpl.h"


#if (defined (ACE_WIN32) && !defined (ACE_HAS_WINCE)) || (defined (ACE_HAS_AIO_CALLS))
// This only works on Win32 platforms and on Unix platforms supporting
// aio calls. 

AsynchResultImpl::AsynchResultImpl (void)
{
}

AsynchOperationImpl::AsynchOperationImpl (void)
{
}

AsynchReadStreamImpl::AsynchReadStreamImpl (void)
  : AsynchOperationImpl ()
{
}

AsynchReadStreamResultImpl::AsynchReadStreamResultImpl (void)
  : AsynchResultImpl ()
{
}

AsynchWriteStreamImpl::AsynchWriteStreamImpl (void)
  : AsynchOperationImpl ()
{
}


AsynchWriteStreamResultImpl::AsynchWriteStreamResultImpl (void)
  : AsynchResultImpl ()
{
}

AsynchReadFileImpl::AsynchReadFileImpl (void)
  : AsynchOperationImpl (),
    AsynchReadStreamImpl ()
{
}


AsynchReadFileResultImpl::AsynchReadFileResultImpl (void)
  : AsynchResultImpl (),
    AsynchReadStreamResultImpl ()
{
}

AsynchWriteFileImpl::AsynchWriteFileImpl (void)
  : AsynchOperationImpl (),
    AsynchWriteStreamImpl ()
{
}

AsynchWriteFileResultImpl::AsynchWriteFileResultImpl (void)
  : AsynchResultImpl (),
    AsynchWriteStreamResultImpl ()
{
}

AsynchResultImpl::~AsynchResultImpl (void)
{
}

AsynchOperationImpl::~AsynchOperationImpl (void)
{
}

AsynchReadStreamImpl::~AsynchReadStreamImpl (void)
{
}

AsynchReadStreamResultImpl::~AsynchReadStreamResultImpl (void)
{
}

AsynchWriteStreamImpl::~AsynchWriteStreamImpl (void)
{
}

AsynchWriteStreamResultImpl::~AsynchWriteStreamResultImpl (void)
{
}

AsynchReadFileImpl::~AsynchReadFileImpl (void)
{
}

AsynchWriteFileImpl::~AsynchWriteFileImpl (void)
{
}

AsynchReadFileResultImpl::~AsynchReadFileResultImpl (void)
{
}

AsynchWriteFileResultImpl::~AsynchWriteFileResultImpl (void)
{
}



#endif /* ACE_WIN32 || ACE_HAS_WINCE */
