
#include "icm/Reference.h"
#include "icm/Communicator.h"
#include "icm/Pluggable.h"
#include "icc/TimeValue.h"
#include "icc/Connector.h"
#include "icm/Invocation.h"
#include "icm/IcmProtocol.h"
#include "icm/TcpConnection.h"
#include "icm/TransportMuxStrategy.h"
#include "icm/WaitStrategy.h"
#include "icm/ResourceFactory.h"
#include "icm/IcmProtocol.h"

Invocation::Invocation(Reference *ref, 
                       const char *operation, 
                       Communicator* comm,
                       TimeValue* maxWaitTime)
: mReference(ref), 
  mOpName(operation), 
  mRequestId(0), 
  //mOutStream (mBuffer, sizeof mBuffer),
  mCommunicator (comm), 
  mMaxWaitTime (maxWaitTime)
{
  mOutStream = new OutputStream();
}

void
Invocation::setOperation(const char* operation)
{
  mOpName = operation;
}

Invocation::~Invocation(void)
{
  if (mOutStream)
    delete mOutStream;

//  if (mMaxWaitTime)
//    delete mMaxWaitTime;
}

OutputStream*
Invocation::outStream (void)
{
  return this->mOutStream;
}

void
Invocation::outStream(OutputStream* os)
{
  if(this->mOutStream)
    delete this->mOutStream;
  this->mOutStream = os;
}

UInt
Invocation::requestId (void)
{
  return this->mRequestId;
}

int
Invocation::start(IcmTransport* &transport)
{
  if (transport != 0 && this->mCommunicator->validateTransport(transport) && transport->tms() != 0) {
    this->mTransport = transport;
  } else {
    //Establish the transport by reference in object
    ConnectorRegistry* connReg = this->mCommunicator->connectorRegistry ();
    if (connReg == 0)
      return -1;

    int result = connReg->connect (this->mReference, this->mTransport);
    if (result == -1) {
      if (errno == ETIME)
        return -ICM_TIMEOUT_CONNECT;
      return result;
    }

    assert(mTransport->tms() != 0);
    transport = mTransport;
  }

  if(this->mTransport->tms () == 0) {
    return -1;
  }

  UInt requestId = this->mTransport->tms ()->requestId();
  this->mRequestId = requestId;

  return 0;
}

int
Invocation::prepareHeader (ACE_CDR::Octet responseFlags)
{
  if (IcmProtocol::writeRequestHeader (this->mRequestId,
                                       responseFlags,
                                       this->mReference->identity(),
                                       this->mOpName,
                                       *this->mOutStream) == false)
  {
    return -ICM_MARSHAL;
  }

  return 0;
}

int Invocation::invoke(bool isRoundTrip)
{
  if (this->mTransport == 0 || !mCommunicator->validateTransport(mTransport))
    return -1;

  if(this->mTransport->sendRequest(this->mReference, 
                                   this->mCommunicator,
                                   *this->mOutStream,
                                   isRoundTrip,
                                   this->mMaxWaitTime) == -1)
  {
    this->mTransport->closeConnection();
    this->mTransport = 0;
    
    return -1;
  }

  return 0;
}

TwowayInvocation::TwowayInvocation (Reference* ref, const char* operation, Communicator* comm, TimeValue* maxWaitTime)
: Invocation (ref, operation, comm, maxWaitTime), tms(0)
{
  mRd = new SynchReplyDispatcher(comm);
}

TwowayInvocation::~TwowayInvocation (void)
{
  if(tms != 0) {
    tms->unbindDispatcher(this->mRd);
    tms->removeObserver(this);
  } else {
    delete mRd;
  }
}

InputStream*
TwowayInvocation::inpStream (void)
{
  return &this->mRd->replyStream ();
}

int
TwowayInvocation::start (IcmTransport* &transport)
{
  int ret = Invocation::start (transport);
  if(ret != 0)
    return ret;

  if(mTransport != 0) {
    this->tms = dynamic_cast<MuxedTms*>( mTransport->tms());
    if(tms != 0) {
      tms->addObserver(this);
    }
  }

  this->mTransport->startRequest (this->mCommunicator, this->mReference, *this->mOutStream);

  return 0;
}

