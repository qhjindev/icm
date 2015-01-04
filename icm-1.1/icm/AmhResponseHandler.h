#ifndef ICM_AMH_RESPONSE_HANDLER_H
#define ICM_AMH_RESPONSE_HANDLER_H

#include "os/OS.h"
#include "icm/IcmStream.h"

class ServerRequest;
class IcmTransport;
//class IcmMessageState;
class Communicator;
//class OutputStream;

class AmhResponseHandler
{
public:
  AmhResponseHandler (ServerRequest& serverRequest);

  virtual ~AmhResponseHandler (void);

  void setConnectionError();

  bool getConnectionError();

  IcmTransport* getTransport();

protected:

  void initReply (void);

  void sendReply (void);

protected:

  OutputStream* mOutStream;

private:

  //IcmMessageState* mMessageState;

  ACE_CDR::UInt mRequestId;

  ACE_CDR::Boolean mResponseExpected;

  IcmTransport* mTransport;

  Communicator* mCommunicator;

  bool mConnectionError;
};

#endif //ICM_AMH_RESPONSE_HANDLER_H
