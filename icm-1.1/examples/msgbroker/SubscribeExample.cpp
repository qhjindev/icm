/*
 * example usage of SubscribeClient
 *
 * This example demonstrates:
 *
 * 1) How to connect to icmmsg broker
 * 
 * 2) How to subscribe topic, read and process recved messages 
 *
 */

#include "msg/SubscribeClient.h"
#include <string>
#include <iostream>

using namespace std;

class MySub: public SubscribeClient {
public:
  void handleEvent(InputStream& is) {
    // recved message of subscribed topic,the input argument InputStream hold the published message,
    // then read and process all data
    string type;
    is.read_string(type);
    if(type == "testType") {
      // is.read remaining content
      cout << "recved message of type: testType" << endl;
      // .. do sth
    } // else if(type == ....
  }
};

int main() {
  // unix network program must ignore or catch the SIGPIPE signal,
  // or a write to socket may generate SIGPIPE before return -1, cause the process to terminate
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  MySub client;
  // connect to icmmmsg, change "172.0.0.1" to the real ip if icmmsg not on localhost 
  if(client.init("127.0.0.1") == false) { 
    cerr << "connect to icmmsg failed" << endl;
    return -1;
  }

  client.subscribeTopic("testTopic");

  // run() will enter a cycle to wait and process subscribed messages(Mysub::handleEvent(InputStream&) will be called)
  // so usually we create a new thread to call this 
  client.run();

  return 0;
}
