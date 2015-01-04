#ifndef ICC_SOCKET_STREAM_H
#define ICC_SOCKET_STREAM_H

#include "Socket.h"
#include "icc/TimeValue.h"

class SocketStream : public Socket
{
public:
  ssize_t send (const void *buf,
                size_t n,
                int flags = 0) const;

  ssize_t recv (void *buf,
                size_t n,
                int flags = 0) const;

  ssize_t recv_n (void *buf,
                 size_t len,
                 const TimeValue *timeout = 0,
                 size_t *bytesTransferred = 0) ;

  int  handleReady (ACE_HANDLE handle,
                    const TimeValue *timeout,
                    int readReady,
                    int writeReady,
                    int exceptionReady);

};

#endif //ICC_SOCKET_STREAM_H
