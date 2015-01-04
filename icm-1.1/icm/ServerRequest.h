#ifndef ICM_SERVER_REQUEST
#define ICM_SERVER_REQUEST

#include <string>

#include "os/Basic_Types.h"
#include "icm/IcmStream.h"
#include "icm/Reference.h"


class Communicator;
class IcmTransport;

class ServerRequest
{
public:
  ServerRequest(InputStream& input, 
                OutputStream& output,
                IcmTransport* transport,
                Communicator* comm,
                int& parseError);

  ServerRequest(ACE_CDR::UInt& requestId,
                ACE_CDR::Boolean& responseExpected,
                Identity& identity,
                const std::string& operation,
                OutputStream& output,
                IcmTransport* transport,
                Communicator* comm,
                int& parseError);

  ACE_CDR::UInt requestId(void)
  {
    return this->mRequestId;
  }

  ACE_CDR::Boolean responseExpected(void) const
  {
    return this->mResponseExpected;
  }

  InputStream* incoming(void)
  {
    return this->mIncoming;
  }

  OutputStream* outgoing(void)
  {
    return this->mOutgoing;
  }

  const char* operation(void) const
  {
    return this->mOperation.c_str ();
  }

  Identity& identity(void) 
  {
    return this->mIdentity;
  }

  IcmTransport* transport (void)
  {
    return this->mTransport;
  }

  Communicator* communicator (void)
  {
    return this->mCommunicator;
  }

  void initReply ();

private:

  int parseHeader (void);

  std::string mOperation;
  InputStream* mIncoming;
  OutputStream* mOutgoing;
  ACE_CDR::Boolean mResponseExpected;
  int mRetval;
  IcmTransport* mTransport;
  Communicator* mCommunicator;
  ACE_CDR::UInt mRequestId;
  Identity mIdentity;
};

#endif //ICM_SERVER_REQUEST
