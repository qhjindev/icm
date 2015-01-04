#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "msg/IcmMsg.h"
#include "NetworkI.h"

using namespace std;
using namespace demo;

class Subscriber {
public:
  int run(int, char*[]);
};

int
main(int argc, char* argv[]) {
  Subscriber app;
  return app.run(argc, argv);
}

int
Subscriber::run(int argc, char* argv[]) {
  Communicator* comm = Communicator::instance();
  if (comm->init (true) == -1)
    return -1;

  Reference ref (comm, Identity("TopicManager"), Endpoint("TCP", "127.0.0.1", 5555));
  IcmProxy::IcmMsg::TopicManager topicManager;
  topicManager.setReference (&ref);

  ObjectAdapter* adapter = comm->createObjectAdapterWithEndpoint("Subscriber", "127.0.0.1 8888");
  IcmProxy::Object* networkProxy = adapter->add(new NetworkI(), "NetworkTopic");

  ::IcmProxy::IcmMsg::Topic* topic = topicManager.retrieve("NetworkTopic");
  if(topic == 0)
	  topic = topicManager.create("NetworkTopic");
  if (topic == 0)
    return -1;
  topic->subscribe(networkProxy);

  comm->run();

  return 0;
}
