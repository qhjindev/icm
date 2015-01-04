#ifndef ICM_TCP_ACCEPTOR_H
#define ICM_TCP_ACCEPTOR_H

#include <string>

#include "icm/TcpConnection.h"
#include "icm/TcpAcceptorImpl.h"

class IcmAcceptor;
//class TcpAcceptorImpl;
class Communicator;
class ObjectAdapter;
//class TcpServerConnectionHandler;

class TcpAcceptor : public IcmAcceptor
{
public:

  TcpAcceptor ();

  virtual ~TcpAcceptor (void);

  virtual int open (Communicator * comm, ObjectAdapter* oa, unsigned short port);

  virtual int openDefault (Communicator* comm);

  virtual int close (void);

  virtual const char* tag (void);

  virtual unsigned short getListenPort();

protected:

  Communicator* mCommunicator;

  std::string mTag;

  TcpAcceptorImpl<TcpConnectionHandler> mBaseAcceptor;

  unsigned short listenPort;
};

#endif //ICM_TCP_ACCEPTOR_H
