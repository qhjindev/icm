/*
 * testPub.cpp
 *
 *  Created on: 2011-10-10
 *      Author: robert
 */

#include "msg/PublishClient.h"
#include <string>
#include "javatopic.h"
#include <icc/ThreadManager.h>
#include <iostream>
#include "TvmEvent.h"

using namespace std;

class TestPub: public PublishClient {
public:
  TestPub();
  virtual ~TestPub() {
  }

  void pubRequest3(const string& topic);
  void pubRequest4(const string& topic);
};

//static string topic = "javatopic";
//static string topic = "cpptopic1";
static string type1 = "Request3";
static string type2 = "Request4";

TestPub::TestPub() {
}

const int msglen = 516;
const char* getTestString() {
  static char str[msglen];
  memset(str, 's', msglen);
  str[msglen - 1] = '\0';
  return str;
}

void TestPub::pubRequest3(const string& topic) {
  OutputStream os1;
  MsgProtocol::start(Msg_Event, &os1);
  os1.write_string(topic);
  os1.write_string(type1);

  JavaTopic::Request3 rq3;
  rq3.cpu = 33;
  //rq3.instanceId = "rq3";
  static const char* teststr = getTestString();
  rq3.instanceId = teststr;
  rq3.mem = 333;
  rq3.nodeId = "node_rq3";
  rq3.seq = 1;
  rq3.__write(&os1);
  ICC_DEBUG("pubRequest3 Publishing msg length: %d", os1.total_length());
  publish(os1);
}

void TestPub::pubRequest4(const string& topic) {
  OutputStream os2;
  MsgProtocol::start(Msg_Event, &os2);
  os2.write_string(topic);
  os2.write_string(type2);

  JavaTopic::Request4 rq4;
  rq4.state = 4;
  rq4.instanceId = "rq4";
  rq4.result = "rq4 ok";
  rq4.nodeId = "node_rq4";
  rq4.seq = 2;
  rq4.__write(&os2);
  publish(os2);
}

class PubThread: public Thread {
public:
  PubThread() :
      pub(new TestPub()) {
    bool result = pub->init("172.16.10.23", 10003);
    //bool result = pub->init("127.0.0.1");
    //bool result = pub->init("172.30.2.70");
    //assert(result);
  }
  ~PubThread() {
    if (pub)
      delete pub;
  }

  void setTopic(const string& tp) {
    topic = tp;
  }
  virtual int svc(void) {
    assert(topic != "");
    int i = 0;
    while (true) {
      pub->pubRequest3(topic);
      if(++i == 100)
        break;
    }
    return 0;
  }
private:
  TestPub* pub;
  string topic;
};

std::string int2string(int i) {
  stringstream ss;
  ss << i;
  return ss.str();
}

//const int Publishers = 10;
int main() {
  Log::instance()->open();
//  Log::instance()->open("", LM_ERROR);

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  TvmEvt1::RunInstanceRequest event;
  OutputStream os;
  MsgProtocol::start(Msg_Event, &os);
  os.write_string("node1"); //write topic
  os.write_string("RunInstanceRequest");
  event.__write(&os);

  TestPub client;
  bool result = client.init("172.16.10.23", 10003);
  //bool result = client.init("172.30.2.70");
  //assert(result);

  while (true) {
    static int i = 0;

    TvmEvt1::RunInstanceRequest event;
    OutputStream os1;
    MsgProtocol::start(Msg_Event, &os1);
    os1.write_string("node1"); //write topic
    string type = int2string(++i);
    ICC_DEBUG("publishing, type: %s", type.c_str());
    os1.write_string(type);

    /////////////////////
    JavaTopic::Request3 rq3;
    rq3.cpu = 33;
    //rq3.instanceId = "rq3";
    static const char* teststr = getTestString();
    rq3.instanceId = teststr;
    rq3.mem = 333;
    rq3.nodeId = "node_rq3";
    rq3.seq = 1;
    rq3.__write(&os1);
    //////////////////////


// replace this
//    event.__write(&os1);

    client.publish(os1);
    sleep(5);
  }

  return 0;
}

