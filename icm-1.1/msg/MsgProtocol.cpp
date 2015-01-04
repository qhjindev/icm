
#include "icc/SocketStream.h"
#include "icc/Log.h"
#include "MsgProtocol.h"
#include "Msg.h"
#include <algorithm>
#include <iostream>

using namespace std;

MsgProtocol::MsgProtocol() {
}

MsgProtocol::~MsgProtocol() {
}

string MsgProtocol::toString(OutputStream& os) {
  string result;

  for (const MessageBlock *i = os.begin(); i != os.end(); i = i->cont()) {
    assert(i->length() != 0);
    result.append(i->rdPtr(), i->length());
  }

  return result;
}

OutputStream* MsgProtocol::fromString(const string& str) {
  OutputStream* os = new OutputStream((char*)str.c_str(), str.size());
  size_t size = str.size();
  MessageBlock* mb = (MessageBlock*)(os->current());
  mb->wrPtr(size);

  return os;
}

void MsgProtocol::check(const string& osStr) {
//  assert(osStr.c_str()[MESSAGE_TYPE_OFFSET] == Msg_Event);
  if((osStr.c_str()[MESSAGE_TYPE_OFFSET] != Msg_Event)) {
    return;
  }
  Msg msg;
  msg.is.grow(1024*64);
  std::copy(osStr.c_str(), osStr.c_str() + osStr.size(), msg.is.rdPtr());
  msg.is.skip_bytes(MSG_HEADER_LEN);
  string topic;
  msg.is.read_string(topic);
  assert(topic != "");
//  ICC_INFO("topic: %s", topic.c_str());
}

void MsgProtocol::check(OutputStream& os) {
  check(MsgProtocol::toString(os));
}

// -1: error, 0 : complete, 1: half read
int MsgProtocol::parse(SocketStream* stream, Msg* msg, bool block) {

  //LogDebug << "msg->currentOffset:" << msg->currentOffset << endl;

  block = true;

  if(msg->currentOffset == 0 && msg->is.grow(MSG_HEADER_LEN) == -1) {
    return -1;
  }

  int bytesRead = 0;
  int bytesToRead = 0;
  if(msg->currentOffset < MSG_HEADER_LEN) {
    bytesToRead = MSG_HEADER_LEN - msg->currentOffset;
    bytesRead = stream->recv (msg->is.rdPtr() + msg->currentOffset, bytesToRead);
    if(bytesRead == -1) {
      ICC_ERROR("MsgProtocol::parse header error: %s", OS::strerror(OS::map_errno(OS::last_error())));
      return -1;
    } else if (bytesRead == 0) {
      ICC_DEBUG("MsgProtocol::parse header EOF, peer closed");
      return -1;
    }

    msg->currentOffset += bytesRead;
    if(msg->currentOffset < MSG_HEADER_LEN) { // bytesRead < bytesToRead, half read
      return 1;
    }

    assert(msg->currentOffset == MSG_HEADER_LEN);
    msg->messageType =  msg->is.rdPtr()[MESSAGE_TYPE_OFFSET];
  //  ICC_ERROR("MsgProtocol::parse, size1: %d, size2: %d", *(int*)(buf -5), *(int*)(state->is->rdPtr()));
    msg->is.read_uint(msg->messageSize);
    //LogDebug << "msg size:" << msg->messageSize << endl;

    if (msg->is.grow(MSG_HEADER_LEN + msg->messageSize) == -1) {
      ICC_ERROR("MsgProtocol::parse, CDR::grow: %s", OS::strerror(OS::map_errno(OS::last_error())));
      return -1;
    }

    msg->is.skip_bytes(MSG_HEADER_LEN);
  }

  bytesToRead = msg->messageSize - (msg->currentOffset - MSG_HEADER_LEN);
  char* destToRead = msg->is.rdPtr() + (msg->currentOffset - MSG_HEADER_LEN);
  bytesRead = stream->recv(destToRead, bytesToRead);
  if(bytesRead == -1) {
    //ICC_ERROR("MsgProtocol::parse: %s, transport = %d,bytes read = %d,bytes to read = %d",
      //  OS::strerror(OS::map_errno(OS::last_error())), (int)stream->getHandle(), bytesRead, msg->messageSize);

    LogError << "MsgProtocol::parse conn:" << (int)stream->getHandle()
        << " bytesRead:" << bytesRead << " messageSize:" << msg->messageSize
        << " errno:" << errno << " err:" << perr << endl;
    if(errno == EWOULDBLOCK) {
      LogDebug << "no input available, going back to reading" << endl;
      return 0;
    } else {
      return -1;
    }
  } else if (bytesRead == 0) {
    ICC_ERROR("MsgProtocol::parse: %s, transport = %d,bytes read = %d,bytes to read = %d",
        "peer closed connection", (int)stream->getHandle(), bytesRead, msg->messageSize);

    return -1;
  }

  msg->currentOffset += bytesRead;
  if(bytesRead < bytesToRead) { // half read
    return 1;
  }

  assert(msg->currentOffset == MSG_HEADER_LEN + msg->messageSize);

  msg->is.read_string(msg->topic);
  if(msg->messageType == Msg_Subscribe || msg->messageType == Msg_UnSubscribe || msg->messageType == Msg_Subscribe_OK) {
    msg->is.read_string(msg->subscriberId);
    if(msg->messageType == Msg_Subscribe_OK) {
      msg->is.read_int(msg->keepAliveTryTimes);
      msg->is.read_int(msg->keepAliveTimeSpan);
    }
  } else if(msg->messageType == Msg_Event) {
    assert(msg->topic != "");
  } else if(msg->messageType == Msg_Publish_OK) {
    msg->is.read_int(msg->keepAliveTryTimes);
    msg->is.read_int(msg->keepAliveTimeSpan);
    msg->is.read_boolean(msg->enableBuffering);
  }

  return 0;
}

