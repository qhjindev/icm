#ifndef ICM_TRANSPORT_MUX_STRATEGY_H
#define ICM_TRANSPORT_MUX_STRATEGY_H

#include <map>
#include <set>
#include "icm/IcmProtocol.h"
#include "icm/ReplyDispatcher.h"
#include "icc/ThreadMutex.h"

class IcmTransport;
class TwowayInvocation;

class TransportMuxStrategy
{
  // = TITLE
  //
  //     Strategy to determine whether the connection should be
  //     multiplexed for multiple requests or it is exclusive for a
  //     single request at a time.
  //
  // = DESCRIPTION
  //

public:
  // Base class constructor.
  TransportMuxStrategy (IcmTransport* transport);

  // Base class destructor.
  virtual ~TransportMuxStrategy (void);

  // Generate and return an unique request id for the current
  // invocation.
  virtual UInt requestId (void) = 0;

  virtual void requestId (UInt) = 0;

  // = Bind and Find methods for the <Request ID, ReplyDispatcher>
  //   pairs.
  virtual int bindDispatcher (UInt requestId, ReplyDispatcher* rd);
  // Bind the dispatcher with the request id. Commonalities in the
  // derived class implementations is kept here.

//  virtual int unbindDispatcher(UInt requestId) = 0;

  // Dispatch the reply for <request_id>, cleanup any resources
  // allocated for that request.
  virtual int dispatchReply (UInt requestId, 
                             UInt replyStatus, 
                             IcmMessageState* messageState) = 0;

  // = "Factory methods" to obtain the CDR stream, in the Muxed case
  //    the factory simply allocates a new one, in the Exclusive case
  //    the factory returns a pointer to the pre-allocated CDR.

  // Get a CDR stream.
  virtual IcmMessageState* getMessageState (void) = 0;

  // Destroy a CDR stream.
  virtual void destroyMessageState (IcmMessageState *) = 0;

protected:
  // Cache the transport reference.
  IcmTransport* mTransport;
};

class ExclusiveTms : public TransportMuxStrategy
{
  // = TITLE
  //
  //    Connection exclusive for the request.
  //
  // = DESCRIPTION
  //

public:
  // Constructor.
  ExclusiveTms (IcmTransport* transport);

  // Destructor.
  virtual ~ExclusiveTms (void);

  // Generate and return an unique request id for the current
  // invocation.
  virtual UInt requestId (void);
  virtual void requestId (UInt);

  // Bind the dispatcher with the request id.
  virtual int bindDispatcher (UInt requestId, ReplyDispatcher* rd);
//  virtual int unbindDispatcher(UInt requestId);

  // Dispatch the reply for <request_id>, cleanup any resources
  // allocated for that request.
  virtual int dispatchReply (UInt requestId, 
                             UInt replyStatus, 
                             IcmMessageState* messageState);

  // Return the pre-allocated message state.
  virtual IcmMessageState* getMessageState (void);

  // No op in this strategy.
  virtual void destroyMessageState (IcmMessageState *);

protected:
  // Used to generate a different request_id on each call to
  // request_id().
  UInt mRequestIdGenerator;

  // Request id for the current request.
  UInt mRequestId;

  // Reply Dispatcher corresponding to the request.
  ReplyDispatcher* mRd;

  // Message state to read the incoming message.
  IcmMessageState mMessageState;
};

class ReplyDispatcher;

class MuxedTms : public TransportMuxStrategy
{
  // = TITLE
  //
  //    Connection is multiplexed for many requests.
  //
  // = DESCRIPTION
  //

public:
  // Constructor.
  MuxedTms (IcmTransport* transport);

  // Destructor.
  virtual ~MuxedTms (void);

  // Generate and return an unique request id for the current
  // invocation.
  virtual UInt requestId (void);
  virtual void requestId (UInt);

  // Bind the dispatcher with the request id.
  virtual int bindDispatcher (UInt requestId, ReplyDispatcher* rd);

//  virtual int unbindDispatcher(UInt requestId);

  virtual int unbindDispatcher(ReplyDispatcher* rd, bool del = true);

  // Dispatch the reply for <request_id>, cleanup any resources
  // allocated for that request.
  virtual int dispatchReply (UInt requestId, 
                             UInt replyStatus, 
                             IcmMessageState* messageState);

  // Return the message state.
  virtual IcmMessageState* getMessageState (void);

  // No op in this strategy.
  virtual void destroyMessageState (IcmMessageState *);

  void addObserver(TwowayInvocation* observer);

  void removeObserver(TwowayInvocation* observer);

protected:
  // Used to generate a different request_id on each call to
  // request_id().
  unsigned long mRequestIdGenerator;

  typedef std::map <unsigned long, ReplyDispatcher*> RequestDispatcherTable;
  typedef std::map <unsigned long, ReplyDispatcher*>::iterator RequestDispatcherTableIter;

  // Table of <Request ID, Reply Dispatcher> pairs.
  RequestDispatcherTable mDispatcherTable;

  // Keep track of the communicator pointer. We need to this to create the
  // Reply Dispatchers.
  Communicator* mCommunicator;

  // Message state where the current input message is being read. This
  // is created at start of each incoming message. When that message
  // is read, the message is processed and for the next message a new
  // message state is created.
  IcmMessageState* mMessageState;

  std::set<TwowayInvocation*> observers;
  ThreadMutex mutex; // async rpc use multi thread, so need mutex to protect data member(mDispatcherTable)
};

#endif //ICM_TRANSPORT_MUX_STRATEGY_H
