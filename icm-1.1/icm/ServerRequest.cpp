
#include "icm/IcmProtocol.h"
#include "icm/ServerRequest.h"
//#include "icm/IcmStream.h"
#include "icm/Communicator.h"

ServerRequest::ServerRequest(InputStream& input, 
                             OutputStream& output,
                             IcmTransport* transport,
                              Communicator* comm,
                              int& parseError)
: mIncoming(&input), 
  mOutgoing(&output), 
  mResponseExpected(0), 
  mRetval(0),
  mTransport (transport),
  mCommunicator (comm),
  mRequestId(0)
{
  parseError = this->parseHeader ();
}

ServerRequest::ServerRequest(ACE_CDR::UInt& requestId,
                             ACE_CDR::Boolean& responseExpected,
                             Identity& identity,
                             const std::string& operation,
                             OutputStream& output,
                             IcmTransport* transport,
                             Communicator* comm,
                             int& parseError)
: mOperation (operation),
  mIncoming (0),
  mOutgoing (&output),
  mResponseExpected (responseExpected),
  mRetval (0),
  mTransport (transport),
  mCommunicator (comm),
  mRequestId (requestId),
  mIdentity (identity)
{
  parseError = 0;
}

int
ServerRequest::parseHeader (void)
{
  InputStream& input = *this->mIncoming;

  bool hdrStatus = input.read_uint (this->mRequestId); 

  ACE_CDR::Octet responseFlags;
  hdrStatus = input.read_octet (responseFlags);
  this->mResponseExpected = (responseFlags != 0);

  hdrStatus = input.read_string (this->mIdentity.name);

  hdrStatus = input.read_string (this->mIdentity.category);

  hdrStatus = input.read_string (this->mOperation);

  return hdrStatus ? 0 : -1;
}

void
ServerRequest::initReply ()
{
  // Construct a REPLY header.
  IcmProtocol::startMessage (IcmProtocol::Reply,
                             *this->mOutgoing);

  this->mOutgoing->write_uint (this->mRequestId);
  this->mOutgoing->write_uint (ICM_NO_EXCEPTION);
}
