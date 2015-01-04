#include "icc/ThreadManager.h"
//#include "PublisherProxy.h"
//#include "SubscriberProxy.h"
#include "MsgBrokerI.h"
#include <map>
#include <list>
#include <string>

int
main(int argc, char* argv[]) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  MessageBroker messageBroker;
  messageBroker.activate();

  ThreadManager::instance()->wait();
  return 0;
}

