#include <iostream>
#include "icc/SvcHandler.h"
#include "icm/TcpTransport.h"
#include "icm/TcpConnection.h"
#include "icm/TransportMuxStrategy.h"
#include "icm/WaitStrategy.h"
#include "icm/Communicator.h"
#include "icm/TransportCacheManager.h"
#include "icm/ObjectAdapter.h"
#include "icm/Reference.h"
#include "icc/MessageBlock.h"

TcpTransport::TcpTransport(TcpConnectionHandler *handler, Communicator* comm, ObjectAdapter* oa)
: IcmTransport (comm), mConnectionHandler (handler)
{
  this->mObjectAdapter = oa;
}

TcpTransport::~TcpTransport (void)
{
}

ACE_HANDLE
TcpTransport::handle (void)
{
  return this->mConnectionHandler->getHandle ();
}

TcpConnectionHandler* &
TcpTransport::handler (void)
{
  return this->mConnectionHandler;
}

void
TcpTransport::closeConnection (void)
{
  if (this->mCommunicator->cacheTransport()) {
    //InetAddr addr;
    //this->mConnectionHandler->getStream().getRemoteAddr(addr);
    //std::cout << addr.toString() << std::endl;

    InetAddr addr = this->mConnectionHandler->getRemoteAddr();
//    ICC_DEBUG("unbinding map from remote addr %s to transport %d", addr.toString().c_str(), this);
    this->mCommunicator->transportCache()->closeTransport(addr);
  }

  //this->mConnectionHandler->handleClose (ACE_INVALID_HANDLE, EventHandler::ALL_EVENTS_MASK);
  mConnectionHandler->getStream().close();
  mConnectionHandler->getReactor()->removeHandler(mConnectionHandler, EventHandler::ALL_EVENTS_MASK||EventHandler::DONT_CALL);
}

ssize_t
TcpTransport::send (Reference* ref, 
                    int twoWay,
                    const MessageBlock* mb,
                    const TimeValue* s )
{
  return this->send (mb, s);
}

ssize_t
TcpTransport::send (const MessageBlock* mb, const TimeValue* maxWaitTime)
{
  return this->mConnectionHandler->getStream ().send (mb->rdPtr (), mb->length ());
}

ssize_t 
TcpTransport::send (const unsigned char* buf, size_t len, const TimeValue* maxWaitTime)
{
  return this->mConnectionHandler->getStream ().send (buf, len);
}

ssize_t
TcpTransport::recv(char *buf, size_t len, const TimeValue *maxWaitTime)
{
  return this->mConnectionHandler->getStream ().recv_n (buf, len, maxWaitTime, 0);
}

int
TcpTransport::sendRequest (Reference* ref, 
                           Communicator* comm, 
                           OutputStream& stream, 
                           int twoWay, 
                           TimeValue* maxWaitTime)
{
//  if (this->mWs->sendingRequest (comm, twoWay) == -1)
//    return -1;

  if (IcmProtocol::sendMessage (this, stream, comm, maxWaitTime, ref, twoWay) == -1)
    return -1;

  return 0;
}

int 
TcpTransport::registerHandler (void)
{
  Reactor* r = this->mCommunicator->reactor ();
  if (r == this->mConnectionHandler->getReactor ())
    return 0;

  return r->registerHandler (this->mConnectionHandler, EventHandler::READ_MASK);
}
