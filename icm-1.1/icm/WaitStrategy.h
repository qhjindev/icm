#ifndef ICM_WAIT_STRATEGY_H
#define ICM_WAIT_STRATEGY_H

#include "icc/TimeValue.h"

class Communicator;
class IcmTransport;
class TransportMuxStrategy;

class WaitStrategy
{
  // = TITLE
  //
  //    Strategy for waiting for the reply.
  //
  // = DESCRIPTION
  //

public:
  // Constructor.
  WaitStrategy (IcmTransport* transport);

  // Destructor.
  virtual ~WaitStrategy (void);

  // The user is going to send a request, prepare any internal
  // variables because the reply may arrive *before* the user calls
  // wait.
//  virtual int sendingRequest(Communicator* comm, int twoWay);

  // Base class virtual method. Wait till the <replyReceived> flag is
  // true or the time expires.
  virtual int wait (TimeValue* maxWaitTime, int& replyReceived) = 0;

  // Handle the input.
  virtual int handleInput (void) = 0;

  // Register the handler with the Reactor if it makes sense for the
  // strategy.
  virtual int registerHandler (void) = 0;

  bool getConnClosed() {
    return connClosed;
  }

  void setConnClosed(bool closed) {
    connClosed = closed;
  }

protected:
  IcmTransport* mTransport;

  bool connClosed;
};

class WaitOnReactor : public WaitStrategy
{
  // = TITLE
  //
  //    Wait on the Reactor. Happens in s Single Threaded client
  //    environment.
  //
  // = DESCRIPTION
  //
public:
  // Constructor.
  WaitOnReactor (IcmTransport* transport);

  // Destructor.
  virtual ~WaitOnReactor (void);

  virtual int wait (TimeValue* maxWaitTime, int& replyReceived);

  virtual int handleInput (void);

  virtual int registerHandler (void);

};

class WaitOnRead : public WaitStrategy
{
  // = TITLE
  //
  // = DESCRIPTION
  //   Simply block on read() to wait for the reply.
  //

public:
  // Constructor.
  WaitOnRead (IcmTransport* transport);

  // Destructor.
  virtual ~WaitOnRead (void);

  virtual int wait (TimeValue* maxWaitTime, int& replyReceived);

  virtual int handleInput (void);

  virtual int registerHandler (void);

};

#endif //ICM_WAIT_STRATEGY_H
