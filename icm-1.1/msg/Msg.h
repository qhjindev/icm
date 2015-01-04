#ifndef MSG_EVENT_H
#define MSG_EVENT_H

#include <string>
#include "icm/IcmStream.h"

const unsigned int MSG_HEADER_LEN = 5;
const unsigned int MESSAGE_SIZE_OFFSET = 0;
const unsigned int MESSAGE_TYPE_OFFSET = 4;

enum MsgType {
  Msg_Subscribe = 0,
  Msg_UnSubscribe = 1,
  Msg_Subscribe_OK = 2,
  Msg_UnSubscribe_OK = 3,
  Msg_Event = 4,
  Msg_Unknow = 5,
  Msg_Publish = 6,
  Msg_Publish_OK = 7,
  Msg_KeepAlive_Probe = 8,
  Msg_KeepAlive_ACK = 9,
  Msg_Publish_Buffer_Full = 10
};

class Msg {
public:
  Msg();
  ~Msg();

  unsigned char messageType;
  unsigned int messageSize;
  unsigned int currentOffset;

  InputStream is;
  std::string topic;
  std::string subscriberId;
  time_t creatTime;
  int keepAliveTryTimes;
  int keepAliveTimeSpan;
  unsigned char enableBuffering;
};

#endif //MSG_EVENT_H
