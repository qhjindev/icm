
#include "icm/AmhResponseHandler.h"
#include "icm/ServerRequest.h"
#include "icm/Pluggable.h"
#include "icm/Communicator.h"
#include "icm/IcmProtocol.h"


AmhResponseHandler::AmhResponseHandler (ServerRequest& serverRequest) :
  mRequestId (serverRequest.requestId ()),
  mResponseExpected (serverRequest.responseExpected ()),
  mTransport (serverRequest.transport ()),
  mCommunicator (serverRequest.communicator ())
{
  mOutStream = new OutputStream;
  mConnectionError = false;
}

AmhResponseHandler::~AmhResponseHandler (void)
{
  delete mOutStream;
  if(mTransport != 0) {
    mTransport->clearResponsHandler();
  }
}

void
AmhResponseHandler::initReply (void)
{
  IcmProtocol::startMessage (IcmProtocol::Reply, *this->mOutStream);

  IcmProtocol::writeReplyHeader (this->mRequestId, 0, *this->mOutStream);
}

void
AmhResponseHandler::sendReply (void)
{
  if(mTransport == 0 || !mCommunicator->validateTransport(mTransport)) {
    ICC_ERROR("AmhResponseHandler::sendReply, transport error, stop sending");
    mTransport = 0;
    return;
  }
  int result = IcmProtocol::sendMessage (this->mTransport,
                            *this->mOutStream,
                            this->mCommunicator);
  if (result == -1) {
    ICC_ERROR("AmhResponseHandler::sendReply error, send error");
  }
}

void AmhResponseHandler::setConnectionError() {
//  ICC_ERROR("AmhResponseHandler::setConnectionError called");
  //this->mTransport = 0;
  mConnectionError = true;
}

bool AmhResponseHandler::getConnectionError() {
  return mConnectionError;
}

IcmTransport* AmhResponseHandler::getTransport() {
  return mTransport;
}
