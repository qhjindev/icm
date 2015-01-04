
#include "icm/Communicator.h"
#include "icm/TcpConnector.h"
#include "icm/Endpoint.h"
#include "icm/Reference.h"
#include "icm/TransportCacheManager.h"

TcpConnector::TcpConnector (void) :
  IcmConnector ("TCP")
{
}

TcpConnector::~TcpConnector (void)
{
}

int
TcpConnector::open (Communicator* comm)
{
  this->mCommunicator = comm;

  this->mBaseConnector.communicator (comm);

  return 0;
}

int
TcpConnector::close ()
{
  return 0;
}

int
TcpConnector::connect(InetAddr& addr, 
                      IcmTransport*& transport, 
                      TimeValue* maxWaitTime)
{
  int result = 0;
  TcpConnectionHandler* svcHandler = 0;
  IcmTransport* baseTransport = 0;
  //TransportCacheManager* transportCacheManager = 0;

  bool cacheTrans = this->mCommunicator->cacheTransport();
  if (cacheTrans && 
    this->mCommunicator->transportCache ()->findTransport(addr, baseTransport) == 0) {
//      ICC_DEBUG("TcpConnector::connect - got an existing transport with addr %s",
//                addr.toString().c_str());
  } else {

    //ICC_INFO("TcpConnector::connect - making a new connection");

    result = this->mBaseConnector.connect (svcHandler, addr);
    if (result == -1) {
      LogMsg << "TcpConnector::connect to " << addr.getHostAddr() << ":" << addr.getPortNumber()
        << " failed: " << perr << endl;

      return -1;
    }

    baseTransport = svcHandler->transport ();

    if (cacheTrans) {
      this->mCommunicator->transportCache ()->cacheTransport (addr, baseTransport);
    }
  }

  transport = baseTransport;

  return 0;
}
