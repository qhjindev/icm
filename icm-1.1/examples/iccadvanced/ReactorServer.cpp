/*
 * example usage of server side io handler, acceptor and reactor
 *
 * This example demonstrates:
 *
 * 1) Use Acceptor to listen, and make new io handler when connection request accepted
 *      
 * 2) Use reactor event loop to process socket data flow
 *
 * 3) Protocol: Client send one byte request, when Server recved, send back one byte ack
 *
 */

#include "icc/SelectReactor.h"
#include "icc/Reactor.h"
#include "icc/SvcHandler.h"
#include "icc/Acceptor.h"
#include "os/OS_NS_string.h"
#include <string>
#include <iostream>

using namespace std;
const short ListenPort = 23456;

class ServerHandler : public SvcHandler
{
public:
  int open (void * ) {
    return SvcHandler::open(0);
  }

  int close (void *) {
    return 0;
  }

  virtual int handleInput (ACE_HANDLE fd ) {
    char request = 0;
    if(stream.recv(&request, 1) == 1) {
      cout << "recved request: " << (int)request << endl;
      if(stream.send(&request, 1) < 0) {
        return -1;
      }
    } else {
      cout << "read request failed, errno: " << errno << endl;
      return -1;
    }

    return 0;
  }

  virtual int handleClose (ACE_HANDLE h,ReactorMask mask) {
    return SvcHandler::handleClose(h, mask);
  }
};

int main() {
  SelectReactor selectReactor;
  Reactor reactor(&selectReactor);
  Acceptor<ServerHandler> acceptor;
  InetAddr listenAddr(ListenPort);

  // when connection established, make new io handler and register it to reactor
  if (acceptor.open(listenAddr, &reactor) == -1) {
    cout << "acceptor failed, errno: " << errno << endl;
    return -1;
  }

  reactor.runReactorEventLoop();

  return 0;
}
