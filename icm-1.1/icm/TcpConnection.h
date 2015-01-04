#ifndef ICM_CONNECTION_H
#define ICM_CONNECTION_H

#include "icc/SvcHandler.h"
#include "icm/IcmStream.h"
#include "icm/TcpTransport.h"

//class TcpClientTransport;
//class TcpServerTransport;

class TcpConnectionHandler : public SvcHandler
{
  typedef SvcHandler super;
public:

  TcpConnectionHandler (Communicator* comm = 0, ObjectAdapter* oa = 0);

  ~TcpConnectionHandler (void);

  virtual int open(void*);

  //virtual int activate (void);

  virtual int svc (void);

  virtual int handleInput(ACE_HANDLE fd);

  virtual int handleClose(ACE_HANDLE h,ReactorMask mask);

  IcmTransport* transport (void);

  InetAddr getLocalAddr() {
    return localAddr;
  }

  InetAddr getRemoteAddr() {
    return remoteAddr;
  }

protected:

  //int handleInputI (ACE_HANDLE = ACE_INVALID_HANDLE, TimeValue* maxWaitTime = 0);

  Communicator* mCommunicator;

  TcpTransport mTransport;

  InetAddr localAddr;
  InetAddr remoteAddr;
};

/*
class TcpServerConnectionHandler : public SvcHandler
{
public:

  TcpServerConnectionHandler (Communicator* comm = 0, ObjectAdapter* oa = 0);

  ~TcpServerConnectionHandler (void);

  virtual int open(void*);

  virtual int svc ();

  IcmTransport* transport (void);

protected:

  int handleInputI (ACE_HANDLE, TimeValue* maxWaitTime = 0);

  virtual int handleInput(ACE_HANDLE fd);

  virtual int handleClose(ACE_HANDLE h,ReactorMask mask);

  void sendResponse(BasicStream& output);

private:

  TcpServerTransport mTransport;

  Communicator* mCommunicator;
};
*/

#endif //ICM_CONNECTION_H
