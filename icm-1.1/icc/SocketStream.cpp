
#include "icc/SocketStream.h"
#include "os/OS_NS_sys_socket.h"
#include "os/OS_NS_unistd.h"
#include "os/OS_NS_sys_select.h"
#include "os/OS_NS_errno.h"
#include "os/OS_NS_fcntl.h"
#include "HandleSet.h"
#include "Log.h"

ssize_t
SocketStream::send (const void *buf,
                    size_t n,
                    int flags) const
{
  return OS::send (this->getHandle (), (const char *)buf, n, flags);
}

ssize_t
SocketStream::recv (void *buf,
                    size_t n,
                    int flags) const
{
  return OS::recv (this->getHandle (), (char *)buf, n, flags);
}

ssize_t
SocketStream::recv_n (void *buf,
                      size_t len,
                      const TimeValue *timeout ,
                      size_t *bytesTransferred )
{
  size_t temp;
  size_t &bt = bytesTransferred == 0 ? temp : *bytesTransferred;
  ssize_t n;
  ssize_t result = 0;
  int error = 0;

  enable(ACE_NONBLOCK);

  for (bt = 0;
      bt < len;
      bt += n)
    {
      // Try to transfer as much of the remaining data as possible.
      // Since the socket is in non-blocking mode, this call will not
      // block.
      n = OS::recv (m_handle,
                        static_cast <char *> (buf) + bt,
                        len - bt);

//      ICC_ERROR("recv_n, n = %d, errno = %d, toRecv = %d", n, errno, len - bt);
      // Check for errors.
      if (n == 0 ||
          n == -1)
        {
          // Check for possible blocking.
          if (n == -1 && (errno == EWOULDBLOCK || errno == 0))
            {
              // Wait upto <timeout> for the blocking to subside.
                int rtn = handleReady(m_handle, timeout, 1, 0, 0);

              // Did select() succeed?
              if (rtn != -1)
                {
                  // Blocking subsided in <timeout> period.  Continue
                  // data transfer.
                  n = 0;
                  continue;
                }
            }

          // Wait in select() timed out or other data transfer or
          // select() failures.
          error = 1;
          result = n;
//          ICC_ERROR("recv_n error, result = %d, errno = %d", result, errno);
          break;
        }
    }

  if (error)
    return result;
  else
    return static_cast<ssize_t> (bt);
}

int
SocketStream::handleReady (ACE_HANDLE handle,
                            const TimeValue *timeout,
                            int readReady,
                            int writeReady,
                            int exceptionReady)
{
  HandleSet handleSet;
  handleSet.setBit(handle);

  // Wait for data or for the timeout to elapse.
  int selectWidth;
#  if defined (ACE_WIN32)
  // This arg is ignored on Windows and causes pointer truncation
  // warnings on 64-bit compiles.
  selectWidth = 0;
#  else
  selectWidth = int (handle) + 1;
#  endif /* ACE_WIN64 */
  int result = OS::select (selectWidth,
                               readReady ? handleSet.fdset () : 0, // read_fds.
                               writeReady ? handleSet.fdset () : 0, // write_fds.
                               exceptionReady ? handleSet.fdset () : 0, // exception_fds.
                               timeout);

  switch (result)    {
    case 0:  // Timer expired.
      errno = ETIME;
      /* FALLTHRU */
    case -1: // we got here directly - select() returned -1.
      return -1;
    case 1: // Handle has data.
      /* FALLTHRU */
    default: // default is case result > 0; return a
      // ACE_ASSERT (result == 1);
      return result;
  }
}
