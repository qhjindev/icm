
#include "icm/WaitStrategy.h"
#include "icm/Pluggable.h"
#include "icm/Communicator.h"
#include "icc/Reactor.h"

WaitStrategy::WaitStrategy (IcmTransport* transport)
: mTransport (transport), connClosed(false)
{
}

WaitStrategy::~WaitStrategy (void)
{
}

//int
//WaitStrategy::sendingRequest(Communicator *comm, int twoWay)
//{
//  return 0;
//}

// ****************** WaitOnReactor *******************************

WaitOnReactor::WaitOnReactor(IcmTransport *transport)
: WaitStrategy (transport)
{
}

WaitOnReactor::~WaitOnReactor (void)
{
}

int
WaitOnReactor::wait(TimeValue *maxWaitTime, int &replyReceived)
{
  // Reactor does not change inside the loop.
  Reactor* reactor = this->mTransport->communicator ()->reactor ();

  // Do the event loop, till we fully receive a reply.
  int result = 0;
  TimeValue callTime = OS::gettimeofday ();
  while (1) {
    // Run the event loop.
    result = reactor->handleEvents (maxWaitTime);

    // If we got our reply, no need to run the event loop any further.
    if (replyReceived)
      break;

    if(connClosed)
      return -1;

    // Did we timeout? If so, stop running the loop.
    TimeValue curTime = OS::gettimeofday ();
    if (result == 0 && 
        maxWaitTime != 0 &&
        curTime - callTime >= *maxWaitTime) {
      errno = ETIME;
//      ICC_DEBUG("Timeout after recv is %d seconds", (curTime - callTime).sec());
      return -1;
    }

    // Other errors? If so, stop running the loop.
    if (result == -1)
      return -1;

    // Otherwise, keep going...
  }

  return 0;
}

int
WaitOnReactor::handleInput (void)
{
  int result = this->mTransport->handleClientInput (0);

  if (result == -1)
    result = 0;

  return result;
}

int
WaitOnReactor::registerHandler (void)
{
  return this->mTransport->registerHandler ();
}


// ******************************************************************
WaitOnRead::WaitOnRead (IcmTransport* transport)
: WaitStrategy (transport)
{
}

WaitOnRead::~WaitOnRead (void)
{
}

// Wait on the read operation.
int
WaitOnRead::wait(TimeValue *maxWaitTime, int &replyReceived)
{
  TimeValue callTime = OS::gettimeofday ();

  int replyComplete = this->mTransport->handleInputI(0, maxWaitTime);

  if (replyComplete == -1) {
    if(maxWaitTime != 0 && errno == ETIME) {
      TimeValue curTime = OS::gettimeofday ();
//      ICC_DEBUG("Timeout after recv is %d seconds", (curTime - callTime).sec());
    }
    return -1;
  }

  assert(replyReceived);
  return 0;
}

// Handle the input. Delegate this job to Transport object.
int
WaitOnRead::handleInput (void)
{
  // Block to get the whole message.
  return this->mTransport->handleClientInput (1);
}

// No-op.
int
WaitOnRead::registerHandler (void)
{
  // async call use reactor, so this should not be no-op,
  // call registerHandler for async call
  return this->mTransport->registerHandler ();
//  return 0;
}

