
#include "icc/Log.h"
#include "icm/Pluggable.h"
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Reference.h"
#include "icc/MessageBlock.h"
#include "icc/TimeValue.h"
#include "icm/IcmStream.h"
#include "icm/TransportMuxStrategy.h"
#include "icm/WaitStrategy.h"
#include "icm/ClientStrategyFactory.h"

IcmTransport::IcmTransport (Communicator* comm, AmhResponseHandler* amhResponseHandler)
: mCommunicator (comm),
  mObjectAdapter (0),
  mTms (0),
  mWs (0),
  mAmhResponseHandler(amhResponseHandler)
{
  assert(comm != 0);
  this->mWs = comm->clientFactory ()->createWaitStrategy (this);

  this->mTms = comm->clientFactory ()->createTransportMuxStrategy (this);

  this->mCommunicator->addValidTransport(this);
}

IcmTransport::~IcmTransport(void)
{
  if(mCommunicator) {
    mCommunicator->removeValidTransport(this);
  }

  delete this->mWs;
  this->mWs = 0;
  delete this->mTms;
  this->mTms = 0;

  if(mAmhResponseHandler != 0) {
//    ICC_ERROR("IcmTransport::~IcmTransport, calling mAmhResponseHandler->setConnectionError()");
    mAmhResponseHandler->setConnectionError();
  }
}

Communicator*
IcmTransport::communicator (void) const
{
  return this->mCommunicator;
}

ObjectAdapter*
IcmTransport::adapter(void) const
{
  return this->mObjectAdapter;
}

void
IcmTransport::adapter (ObjectAdapter* oa)
{
  this->mObjectAdapter = oa;
}

TransportMuxStrategy* 
IcmTransport::tms (void) const
{
  return this->mTms;
}

WaitStrategy* 
IcmTransport::waitStrategy (void) const
{
  return this->mWs;
}

int
IcmTransport::handleClientInput (int block, TimeValue*)
{
  return -1;
}

int
IcmTransport::registerHandler (void)
{
  return -1;
}

void IcmTransport::connClosed() {
  if(mWs)
    mWs->setConnClosed(true);
}

void
IcmTransport::startRequest (Communicator* comm, Reference* ref, OutputStream& output)
{
  IcmProtocol::startMessage (IcmProtocol::Request, output);
}

int
IcmTransport::handleInputI (ACE_HANDLE , TimeValue* maxWaitTime )
{
  int result = IcmProtocol::handleInput (this,
                                         this->mCommunicator,
                                         this->messageState,
                                         maxWaitTime);
  
  if (result == -1 && errno != 0)
  {
    LogMsg << "IcmTransport::handleInputI handleInput: " << perr << endl;
    return result;
  }

  if (result == 0 || result == -1)
  {
    //close ?
    //this->m
    return result;
  }

  IcmMessageState& ms = this->messageState;

  unsigned char messageType = ms.messageType;
  
  //this->messageState.reset (0);

  switch (messageType) {
    case IcmProtocol::Request:
      {
        InputStream inputStream(InputStream::Transfer_Contents (ms.stream));

        result = IcmProtocol::processServerMessage (this,
                                                    this->mCommunicator,
                                                    inputStream,
                                                    messageType);
        if (result != -1)
          result = 0;

        messageState.reset ();

        return result;
      }
    case IcmProtocol::Reply:
      {
        UInt requestId;
        UInt replyStatus;
        result = IcmProtocol::parseReply (this,
                                          this->mCommunicator,
                                          messageState,
                                          requestId,
                                          replyStatus);
        if (result == -1) {
          ////////
          messageState.reset ();
          return -1;
        }

        result = this->mTms->dispatchReply (requestId, replyStatus, &messageState);
        if (result == -1) {
          //////////
          messageState.reset ();
          return -1;
        } else if (result == 0) {
          messageState.reset ();
          return 0;
        }

        this->mTms->destroyMessageState (&messageState);
        messageState.reset ();

        return result;
      }
    case IcmProtocol::Error:
      {
        ICC_ERROR("Closing down the connection");
        return -1;
      }
  }

  return 0;
}
