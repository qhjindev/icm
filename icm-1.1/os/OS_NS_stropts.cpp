// $Id: OS_NS_stropts.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $

#include "os/OS_NS_stropts.h"

ACE_RCSID(ace, OS_NS_stropts, "$Id: OS_NS_stropts.cpp,v 1.2 2009/03/19 05:29:23 qinghua Exp $")

#include "os/os_include/os_errno.h"
#include "os/OS_NS_unistd.h"
#include "os/OS_NS_string.h"
#include "os/OS_NS_macros.h"
#include "os/OS_Memory.h"
#include "os/OS_QoS.h"



#if defined (ACE_LACKS_CONST_STRBUF_PTR)
typedef struct strbuf *ACE_STRBUF_TYPE;
#else
typedef const struct strbuf *ACE_STRBUF_TYPE;
#endif /* ACE_LACKS_CONST_STRBUF_PTR */

ACE_INLINE
ACE_Str_Buf::ACE_Str_Buf (void *b, int l, int max)
{
  this->maxlen = max;
  this->len = l;
  this->buf = (char *) b;
}

ACE_INLINE
ACE_Str_Buf::ACE_Str_Buf (strbuf &sb)
{
  this->maxlen = sb.maxlen;
  this->len = sb.len;
  this->buf = sb.buf;
}

/*****************************************************************************/

ACE_INLINE int
OS::getmsg (ACE_HANDLE handle,
                struct strbuf *ctl,
                struct strbuf *data,
                int *flags)
{
  OS_TRACE ("OS::getmsg");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::getmsg (handle, ctl, data, flags), int, -1);
#else
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (ctl);
  ACE_UNUSED_ARG (data);
  ACE_UNUSED_ARG (flags);

  // I'm not sure how to implement this correctly.
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_STREAM_PIPES */
}

ACE_INLINE int
OS::getpmsg (ACE_HANDLE handle,
                 struct strbuf *ctl,
                 struct strbuf *data,
                 int *band,
                 int *flags)
{
  OS_TRACE ("OS::getpmsg");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::getpmsg (handle, ctl, data, band, flags), int, -1);
#else
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (ctl);
  ACE_UNUSED_ARG (data);
  ACE_UNUSED_ARG (band);
  ACE_UNUSED_ARG (flags);

  // I'm not sure how to implement this correctly.
  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_STREAM_PIPES */
}

ACE_INLINE int
OS::fattach (int handle, const char *path)
{
  OS_TRACE ("OS::fattach");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::fattach (handle, path), int, -1);
#else
  ACE_UNUSED_ARG (handle);
  ACE_UNUSED_ARG (path);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_STREAM_PIPES */
}

ACE_INLINE int
OS::fdetach (const char *file)
{
  OS_TRACE ("OS::fdetach");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::fdetach (file), int, -1);
#else
  ACE_UNUSED_ARG (file);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_STREAM_PIPES */
}

ACE_INLINE int
OS::ioctl (ACE_HANDLE handle,
               ACE_IOCTL_TYPE_ARG2 cmd,
               void *val)
{
  OS_TRACE ("OS::ioctl");

#if defined (ACE_WIN32)
  ACE_SOCKET sock = (ACE_SOCKET) handle;
  ACE_SOCKCALL_RETURN (::ioctlsocket (sock, cmd, reinterpret_cast<unsigned long *> (val)), int, -1);
#elif defined (ACE_HAS_IOCTL_INT_3_PARAM)
  OSCALL_RETURN (::ioctl (handle, cmd, reinterpret_cast<int> (val)),
                     int, -1);
#else
  OSCALL_RETURN (::ioctl (handle, cmd, val), int, -1);
#endif /* ACE_WIN32 */
}

ACE_INLINE int
OS::isastream (ACE_HANDLE handle)
{
  OS_TRACE ("OS::isastream");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::isastream (handle), int, -1);
#else
  ACE_UNUSED_ARG (handle);

  ACE_NOTSUP_RETURN (-1);
#endif /* ACE_HAS_STREAM_PIPES */
}

