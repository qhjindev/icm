
#include "icc/Socket.h"
#include "os/OS_NS_sys_socket.h"
//#include "os/OS_NS_stropts.h"
#include "os/OS_NS_fcntl.h"
#include "os/Flag_Manip.h"

Socket::Socket (void)
  : m_handle (ACE_INVALID_HANDLE)
{

}

Socket::~Socket (void)
{

}

int Socket::open (int type,
                  int protocolFamily,
                  int protocol,
                  int reuseAddr)
{
  int one = 1;

  this->setHandle (OS::socket (protocolFamily, type, protocol));

  if (this->getHandle () == ACE_INVALID_HANDLE)
    return -1;
  else if (protocolFamily != PF_UNIX
           && reuseAddr
           && this->setOption (SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) == -1)
  {
    this->close ();
    return -1;
  }

  return 0;
}

int Socket::close (void)
{
  int result = 0;

  if (m_handle != ACE_INVALID_HANDLE) {
    result = OS::closesocket (m_handle);
    m_handle = ACE_INVALID_HANDLE;
  }
  return result;
}

int Socket::enable (int value) const
{
#if defined (ACE_WIN32)
  switch (value)
    {
    case ACE_NONBLOCK:
      {
        // nonblocking argument (1)
        // blocking:            (0)
        int nonblock = 1;
        return OS::ioctl (this->m_handle,
                              FIONBIO,
                              &nonblock);
      }
    default:
      ACE_NOTSUP_RETURN (-1);
    }
#else  /* ! ACE_WIN32 && ! VXWORKS */
  switch (value)
    {

#if defined (F_SETFD)
    case ACE_CLOEXEC:
      // Enables the close-on-exec flag.
      if (OS::fcntl (this->m_handle,
                         F_SETFD,
                         1) == -1)
        return -1;
      break;
#endif /* F_SETFD */
    case ACE_NONBLOCK:
      if (ACE::set_flags (this->m_handle, ACE_NONBLOCK) == ACE_INVALID_HANDLE)
        return -1;
      break;
    default:
      return -1;
    }
  return 0;
#endif /* ! ACE_WIN32 */
}

int Socket::getLocalAddr (InetAddr &sa)
{
  int len = sa.getSize ();
  sockaddr *addr = reinterpret_cast<sockaddr *> (sa.getAddr ());

  if (OS::getsockname (this->getHandle (), addr, &len) == -1)
    return -1;

  sa.setType (addr->sa_family);
  sa.setSize (len);
  return 0;
}

int Socket::getRemoteAddr (InetAddr &sa) const
{
  int len = sa.getSize ();
  sockaddr *addr = reinterpret_cast<sockaddr *> (sa.getAddr ());

  if (OS::getpeername (this->getHandle (), addr, &len) == -1)
    return -1;

  sa.setSize (len);
  sa.setType (addr->sa_family);
  return 0;
}

int Socket::control (int cmd, void *arg) const
{
  return OS::ioctl (this->m_handle, cmd, arg);
}

ACE_HANDLE Socket::getHandle (void) const
{
  return this->m_handle;
}

void Socket::setHandle (ACE_HANDLE h)
{
  this->m_handle = h;
}

int Socket::setOption(int level, int option, void *optval, int optlen) const
{
  return OS::setsockopt (this->getHandle (), level, option, (char *)optval, optlen);
}

int Socket::getOption(int level, int option, void *optval, int *optlen) const
{
  return OS::getsockopt (this->getHandle (), level, option, (char *)optval, optlen);
}


int
Socket::disable (int value) const
{
  ACE_TRACE ("ACE_IPC_SAP::disable");

#if defined (ACE_WIN32)
  switch (value)
    {
    case ACE_NONBLOCK:
      // nonblocking argument (1)
      // blocking:            (0)
      {
        int nonblock = 0;
        return OS::ioctl (this->m_handle,
                              FIONBIO,
                              &nonblock);
      }
    default:
      ACE_NOTSUP_RETURN (-1);
    }
#else  /* ! ACE_WIN32 */
  switch (value)
    {
#if defined (SIGURG)
    case SIGURG:
    case ACE_SIGURG:
#if defined (F_SETOWN)
      return OS::fcntl (this->m_handle,
                            F_SETOWN,
                            0);
#else
      ACE_NOTSUP_RETURN (-1);
#endif /* F_SETOWN */
#endif /* SIGURG */
#if defined (SIGIO)
    case SIGIO:
    case ACE_SIGIO:
#if defined (F_SETOWN) && defined (FASYNC)
      if (OS::fcntl (this->m_handle,
                         F_SETOWN,
                         0) == -1
          || ACE::clr_flags (this->m_handle,
                                        FASYNC) == -1)
        return -1;
      break;
#else
      ACE_NOTSUP_RETURN (-1);
#endif /* F_SETOWN && FASYNC */
#endif /* SIGIO <== */
#if defined (F_SETFD)
    case ACE_CLOEXEC:
      // Disables the close-on-exec flag.
      if (OS::fcntl (this->m_handle,
                         F_SETFD,
                         0) == -1)
        return -1;
      break;
#endif /* F_SETFD */
    case ACE_NONBLOCK:
      if (ACE::clr_flags (this->m_handle,
                                     ACE_NONBLOCK) == -1)
        return -1;
      break;
    default:
      return -1;
    }
  return 0;
#endif /* ! ACE_WIN32 */
  /* NOTREACHED */
}
