#ifndef ICM_TCP_TRANSPORT_H
#define ICM_TCP_TRANSPORT_H

#include "icm/Pluggable.h"
#include "icm/IcmProtocol.h"

class SvcHandler;
class TcpConnectionHandler;
class Communicator;
class MessageBlock;
class ObjectAdapter;
//class IcmMessageState;

class TcpTransport : public IcmTransport
{
public:
  TcpTransport (TcpConnectionHandler* handler, Communicator* comm, ObjectAdapter* oa = 0);

  ~TcpTransport (void);

  TcpConnectionHandler* &handler (void);

  virtual ACE_HANDLE handle (void);

  virtual void closeConnection (void);

  virtual ssize_t send (Reference* ref, 
                        int twoWay,
                        const MessageBlock* mb,
                        const TimeValue* s = 0) ;

  virtual ssize_t send (const MessageBlock* mb, const TimeValue* s = 0);

  virtual ssize_t send (const unsigned char* buf, size_t len, const TimeValue* s = 0);

  virtual ssize_t recv (char* buf, size_t len, const TimeValue* s = 0);

  virtual int sendRequest (Reference* ref, 
                           Communicator* comm, 
                           OutputStream& stream, 
                           int twoWay, 
                           TimeValue* maxTimeWait);

  int handleInputI ();

  virtual int registerHandler (void);

protected:

  TcpConnectionHandler* mConnectionHandler;
};

/*
class TcpClientTransport : public TcpTransport
{
public:
  TcpClientTransport (TcpClientConnectionHandler* handler, Communicator* comm);

  ~TcpClientTransport (void);

  TcpClientConnectionHandler* clientHandler (void);

  virtual void startRequest (Communicator* comm, Reference* ref, OutputStream& output);

  virtual int sendRequest (Reference* ref, 
                         Communicator* comm, 
                         OutputStream& stream, 
                         int twoWay, 
                         TimeValue* maxTimeWait) ;

  virtual int handleClientInput (int block = 0, TimeValue* maxWaitTime = 0);

  virtual int registerHandler (void);

private:
  TcpClientConnectionHandler* mClientHandler;
};

class TcpServerTransport : public TcpTransport
{
public:

  TcpServerTransport (TcpServerConnectionHandler* handler, Communicator* comm, ObjectAdapter* oa);

  ~TcpServerTransport (void);

  TcpServerConnectionHandler* serverHandler;

  IcmMessageState messageState;

};
*/

#endif //ICM_TCP_TRANSPORT_H