int MsgProtocol::start(unsigned char type, OutputStream* os) {
  unsigned int size = 0;
  os->write_uint(size);
  os->write_octet(type);

  return 0;
}

int MsgProtocol::send(SocketStream* stream, OutputStream* os) {
//  ICC_INFO("MsgProtocol::send called, %d bytes to send", os->total_length());

  check(*os);

  // calculate and write length
  char* buf = (char*) os->buffer ();
  size_t totalLen = os->total_length ();
  size_t headerLen = MSG_HEADER_LEN;
  ACE_CDR::UInt bodyLen = (ACE_CDR::UInt)(totalLen - headerLen);
  if(os->do_byte_swap()) {
    std::reverse_copy((char*)&bodyLen, (char*)&bodyLen + 4, buf + MESSAGE_SIZE_OFFSET);
  } else {
    std::copy((char*)&bodyLen, (char*)&bodyLen + 4, buf + MESSAGE_SIZE_OFFSET);
  }

  for (const MessageBlock *i = os->begin(); i != os->end(); i = i->cont()) {
    assert(i->length() != 0);

    ssize_t n = stream->send(i->rdPtr(), i->length());

    if (n > 0) {
//      ICC_ERROR("IcmProtocol::sendMessage succeeded, %d bytes to send, %d bytes sent", i->length(), n);
//      ICC_INFO("IcmProtocol::sendMessage succeeded, %d bytes sent", n);
    } else if(n == -1) {
      LogError << "IcmProtocol::send conn:" << (int)stream->getHandle() << " errno:" << errno << " fault:" << perr << endl;
      /*
      if(errno == EWOULDBLOCK) {
        LogDebug << "EWOULDBLOCK " << endl;
        return 0;
      }*/
      return -1;
    } else if(n == 0) { // EOF
      ICC_ERROR("IcmProtocol::sendMessage() EOF, bytes to send: %d, closing conn %d", i->length(), stream->getHandle());
      return -1;
    }
  }

  return 0;
}


