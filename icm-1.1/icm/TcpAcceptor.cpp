
#include "icc/InetAddr.h"
#include "icm/TcpAcceptor.h"
#include "icm/TcpAcceptorImpl.h"
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"

TcpAcceptor::TcpAcceptor (void)
{
  this->mTag = "TCP";
  this->listenPort = 0;
}

TcpAcceptor::~TcpAcceptor (void)
{
}

int
TcpAcceptor::openDefault (Communicator* comm)
{
  this->mCommunicator = comm;

  InetAddr addr (1234,"localhost");

  //To be do ?
  this->mBaseAcceptor.open (addr, 0);

  return 0;
}

int
TcpAcceptor::open (Communicator* comm, ObjectAdapter* oa, unsigned short port)
{
  this->mCommunicator = comm;
  this->mBaseAcceptor.communicator (comm);
  this->mBaseAcceptor.objectAdapter (oa);
  this->listenPort = port;

  InetAddr addr (port);

  int ret = this->mBaseAcceptor.open (addr, comm->reactor ());
  if(ret == 0 && port == 0)
    this->listenPort = getListenPort();

  return ret;
}

int
TcpAcceptor::close (void)
{
  return this->mBaseAcceptor.close ();
}

const char* 
TcpAcceptor::tag (void)
{
  return this->mTag.c_str ();
}

unsigned short TcpAcceptor::getListenPort() {
  if(this->listenPort == 0) {
    InetAddr sa;
    if(mBaseAcceptor.getLocalAddr(sa) == 0)
      this->listenPort = sa.getPortNumber();
    else
      LogError << "err get port: " << perr << endl;
  }
  return this->listenPort;
}
