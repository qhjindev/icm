#ifndef ICM_PROTOCOL_H
#define ICM_PROTOCOL_H

#include "icc/SvcHandler.h"
#include "icm/IcmStream.h"
#include "icm/Reference.h"

class IcmTransport;
class BasicStream;
class Communicator;
class TimeValue;

class IcmMessageState
{

public:

  IcmMessageState (Communicator* comm = 0);

  void reset (int resetContents = 1);

  int headerReceived (void) const;

  int isComplete (void);

  ACE_CDR::Octet magic[4];
  ACE_CDR::Octet byteOrder;
  ACE_CDR::Octet moreFragment;
  ACE_CDR::Octet messageType;
  ACE_CDR::UInt messageSize;

  ACE_CDR::UInt currentOffset;

  InputStream stream;
};

const int ICM_HEADER_LEN = 5;
const int ICM_MESSAGE_SIZE_OFFSET = 0;
const int ICM_MESSAGE_TYPE_OFFSET = 4;

enum ICM_ReplyStatusType
{
  ICM_NO_EXCEPTION,
  // Request completed successfully

  ICM_USER_EXCEPTION,
  // Request terminated with user exception

  ICM_SYSTEM_EXCEPTION,
  // Request terminated with system exception

  ICM_LOCATION_FORWARD
  // Reply is a location forward type
};

class IcmProtocol
{
public:
  enum MessageType
  {
    Request = 0,
    Reply = 1,
    CancelRequest = 2,
    Error
  };

  static bool startMessage(IcmProtocol::MessageType t, OutputStream& msg);

  static bool writeRequestHeader (ACE_CDR::UInt requestId, 
                                  ACE_CDR::Octet responseFlags, 
                                  const Identity& ident, 
                                  const char* opname,
                                  OutputStream& msg);

  static bool writeReplyHeader (ACE_CDR::UInt requestId,
                                ACE_CDR::UInt replyStatus,
                                OutputStream& output);

  //static bool generateReplyHeader (OutputStream& stream);

  static int sendMessage (IcmTransport* transport, 
                          OutputStream& stream, 
                          Communicator* comm, 
                          TimeValue* maxWaitTime = 0,
                          Reference* ref = 0,
                          int twoWay = 1);

  //static int sendError (IcmTransport* transport);

  static ssize_t readBuffer(IcmTransport* transport, 
                            char* buf, 
                            size_t len, 
                            TimeValue* maxWaitTime);

  static int readHeader (IcmTransport* transport, 
                         Communicator* comm,
                         IcmMessageState& state,
                         unsigned long& headerSize,
                         InputStream& input,
                         TimeValue* maxWaitTime);

  static int handleInput (IcmTransport* transport,
                          Communicator* comm,
                          IcmMessageState& state,
                          TimeValue* maxWaitTime = 0);

  static int parseReply (IcmTransport* transport,
                         Communicator* comm,
                         IcmMessageState& state,
                         ACE_CDR::UInt& requestId,
                         ACE_CDR::UInt& replyStatus);

  static int processServerMessage (IcmTransport* transport,
                                   Communicator* comm,
                                   InputStream& input,
                                   unsigned char messageType);

  static int processServerRequest (IcmTransport* transport,
                                   Communicator* comm,
                                   InputStream& input,
                                   OutputStream& output);
private:

  static int parseHeader (InputStream& input, IcmMessageState& state);

};

#endif //ICM_PROTOCOL_H
