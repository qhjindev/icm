
#include <iostream>
#include "icm/Communicator.h"
#include "msg/IcmMsg.h"
#include "Network.h"

using namespace std;
using namespace IcmMsg;

class Publisher {
public:

  int run(int, char*[]);
};

int main(int argc, char* argv[]) {
  Publisher app;
  int rc = app.run(argc, argv);
  return rc;
}

int Publisher::run(int argc, char* argv[]) {
  Communicator* comm = Communicator::instance();
  if (comm->init (true) == -1)
    return -1;

  Reference ref (comm, Identity("TopicManager"), Endpoint("TCP", "127.0.0.1", 5555));
  IcmProxy::IcmMsg::TopicManager topicManager;
  topicManager.setReference (&ref);

  ::IcmProxy::IcmMsg::Topic* topic = topicManager.retrieve("NetworkTopic");
  if(topic == 0)
	  topic = topicManager.create("NetworkTopic");
  if(topic == 0) {
	  cout << "err create topic " << endl;
	  return -1;
  }

  ::IcmProxy::Object* pubObj = topic->getPublisher();
  if(pubObj == 0) {
	  cout << "err get publisher " << endl;
	  return -1;
  }
  IcmProxy::demo::Network network;
  network.setReference(pubObj->getReference());

  cout << "publishing network events:" << endl;
  demo::NetEvent event;

  //add start
  ostringstream oss;
  for(int j=0; j < 1000; j++) {
    oss << "abcdefghijklmnopqrstuvwxyz";
  }
  string ibigstr = oss.str();
  //add end

  event.ip = "172.16.10.190";
  //event.ip = ibigstr;

  event.port = 6789;
  for(int i=0; i< 10; i++) {
    ostringstream oss;
    oss << "evt:" << i;
    event.event = oss.str();
    network.reportEvent(event);
  }

  return 0;
}