ACE_INLINE int
OS::putmsg (ACE_HANDLE handle, const struct strbuf *ctl,
                const struct strbuf *data, int flags)
{
  OS_TRACE ("OS::putmsg");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::putmsg (handle,
                               (ACE_STRBUF_TYPE) ctl,
                               (ACE_STRBUF_TYPE) data,
                               flags), int, -1);
#else
  ACE_UNUSED_ARG (flags);
  ssize_t result;
  if (ctl == 0 && data == 0)
    {
      errno = EINVAL;
      return 0;
    }
  // Handle the two easy cases.
  else if (ctl != 0)
    {
      result =  OS::write (handle, ctl->buf, ctl->len);
      return static_cast<int> (result);
    }
  else if (data != 0)
    {
      result = OS::write (handle, data->buf, data->len);
      return static_cast<int> (result);
    }
  else
    {
      // This is the hard case.
      char *buf;
      ACE_NEW_RETURN (buf, char [ctl->len + data->len], -1);
      OS::memcpy (buf, ctl->buf, ctl->len);
      OS::memcpy (buf + ctl->len, data->buf, data->len);
      result = OS::write (handle, buf, ctl->len + data->len);
      delete [] buf;
      return static_cast<int> (result);
    }
#endif /* ACE_HAS_STREAM_PIPES */
}

ACE_INLINE int
OS::putpmsg (ACE_HANDLE handle,
                 const struct strbuf *ctl,
                 const struct strbuf *data,
                 int band,
                 int flags)
{
  OS_TRACE ("OS::putpmsg");
#if defined (ACE_HAS_STREAM_PIPES)
  OSCALL_RETURN (::putpmsg (handle,
                                (ACE_STRBUF_TYPE) ctl,
                                (ACE_STRBUF_TYPE) data,
                                band, flags), int, -1);
#else
  ACE_UNUSED_ARG (flags);
  ACE_UNUSED_ARG (band);
  return OS::putmsg (handle, ctl, data, flags);
#endif /* ACE_HAS_STREAM_PIPES */
}





int
OS::ioctl (ACE_HANDLE socket,
               unsigned long io_control_code,
               void *in_buffer_p,
               unsigned long in_buffer,
               void *out_buffer_p,
               unsigned long out_buffer,
               unsigned long *bytes_returned,
               ACE_OVERLAPPED *overlapped,
               ACE_OVERLAPPED_COMPLETION_FUNC func)
{
# if defined (ACE_HAS_WINSOCK2) && (ACE_HAS_WINSOCK2 != 0)
  ACE_SOCKCALL_RETURN (::WSAIoctl ((ACE_SOCKET) socket,
                                   io_control_code,
                                   in_buffer_p,
                                   in_buffer,
                                   out_buffer_p,
                                   out_buffer,
                                   bytes_returned,
                                   (WSAOVERLAPPED *) overlapped,
                                   func),
                       int,
                       SOCKET_ERROR);
# else
  ACE_UNUSED_ARG (socket);
  ACE_UNUSED_ARG (io_control_code);
  ACE_UNUSED_ARG (in_buffer_p);
  ACE_UNUSED_ARG (in_buffer);
  ACE_UNUSED_ARG (out_buffer_p);
  ACE_UNUSED_ARG (out_buffer);
  ACE_UNUSED_ARG (bytes_returned);
  ACE_UNUSED_ARG (overlapped);
  ACE_UNUSED_ARG (func);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_WINSOCK2 */
}

