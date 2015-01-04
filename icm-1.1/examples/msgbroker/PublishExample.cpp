/*
 * example usage of PublishClient
 *
 * This example demonstrates:
 *
 * 1) How to connect to icmmsg broker
 * 
 * 2) How to write data into icm OutputStream, and publish to icmmsg using PublishClient
 *
 */

#include "msg/PublishClient.h"
#include <string>
#include <iostream>

using namespace std;

int main() {
  // unix network program must ignore or catch the SIGPIPE signal,
  // or a write to socket may generate SIGPIPE before return -1, cause the process to terminate
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  PublishClient client;
  // connect to icmmmsg, change "172.0.0.1" to the real ip if icmmsg not on localhost 
  if(client.init("127.0.0.1") == false) { 
    cerr << "connect to icmmsg failed" << endl;
    return -1;
  }

  OutputStream os;
  string topic("testTopic");
  string type("testType");
  MsgProtocol::start(Msg_Event, &os);
  os.write_string(topic);
  os.write_string(type);
  // os.write... (what ever content)

  // publish all data in os to icmmsg
  client.publish(os); 

  return 0;
}
