
#include "icm/Communicator.h"
#include "icm/TransportCacheManager.h"
#include "icm/TcpConnection.h"
#include "icm/IcmStream.h"
#include "icm/IcmProtocol.h"
#include "icm/WaitStrategy.h"
#include "icm/ServerRequest.h"

TcpConnectionHandler::TcpConnectionHandler (Communicator* comm, ObjectAdapter* oa)
: mCommunicator (comm), mTransport (this,comm, oa)
{
}

TcpConnectionHandler::~TcpConnectionHandler (void)
{
}

int
TcpConnectionHandler::open(void*)
{
  if(this->getStream().getRemoteAddr(this->remoteAddr) == -1)
    return -1;

  if (this->getStream().getLocalAddr(this->localAddr) == -1)
    return -1;

  std::string ra = remoteAddr.toString();
  std::string la = localAddr.toString();
  //ICC_DEBUG("Connection opened: handle %d, remote addr %s, local addr %s", this->getHandle(), ra.c_str(),la.c_str());

  TransportListener* listener = this->mCommunicator->transportListener();
  if (listener != 0)
    listener->onTransportCreated (&this->mTransport);

  //this->mCommunicator->transportCache()->cacheTransport(remoteAddr, &this->mTransport);

  return 0;
}

int
TcpConnectionHandler::svc (void)
{
  int result = 0;

  while (result >= 0) {
    result = this->transport()->handleInputI (ACE_INVALID_HANDLE, 0);
    if (result == -1 && errno == ETIME)
      result = 0;
  }

  return result;
}

int
TcpConnectionHandler::handleInput(ACE_HANDLE h)
{
  int retval = this->transport()->handleInputI (h);
  if(retval == -1) {
    this->transport()->closeConnection();
  }

  return retval;
}

int
TcpConnectionHandler::handleClose (ACE_HANDLE h, ReactorMask mask)
{
//  ICC_DEBUG("TcpConnectionHandler::handleClose (%d, %d), local: %s, remote: %s", h, mask, localAddr.toString().c_str(), remoteAddr.toString().c_str());

  TransportListener* listener = this->mCommunicator->transportListener();
  if (listener != 0)
    listener->onTransportDestroyed(&this->mTransport);

  this->mTransport.connClosed();

  return SvcHandler::handleClose (h, mask);
}

IcmTransport*
TcpConnectionHandler::transport (void)
{
  return &(this->mTransport);
}