#if !(defined (ACE_HAS_WINCE) && (UNDER_CE < 500))
int
OS::ioctl (ACE_HANDLE socket,
               unsigned long io_control_code,
               ACE_QoS &ace_qos,
               unsigned long *bytes_returned,
               void *buffer_p,
               unsigned long buffer,
               ACE_OVERLAPPED *overlapped,
               ACE_OVERLAPPED_COMPLETION_FUNC func)
{
# if defined (ACE_HAS_WINSOCK2) && (ACE_HAS_WINSOCK2 != 0)

  QOS qos;
  unsigned long qos_len = sizeof (QOS);

  if (io_control_code == SIO_SET_QOS)
    {
      qos.SendingFlowspec = *(ace_qos.sending_flowspec ());
      qos.ReceivingFlowspec = *(ace_qos.receiving_flowspec ());
      qos.ProviderSpecific = (WSABUF) ace_qos.provider_specific ();

      qos_len += ace_qos.provider_specific ().iov_len;

      ACE_SOCKCALL_RETURN (::WSAIoctl ((ACE_SOCKET) socket,
                                       io_control_code,
                                       &qos,
                                       qos_len,
                                       buffer_p,
                                       buffer,
                                       bytes_returned,
                                       (WSAOVERLAPPED *) overlapped,
                                       func),
                           int,
                           SOCKET_ERROR);
    }
  else
    {
      unsigned long dwBufferLen = 0;

      // Query for the buffer size.
      int result = ::WSAIoctl ((ACE_SOCKET) socket,
                                io_control_code,
                                0,
                                0,
                                &dwBufferLen,
                                sizeof (dwBufferLen),
                                bytes_returned,
                                0,
                                0);


      if (result == SOCKET_ERROR)
        {
          unsigned long dwErr = ::WSAGetLastError ();

          if (dwErr == WSAEWOULDBLOCK)
            {
              errno = dwErr;
              return -1;
            }
          else
            if (dwErr != WSAENOBUFS)
              {
                errno = dwErr;
                return -1;
              }
          }

    char *qos_buf = 0;
    ACE_NEW_RETURN (qos_buf,
                    char [dwBufferLen],
                    -1);

    QOS *qos = reinterpret_cast<QOS*> (qos_buf);

    result = ::WSAIoctl ((ACE_SOCKET) socket,
                       io_control_code,
                       0,
                       0,
                       qos,
                       dwBufferLen,
                       bytes_returned,
                       0,
                       0);

    if (result == SOCKET_ERROR)
      return result;

    ACE_Flow_Spec sending_flowspec (qos->SendingFlowspec.TokenRate,
                                    qos->SendingFlowspec.TokenBucketSize,
                                    qos->SendingFlowspec.PeakBandwidth,
                                    qos->SendingFlowspec.Latency,
                                    qos->SendingFlowspec.DelayVariation,
#  if defined(ACE_HAS_WINSOCK2_GQOS)
                                    qos->SendingFlowspec.ServiceType,
                                    qos->SendingFlowspec.MaxSduSize,
                                    qos->SendingFlowspec.MinimumPolicedSize,
#  else /* ACE_HAS_WINSOCK2_GQOS */
                                    0,
                                    0,
                                    0,
#  endif /* ACE_HAS_WINSOCK2_GQOS */
                                    0,
                                    0);

    ACE_Flow_Spec receiving_flowspec (qos->ReceivingFlowspec.TokenRate,
                                      qos->ReceivingFlowspec.TokenBucketSize,
                                      qos->ReceivingFlowspec.PeakBandwidth,
                                      qos->ReceivingFlowspec.Latency,
                                      qos->ReceivingFlowspec.DelayVariation,
#  if defined(ACE_HAS_WINSOCK2_GQOS)
                                      qos->ReceivingFlowspec.ServiceType,
                                      qos->ReceivingFlowspec.MaxSduSize,
                                      qos->ReceivingFlowspec.MinimumPolicedSize,
#  else /* ACE_HAS_WINSOCK2_GQOS */
                                      0,
                                      0,
                                      0,
#  endif /* ACE_HAS_WINSOCK2_GQOS */
                                      0,
                                      0);

       ace_qos.sending_flowspec (&sending_flowspec);
       ace_qos.receiving_flowspec (&receiving_flowspec);
       ace_qos.provider_specific (*((struct iovec *) (&qos->ProviderSpecific)));


      return result;
    }

# else
  ACE_UNUSED_ARG (socket);
  ACE_UNUSED_ARG (io_control_code);
  ACE_UNUSED_ARG (ace_qos);
  ACE_UNUSED_ARG (bytes_returned);
  ACE_UNUSED_ARG (buffer_p);
  ACE_UNUSED_ARG (buffer);
  ACE_UNUSED_ARG (overlapped);
  ACE_UNUSED_ARG (func);
  ACE_NOTSUP_RETURN (-1);
# endif /* ACE_HAS_WINSOCK2 */
}
#endif /* !(defined (ACE_HAS_WINCE) && (UNDER_CE < 500)) */


