
//  Usage:
//    # Peer1
//    % ./dgram 10002 remotehost 10003 peer1
//
//    # Peer1
//    % ./dgram 10003 remotehost 10002 peer2

#include "icc/EventHandler.h"
#include "icc/Reactor.h"
#include "icc/SocketDgram.h"
#include "icc/InetAddr.h"
#include "icc/Log.h"

static u_short port1;

class DgramEndpoint : public EventHandler {
public:
  DgramEndpoint(const InetAddr& localaddr);

  virtual ACE_HANDLE getHandle(void) const;

  virtual int handleInput (ACE_HANDLE fd);

  virtual int handleClose (ACE_HANDLE, ReactorMask);

  int send(const char* buf, size_t len, const InetAddr&);

private:
  SocketDgram endpoint;
};

int
DgramEndpoint::send(const char* buf, size_t len, const InetAddr& addr) {
  return this->endpoint.send(buf, len, addr);
}

DgramEndpoint::DgramEndpoint(const InetAddr& localaddr)
  : endpoint(localaddr) {

}

ACE_HANDLE
DgramEndpoint::getHandle(void) const {
  return this->endpoint.getHandle();
}

int
DgramEndpoint::handleClose(ACE_HANDLE handle, ReactorMask) {
  this->endpoint.close();
  delete this;
  return 0;
}

int
DgramEndpoint::handleInput(ACE_HANDLE) {
  char buf[BUFSIZ];
  InetAddr fromAddr;

  ssize_t n = this->endpoint.recv(buf, sizeof buf, fromAddr);
  if(n == -1) {
    LogError << "handleInput:" << perr << endl;
  } else {
    LogDebug << "buf of size " << n << " = " << buf << endl;
  }

  return 0;
}

static int
runTest(u_short localport,
        const char* remotehost,
        u_short remoteport,
        const char* peer) {
  InetAddr remoteAddr(remoteport, remotehost);
  InetAddr localAddr(localport);

  DgramEndpoint* endpoint = new DgramEndpoint(localAddr);

  if(Reactor::instance()->registerHandler(endpoint, EventHandler::READ_MASK) == -1) {
    LogError << "err:Reactor::instance()->registerHandler" << endl;
    return -1;
  }

  char buf[BUFSIZ];
  OS::strcpy(buf, "Data to transmit");
  size_t len = OS::strlen(buf);

  if(OS::strncmp(peer, "peer1", 5) == 0) {
    LogDebug << "sending data" << endl;

    for(size_t i=0; i<20; i++) {
      endpoint->send(buf, len, remoteAddr);
      OS::sleep(1);
    }
  }

  for(int i = 0; i < 40; i++) {
    TimeValue tv(60, 0);

    if(Reactor::instance()->handleEvents(&tv) <= 0) {
      LogError << "handleEvent:" << perr << endl;
      return -1;
    }

    LogDebug << "return from handleEvents" << endl;

    endpoint->send(buf, len, remoteAddr);
  }

  if(Reactor::instance()->removeHandler(endpoint, EventHandler::READ_MASK) == -1) {
    LogError << "removeHandler:" << perr << endl;
    return -1;
  }

  LogDebug << "exiting" << endl;

  return 0;
}

int main(int argc, char* argv[]) {
  if(argc < 5) {
    LogError << "cmdline:./dgram localport remotehost remoteport peer1/2" << endl;
    return -1;
  }

  port1 = OS::atoi(argv[1]);
  const char* remotehost = argv[2];
  const u_short port2 = OS::atoi(argv[3]);

  runTest(port1, remotehost, port2, argv[4]);

  return 0;
}
