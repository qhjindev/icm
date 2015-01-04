
#include "icc/SocketConnector.h"
#include "icc/InetAddr.h"
#include "icc/Log.h"
//#include "os/OS_NS_unistd.h"
#include "os/OS_NS_sys_socket.h"
#include "os/os_include/os_fcntl.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_errno.h"

SocketConnector::SocketConnector (void)
{

}

int
SocketConnector::connect(SocketStream &newStream, const InetAddr &remoteAddr)
{
  if (newStream.getHandle() == ACE_INVALID_HANDLE) {
    if(newStream.open(SOCK_STREAM,remoteAddr.getType(),0,0) == -1) {
      LogMsg << "newStream::open err: " << perr << endl;
      return -1;
    }
  }

  int result = OS::connect (newStream.getHandle (),
                                reinterpret_cast<sockaddr *> (remoteAddr.getAddr ()),
                                remoteAddr.getSize ());

  if (result == -1) {
    LogError << "OS::connect errno:" << errno << " msg:" << perr << endl;
    newStream.close ();
  }

  // EISCONN is treated specially since this routine may be used to
  // check if we are already connected.
  if (result != -1 || errno == EISCONN)
    // Start out with non-blocking disabled on the <newStream>.
    newStream.disable (ACE_NONBLOCK);
  else if (!(errno == EWOULDBLOCK || errno == ETIMEDOUT))
    newStream.close ();

  return result;
}
