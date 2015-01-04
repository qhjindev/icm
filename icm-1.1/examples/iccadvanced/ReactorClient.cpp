/*
 * example usage of timeout handler, io handler, connector and reactor
 *
 * This example demonstrates:
 *
 * 1) How to create timeout handler and register to reactor(scheduleTimer)
 *      
 * 2) How to create io handler and connect to server
 * 
 * 3) Use reactor event loop to process socket data flow
 *
 * 4) Protocol: Client send one byte request, when Server recved, send back one byte ack
 *
 */

#include "icc/SelectReactor.h"
#include "icc/Reactor.h"
#include "icc/SvcHandler.h"
#include "icc/Connector.h"
#include "os/OS_NS_string.h"
#include <string>
#include <iostream>

using namespace std;
const short ServerPort = 23456;
const char* ServerIp = "localhost";
const int TimeSpec = 5;

class ClientHandler: public SvcHandler {
public:
  class Client_Timeout_Handler: public EventHandler {
  public:
    // called by reactor thread when timer expired
    virtual int handleTimeout(const TimeValue &currentTime, const void *act) {
      ClientHandler* clientHandler = (ClientHandler*)act;
      return clientHandler->sendRequest();
    }
  };

  // called by Connector.connect() after connection successfully established
  virtual int open(void *) {
    Reactor* reactor = this->getReactor();
    Client_Timeout_Handler* timeOutHandler = new Client_Timeout_Handler;
    TimeValue tv1(0);
    TimeValue tv2(TimeSpec);
    if(reactor->scheduleTimer(timeOutHandler, this, tv1, tv2) == -1) {
      cerr << "scheduleTimer() failed!! \n";
      return -1;
    }

    return SvcHandler::open(0);
  }

  virtual int close (void *) {
    return 0;
  }

  // called by reactor thread when socket received data
  virtual int handleInput(ACE_HANDLE fd) {
    char ack = 0;
    if(stream.recv(&ack, 1) <= 0) {
      cout << "recv ack failed" << endl;
      return -1;
    } else {
      cout << "recved ack: " << (int)ack << endl;
    }

    return 0;
  }

  virtual int handleClose(ACE_HANDLE h, ReactorMask mask) {
    return SvcHandler::handleClose(h, mask);
  }

  virtual int sendRequest() {
    static char request = 0;
    request++;
    if(stream.send(&request, 1) < 0) {
      return -1;
    }

    return 0;
  }
};

int main() {
  Connector<ClientHandler> connector(Reactor::instance());
  InetAddr connAddr(ServerPort, ServerIp);
  ClientHandler* handler = 0;

  // connect() will make a new ClientHandler and assign its address to handler when success
  if (connector.connect(handler, connAddr) == -1) {
    cout << "SubscribeClient::init, connect error: " << OS::strerror(OS::map_errno(OS::last_error())) << endl;
    return -1;
  }

  Reactor* reactor = handler->getReactor();
  reactor->runReactorEventLoop();

  return 0;
}
