
#include "icc/Log.h"
#include "icm/TransportMuxStrategy.h"
#include "icm/ReplyDispatcher.h"
#include "icm/Pluggable.h"
#include "icm/Invocation.h"
#include "icc/Guard.h"

using namespace std;

TransportMuxStrategy::TransportMuxStrategy(IcmTransport *transport)
: mTransport(transport)
{
}

TransportMuxStrategy::~TransportMuxStrategy (void)
{
  if(this->mTransport != 0) {
    mTransport->tms(0);
  }
}

int
TransportMuxStrategy::bindDispatcher(UInt requestId, 
                                     ReplyDispatcher *rd)
{
  return 1;
}

ExclusiveTms::ExclusiveTms(IcmTransport *transport)
: TransportMuxStrategy (transport),
  mRequestIdGenerator (0),
  mRequestId (0),
  mRd (0),
  mMessageState (transport->communicator ())
{
}

ExclusiveTms::~ExclusiveTms (void)
{
}

// Generate and return an unique request id for the current
// invocation. We can actually return a predecided ULong, since we
// allow only one invocation over this connection at a time.
UInt
ExclusiveTms::requestId (void)
{
  return this->mRequestIdGenerator++;
}

void
ExclusiveTms::requestId (UInt id)
{
  mRequestIdGenerator = id;
}

// Bind the handler with the request id.
int
ExclusiveTms::bindDispatcher(UInt requestId, ReplyDispatcher *rd)
{
  this->mRequestId = requestId;
  this->mRd = rd;

  if (this->mMessageState.messageSize != 0)
    this->mMessageState.reset ();

  return TransportMuxStrategy::bindDispatcher (requestId, rd);
}

//int
//ExclusiveTms::unbindDispatcher(UInt requestId) {
//  return 0;
//}

int
ExclusiveTms::dispatchReply (UInt requestId, UInt replyStatus, IcmMessageState* messageState)
{
  // Check the ids.
  if (this->mRequestId != requestId)
  {
//    ICC_DEBUG("ExclusiveTms::dispatchReply - <%d != %d>", this->mRequestId, requestId);
    return 0;
  }

  ReplyDispatcher* rd = this->mRd;
  this->mRequestId = 0xdeadbeef; // @@ What is a good value???
  this->mRd = 0;

  // Dispatch the reply.
  int result = rd->dispatchReply (replyStatus, messageState);

  return result;
}

IcmMessageState*
ExclusiveTms::getMessageState (void)
{
  if (this->mRd != 0) {
    IcmMessageState* rdMessageState = this->mRd->messageState ();
    if (rdMessageState == 0)
      return &this->mMessageState;
  }

  return &this->mMessageState;
}

void
ExclusiveTms::destroyMessageState(IcmMessageState *)
{
}

// **************************************************

MuxedTms::MuxedTms(IcmTransport *transport)
: TransportMuxStrategy (transport),
  mRequestIdGenerator (0),
  mCommunicator (transport->communicator()),
  mMessageState (0)
{
}

MuxedTms::~MuxedTms (void)
{
  ACE_GUARD(ThreadMutex, guard, mutex);
  for(set<TwowayInvocation*>::iterator it = observers.begin();
      it != observers.end();
      ++it)
  {
    (*it)->onTmsDestructed();
  }

  for(RequestDispatcherTableIter it = mDispatcherTable.begin();
      it != mDispatcherTable.end();
      ++it)
  {
    delete it->second;
  }
}

// Generate and return an unique request id for the current
// invocation.
UInt
MuxedTms::requestId (void)
{
  return this->mRequestIdGenerator++;
}

void
MuxedTms::requestId (UInt id)
{
  mRequestIdGenerator = id;
}

// Bind the dispatcher with the request id.
int
MuxedTms::bindDispatcher(UInt requestId, ReplyDispatcher *rd)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);
  this->mDispatcherTable.insert (std::make_pair (requestId, rd));

  return TransportMuxStrategy::bindDispatcher (requestId, rd);
}

//int
//MuxedTms::unbindDispatcher(UInt requestId)
//{
//  this->mDispatcherTable.erase(requestId);
//  return 0;
//}

int
MuxedTms::unbindDispatcher(ReplyDispatcher* rd, bool del)
{
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);
  if(del) {
    delete rd;
  }

  for(map<unsigned long, ReplyDispatcher*>::iterator it = mDispatcherTable.begin();
      it != mDispatcherTable.end();)
  {
    map<unsigned long, ReplyDispatcher*>::iterator curIt = it++;
    if(curIt->second == rd) {
      mDispatcherTable.erase(curIt);
    }
  }
  return 0;
}

int
MuxedTms::dispatchReply(UInt requestId, UInt replyStatus, IcmMessageState* messageState)
{
  // This message state should be the same as the one we have here,
  // which we gave to the Transport to read the message. Just a sanity
  // check here.
  //assert (messageState == this->mMessageState);

//  int result = 0;
  ReplyDispatcher* rd = 0;

  {
  ACE_GUARD_RETURN(ThreadMutex, guard, mutex, -1);

  // Grab the reply dispatcher for this id.
  RequestDispatcherTableIter iter = this->mDispatcherTable.find (requestId);
  if (iter == this->mDispatcherTable.end ())
    return -1;
  rd = iter->second;

//  result = (int)this->mDispatcherTable.erase (requestId);
//  if (result != 1) {
//    ICC_DEBUG("MuxedTms::dispatchReply: unbind dispatcher failed: result = %d",result);
//    return -1;
//  }
  mDispatcherTable.erase (iter);
  }

  // Dispatch the reply.
  return rd->dispatchReply (replyStatus, messageState);

  // No need for idling Transport, it would have got idle'd soon after
  // sending the request.
}

IcmMessageState*
MuxedTms::getMessageState (void)
{
  if (this->mMessageState == 0)
  {
    // Create the next message state.
    this->mMessageState = new IcmMessageState (this->mCommunicator);
  }

  return this->mMessageState;
}

void
MuxedTms::destroyMessageState(IcmMessageState *)
{
  delete this->mMessageState;
  this->mMessageState = 0;
}

void MuxedTms::addObserver(TwowayInvocation* observer) {
  observers.insert(observer);
}

void MuxedTms::removeObserver(TwowayInvocation* observer){
  observers.erase(observer);
}


