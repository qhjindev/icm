#include "IcmEvent.h"
#include "msg/PublishClient.h"
#include "msg/Msg.h"
#include "msg/MsgProtocol.h"

int main(int argc, char* argv[]) {

  PublishClient client;
  if(!client.init("127.0.0.1"))
    return -1;

  std::string topic1 = "topic1";

  Icm::Event1 event;
  //::std::string topic;
  event.topic = "topic1";
  event.item1 = 123;
  event.item2 = "item2";
  event.dev.d = "d";
  event.dev.a = "a";

  Icm::Event2 event2;
  //::std::string topic;
  event2.item1 = "123";
  event2.item2 = "item2";

  ::Icm::Port port;
  port.i = 12;
  port.str = "123.456.789";
  event.conn.push_back(port);

  for (int i = 0; i < 10; i++) {
    OutputStream os;
    MsgProtocol::start(Msg_Event, &os);
    os.write_string("topic1");
    os.write_string("type1");
    event.__write(&os);
    client.publish(os);
  }

  for (int j = 0; j < 10; j++) {
    OutputStream os;
    MsgProtocol::start(Msg_Event, &os);
    os.write_string("topic1");
    os.write_string("type2");
    event2.__write(&os);
    client.publish(os);
  }

  return 0;
}


