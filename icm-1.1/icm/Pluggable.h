#ifndef ICM_PLUGGABLE_H
#define ICM_PLUGGABLE_H

#include <string>

#include "os/OS.h"
#include "icm/IcmProtocol.h"
#include "AmhResponseHandler.h"

class Communicator;
class Reference;
class MessageBlock;
class TimeValue;
class InputStream;
class OutputStream;
class TransportMuxStrategy;
class WaitStrategy;
class ObjectAdapter;

class IcmTransport
{
public:

  IcmTransport (Communicator* comm, AmhResponseHandler* amhResponseHandler = 0);

  virtual ~IcmTransport ();

  Communicator* communicator (void) const;

  ObjectAdapter* adapter (void) const;

  void adapter (ObjectAdapter* oa);

  TransportMuxStrategy* tms (void) const;

  void tms(TransportMuxStrategy* tms) {
    this->mTms = tms;
  }

  WaitStrategy* waitStrategy (void) const;

  virtual ACE_HANDLE handle (void) = 0;

  virtual ssize_t send (Reference* ref, 
                        int twoWay,
                        const MessageBlock* mb,
                        const TimeValue* s = 0) = 0;

  virtual ssize_t send (const MessageBlock* mb, 
                        const TimeValue* s = 0) = 0;

  virtual ssize_t send (const unsigned char* buf, 
                        size_t len, 
                        const TimeValue* s = 0) = 0;

  virtual ssize_t recv (char* buf, 
                        size_t len, 
                        const TimeValue* s = 0) = 0;

  virtual void startRequest (Communicator* comm, 
                             Reference* ref, 
                             OutputStream& output);

  virtual int sendRequest (Reference* ref, 
                           Communicator* comm, 
                           OutputStream& stream, 
                           int twoWay, 
                           TimeValue* maxTimeWait) = 0;

  virtual int handleClientInput (int block = 0, 
                                 TimeValue* maxWaitTime = 0);

  virtual int registerHandler (void);

  virtual void closeConnection (void) = 0;

  void connClosed();

  int handleInputI (ACE_HANDLE = ACE_INVALID_HANDLE, TimeValue* maxWaitTime = 0);

  void setResponsHandler(AmhResponseHandler* amhResponseHandler) {
    this->mAmhResponseHandler = amhResponseHandler;
  }

  void clearResponsHandler() {
    this->mAmhResponseHandler = 0;
  }

protected:

  Communicator* mCommunicator;

  ObjectAdapter* mObjectAdapter;

  TransportMuxStrategy* mTms;

  WaitStrategy* mWs;

  IcmMessageState messageState;

  AmhResponseHandler* mAmhResponseHandler;
};

class TransportListener
{
public:
  virtual void onTransportCreated(IcmTransport* transport) = 0;
  virtual void onTransportDestroyed(IcmTransport* transport) = 0;
};

class IcmAcceptor
{
public:
  IcmAcceptor (void)
  {}

  virtual ~IcmAcceptor (void)
  {}

  virtual int open (Communicator* comm, ObjectAdapter* oa, unsigned short port) = 0;

  virtual int openDefault (Communicator* comm) = 0;

  virtual int close (void) = 0;

  virtual const char* tag (void) = 0;

  virtual unsigned short getListenPort() = 0;
};

class IcmConnector
{
public:
  IcmConnector (const char* tag)
  {
    this->mTag = tag;
  }

  const char* tag (void)
  {
    return this->mTag.c_str ();
  }

  virtual ~IcmConnector (void)
  {}
  
  virtual int open (Communicator* comm) = 0;

  virtual int close (void) = 0;

  virtual int connect (InetAddr&, IcmTransport* &, TimeValue* maxWaitTime) = 0;

protected:

  Communicator* mCommunicator;

private:

  std::string mTag;
};

class IcmProtocolFactory
{
public:

  virtual IcmAcceptor* makeAcceptor (void)
  {
    return 0;
  }

  virtual IcmConnector* makeConnector (void)
  {
    return 0;
  }

  virtual int matchTag (const std::string & tag)
  {
    return 0;
  }

};

#endif //ICM_PLUGGABLE_H
