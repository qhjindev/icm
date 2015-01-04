
#ifndef MSGPROTOCOL_H_
#define MSGPROTOCOL_H_

class SocketStream;
class MessageState;
class OutputStream;

class Msg;

class MsgProtocol {
public:
  MsgProtocol();
  virtual ~MsgProtocol();

  // -1: error, 0 : complete, 1: half read
  static int parse(SocketStream* stream, Msg* msg, bool block = false);
  static int start(unsigned char type, OutputStream* os);
  static int send(SocketStream* stream, OutputStream* os);

  static std::string toString(OutputStream& os);
  static OutputStream* fromString(const std::string& str);
  static void check(OutputStream& os);
  static void check(const std::string& os);
};

#endif /* MSGPROTOCOL_H_ */