int
TwowayInvocation::invoke ()
{
  TransportMuxStrategy* tms = this->mTransport->tms ();
  int retval = tms->bindDispatcher (mRequestId, mRd);

  if (retval == -1) {

    this->closeConnection ();
    return -1;
  }

  retval = Invocation::invoke (true);
  if (retval != 0)
    return retval;

  WaitStrategy* ws = this->mTransport->waitStrategy ();
  int replyError = ws->wait (mMaxWaitTime, mRd->replyReceived());
  if (replyError == -1) {
    //tms->unbindDispatcher(mRequestId);

    if(!ws->getConnClosed())
      this->closeConnection ();
    else
      this->mTransport = 0;

    if(errno == ETIME) {
      //when timeout, connection also need to be closed, or the server will send back reply later...
      return -ICM_TIMEOUT_SEND;
    }

    return -ICM_INVOCATION_RECV_REQUEST;
  }

  unsigned int replyStatus = mRd->replyStatus ();
  switch (replyStatus)
  {
  case 0:
      return 0;
  case 1:
      return -1;
  default:
      return -1;
  }

  return 0;
}

int
Invocation::closeConnection (void)
{
  if(this->mTransport && this->mCommunicator->validateTransport(mTransport))
    this->mTransport->closeConnection ();

  return 0;
}

OnewayInvocation::OnewayInvocation(Reference *ref, const char *operation, Communicator *comm, TimeValue* maxWaitTime)
: Invocation (ref, operation, comm, maxWaitTime)
{
  rd = new SynchReplyDispatcher(comm);
}

OnewayInvocation::~OnewayInvocation (void)
{
}

int
OnewayInvocation::start (IcmTransport* transport)
{
  int ret = Invocation::start (transport);
  if(ret != 0)
    return ret;

  this->mTransport->startRequest (this->mCommunicator, this->mReference, *this->mOutStream);

  return 0;
}

int
OnewayInvocation::invoke ()
{
  //return Invocation::invoke (false);
  
//  SynchReplyDispatcher rd (this->mCommunicator);

  int retval = this->mTransport->tms ()->bindDispatcher (this->mRequestId, rd);

  if (retval == -1) {
    this->closeConnection ();
    return -1;
  }

  retval = Invocation::invoke (true);
  if (retval != 0)
    return retval;

  int replyError = this->mTransport->waitStrategy ()->wait (this->mMaxWaitTime, rd->replyReceived());

  if (replyError == -1) {
    /////
    this->closeConnection ();
  }

  unsigned int replyStatus = rd->replyStatus ();
  switch (replyStatus)
  {
  case 0:
      return 0;
    case 1:
      return -1;
    default:
      return -1;
  }

  return 0;
}

TwowayAsynchInvocation::TwowayAsynchInvocation(Reference *ref, const char *operation, Communicator *comm, ReplyHandler* replyHandler, TimeValue* maxWaitTime)
: Invocation (ref, operation, comm, maxWaitTime), mRd (0), mRdHasBound(false)
{
  this->mRd = new AsynchReplyDispatcher (replyHandler);
}

TwowayAsynchInvocation::~TwowayAsynchInvocation (void) {
  if(!mRdHasBound) {
    delete mRd;
  }
}
int
TwowayAsynchInvocation::start (IcmTransport* &transport)
{
  int ret = Invocation::start (transport);
  if(ret != 0)
    return ret;

  this->mTransport->startRequest (this->mCommunicator, this->mReference, *this->mOutStream);

  return 0;
}

int
TwowayAsynchInvocation::invoke ()
{
  if(this->mTransport == 0 || !mCommunicator->validateTransport(mTransport))
    return -1;

  if(this->mTransport->tms () == 0)
    return -1;

  TransportMuxStrategy* tms = this->mTransport->tms ();
//  UInt rid = this->mRequestId;
//  AsynchReplyDispatcher* rd = &this->mRd;
//  IcmTransport* transport = tms->mTransport;

//  int retval = this->mTransport->tms ()->bindDispatcher (this->mRequestId, this->mRd);
  int retval = tms->bindDispatcher (mRequestId, mRd);
  if (retval == -1) {
    this->closeConnection ();
    return -1;
  }
  mRdHasBound = true;

  retval = Invocation::invoke (0);

  if (retval != 0)
    return retval;

  mRd->transport (this->mTransport);

  return 0;
}
