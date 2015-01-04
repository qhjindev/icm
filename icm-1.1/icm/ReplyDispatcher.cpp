
#include "os/OS.h"
#include "icm/IcmStream.h"

#include "icm/ReplyDispatcher.h"
#include "icm/Communicator.h"
#include "icm/IcmProtocol.h"

ReplyDispatcher::ReplyDispatcher (void)
{
}

ReplyDispatcher::~ReplyDispatcher (void)
{
}

IcmMessageState*
ReplyDispatcher::messageState (void)
{
  return 0;
}

// *************************************************************

SynchReplyDispatcher::SynchReplyDispatcher (Communicator* comm)
: mReplyReceived (0),
  mMessageState (),
  mCommunicator (comm)
{
}

SynchReplyDispatcher::~SynchReplyDispatcher (void)
{
}

int
SynchReplyDispatcher::dispatchReply(ACE_CDR::UInt replyStatus, IcmMessageState *messageState)
{
  this->mReplyReceived = 1;

  this->mReplyStatus = replyStatus;

  // Must reset the message state, it is possible that the same reply
  // dispatcher is used because the request must be re-sent.
  this->mMessageState.reset (0);

  if (&this->mMessageState != messageState) {
    // The Transport Mux Strategy did not use our Message_State to
    // receive the event, possibly because it is muxing multiple
    // requests over the same connection.

    // Steal the buffer so that no copying is done.
    this->mMessageState.stream.steal_from (messageState->stream);

    // There is no need to copy the other fields!
  }

  return 1;
}

IcmMessageState*
SynchReplyDispatcher::messageState (void)
{
  return &this->mMessageState;
}

InputStream&
SynchReplyDispatcher::replyStream (void)
{
  return this->mMessageState.stream;
}

int&
SynchReplyDispatcher::replyReceived (void)
{
  return this->mReplyReceived;
}

unsigned int 
SynchReplyDispatcher::replyStatus (void) const
{
  return this->mReplyStatus;
}

// ********************************************************************

AsynchReplyDispatcher::AsynchReplyDispatcher(ReplyHandler *replyHandler)
: mReplyHandler (replyHandler),
  mTransport (0)
{
}

AsynchReplyDispatcher::~AsynchReplyDispatcher (void)
{
  if(mReplyHandler != 0) {
    delete mReplyHandler;
  }
}

int
AsynchReplyDispatcher::dispatchReply(ACE_CDR::UInt replyStatus, IcmMessageState *messageState)
{
  this->mReplyStatus = replyStatus;

  this->mMessageState = messageState;

//  ICC_INFO("AsynchReplyDispatcher::dispatchReply");

//  unsigned int replyError = 0;
//  switch(replyStatus) {
//    case 0:
//      replyError = 0;
//      break;
//    case 1:
//      replyError = 1;
//      break;
//    default:
//      replyError = 2;
//  }

  this->mReplyHandler->__response (&this->mMessageState->stream, replyStatus);

  // This was dynamically allocated. Now the job is done. Commit suicide here.
  delete this;

  return 1;
}

IcmMessageState*
AsynchReplyDispatcher::messageState (void)
{
  return this->mMessageState;
}

void
AsynchReplyDispatcher::transport (IcmTransport* t)
{
  this->mTransport = t;
}
