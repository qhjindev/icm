#ifndef ICM_REPLY_DISPATCHER_H
#define ICM_REPLY_DISPATCHER_H

//#include "icm/IcmStream.h"
#include "icm/IcmProtocol.h"

//class IcmMessageState;
class Communicator;
class InputStream;
class IcmTransport;

class ReplyHandler
{
public:

  virtual void __response (InputStream* in, ACE_CDR::UInt replyStatus) = 0;
};

class ReplyDispatcher
{
  // = TITLE
  //
  // = DESCRIPTION
  //
public:
  ReplyDispatcher (void);

  virtual ~ReplyDispatcher (void);

  // Dispatch the reply. Return 1 on sucess, -1 on error.
  virtual int dispatchReply (ACE_CDR::UInt replyStatus, IcmMessageState* messageState) = 0;

  // Get the Message State into which the reply has been read.
  virtual IcmMessageState* messageState (void);

};

class SynchReplyDispatcher : public ReplyDispatcher
{
  // = TITLE
  //
  //     Reply dispatcher for Synchoronous Method Invocation (SMI)s.
  //
  // = DESCRIPTION
  //
public:
  SynchReplyDispatcher (Communicator* comm);

  virtual ~SynchReplyDispatcher (void);

  // Get the reply status.
  unsigned int replyStatus (void) const;

  // Dispatch the reply. Copy the buffers and return. Since the
  // invocation is synchronous demarshalling will take place on the
  // stack.
  // Return 1 on sucess, -1 on error.
  virtual int dispatchReply (ACE_CDR::UInt replyStatus, IcmMessageState* messageState);

  // Return the reference to the reply received flag. This will not
  // make sense in the Asynch Reply Dispatcher case, since the
  // reply will be dispatched as soon as it is available and the
  // dispatcher will go away immediately after that.
  int& replyReceived (void);

  // Return the reply CDR.
  virtual InputStream& replyStream (void);

  // Return the message state of this invocation.
  virtual IcmMessageState* messageState (void);

private:
  // Reply status.
  ACE_CDR::UInt mReplyStatus;

  // Flag that indicates the reply  has been received.
  int mReplyReceived;

  // All the state required to receive the input...
  IcmMessageState mMessageState;

  // Cache the Communicator pointer.
  Communicator* mCommunicator;
};

class AsynchReplyDispatcher : public ReplyDispatcher
{
  // = TITLE
  //
  //     Reply dispatcher for Asynchoronous Method Invocation (AMI)s.
  //
  // = DESCRIPTION
  //

public:
  AsynchReplyDispatcher (ReplyHandler* replyHandler);

  // Get the reply status.
  unsigned int replyStatus (void) const;

  virtual ~AsynchReplyDispatcher (void);

  // Dispatch the reply. This involves demarshalling the reply and
  // calling the appropriate call back hook method on the reply
  // handler.
  // Return 1 on sucess, -1 on error.
  virtual int dispatchReply (ACE_CDR::UInt replyStatus, IcmMessageState* messageState);

  // Return the message state.
  virtual IcmMessageState* messageState (void);

  void transport (IcmTransport* t);
  
private:
  // Reply status.
  ACE_CDR::UInt mReplyStatus;

  // CDR stream for reading the input.
  // message_state should go away. All we need is the reply
  // cdr. Is that right?
  IcmMessageState* mMessageState;

  // Reply Handler passed in the Asynchronous Invocation.
  ReplyHandler* mReplyHandler;

  // This invocation is using this transport, may change...
  IcmTransport* mTransport;
};

#endif //ICM_REPLY_DISPATCHER_H
