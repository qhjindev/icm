#ifndef ICM_INVOCATION_H
#define ICM_INVOCATION_H

#include "icm/IcmStream.h"
#include "icm/ReplyDispatcher.h"

class Reference;
class Communicator;
class IcmTransport;
class TimeValue;
class OutputStream;
class MuxedTms;

class Invocation
{
public:
  Invocation(Reference* ref, const char* operation, Communicator* comm, TimeValue* maxWaitTime = new TimeValue(60));

  virtual ~Invocation();

  int start(IcmTransport* &transport);

  int prepareHeader (ACE_CDR::Octet responseFlags);

  UInt requestId (void);

  int invoke(bool isRoundTrip);

  int closeConnection (void);

  OutputStream* outStream (void);

  void outStream(OutputStream* os);

  void setOperation(const char* operation);

protected:
  Reference* mReference;

  const char* mOpName;

  UInt mRequestId;

  //char mBuffer[ACE_CDR::DEFAULT_BUFSIZE];

  OutputStream *mOutStream;

  Communicator* mCommunicator;

  IcmTransport* mTransport;

  TimeValue* mMaxWaitTime;
  //BasicStream mInStream;
};

class TwowayInvocation : public Invocation
{
public:
  TwowayInvocation (Reference* ref, const char* operation, Communicator* comm, TimeValue* maxWaitTime = new TimeValue(60));

  ~TwowayInvocation (void);

  int start (IcmTransport* &transport);

  int invoke ();

  InputStream* inpStream (void);

  void onTmsDestructed() {
    tms = 0;
  }

private:
  // Reply dispatcher for the current synchronous invocation.
  SynchReplyDispatcher* mRd;

  MuxedTms* tms;
};

class OnewayInvocation : public Invocation
{
public:
  OnewayInvocation (Reference* ref, const char* operation, Communicator* comm, TimeValue* maxWaitTime = new TimeValue(60));

  ~OnewayInvocation (void);

  int start (IcmTransport* transport = 0);

  int invoke ();
private:
  SynchReplyDispatcher* rd;
};

class TwowayAsynchInvocation : public Invocation
{
public:
  TwowayAsynchInvocation (Reference* ref, 
                          const char* operation, 
                          Communicator* comm,
                          ReplyHandler* replyHandler,
                          TimeValue* maxWaitTime = new TimeValue(60));

  ~TwowayAsynchInvocation (void);

  int start (IcmTransport* &transport);

  int invoke ();

private:
  AsynchReplyDispatcher* mRd;
  bool mRdHasBound;
};

#endif //ICM_INVOCATION_H
