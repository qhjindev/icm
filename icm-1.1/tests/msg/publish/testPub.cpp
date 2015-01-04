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
#include <sstream>

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


//  string str = MsgProtocol::toString(os1);
//  OutputStream* os = MsgProtocol::fromString(str);
//  string str1 = MsgProtocol::toString(*os);
//  ICC_ERROR("str: %s, size: %d", str.c_str(), str.size());
//  ICC_ERROR("str1: %s, size: %d", str1.c_str(), str1.size());
//  assert(str == str1);
//  assert(false);


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
    //bool result = pub->init("172.30.1.58");
    bool result = pub->init("127.0.0.1");
    //bool result = pub->init("172.16.10.177");
//    assert(result);
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
TimeValue tv(0, 10);
OS::sleep(tv);
//sleep(2);
      pub->pubRequest3(topic);
//      if(++i == 100)
//        break;
    }
    return 0;
  }
private:
  TestPub* pub;
  string topic;
};

const int Publishers = 1;
int main() {
  //Log::instance()->open();
  Log::instance()->open("", LM_ERROR);

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigaction( SIGPIPE, &sa, 0 );

  TestPub tp;
  //assert(tp.init("172.30.1.58"));
//  assert(tp.init("127.0.0.1"));
//
  //tp.pubRequest3("testtopic");
//  tp.pubRequest3("node1");
/*
  sleep(1000);
return 0;
*/
//  tp.pubRequest4();
//
//  sleep(1);

  for (int i = 0; i < Publishers; ++i) {
    PubThread* pt = new PubThread;
    stringstream ss;
    ss << "cpptopic" << i + 1;
    string topic = ss.str();
    pt->setTopic(topic);
    pt->activate();
  }

  ThreadManager::instance()->wait();
  return 0;
}

