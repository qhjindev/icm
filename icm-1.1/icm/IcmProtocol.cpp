#include "icc/Log.h"
#include "icc/MessageBlock.h"
#include "icm/IcmProtocol.h"
#include "icm/Pluggable.h"
#include "icm/ServerRequest.h"
#include "icm/ObjectAdapter.h"
#include "icm/Communicator.h"
#include "icm/Object.h"
#include <algorithm>

IcmMessageState::IcmMessageState(Communicator *comm) :
    messageType(IcmProtocol::Error), messageSize(0), currentOffset(0), stream(ACE_CDR::DEFAULT_BUFSIZE) {
}

int IcmMessageState::isComplete(void) {
  if (this->messageSize != this->currentOffset)
    return 0;

  return 1;
}

void IcmMessageState::reset(int resetContents) {
  this->messageSize = 0;
  this->currentOffset = 0;
//  if (resetContents)
//    this->stream.reset ();
}

int IcmMessageState::headerReceived(void) const {
  return this->messageSize != 0;
}

bool IcmProtocol::startMessage(IcmProtocol::MessageType t, OutputStream& msg) {
  ACE_CDR::UInt size = 0;
  msg.write_uint(size);

  msg.write_octet((ACE_CDR::Octet) t);

  return true;
}

bool IcmProtocol::writeRequestHeader(ACE_CDR::UInt requestId, ACE_CDR::Octet responseFlags, const Identity& ident,
    const char* opname, OutputStream& msg) {
  msg << requestId;
  msg << OutputStream::from_octet(responseFlags);
  msg << ident.name;
  msg << ident.category;
  msg << std::string(opname);

  return true;
}

bool IcmProtocol::writeReplyHeader(ACE_CDR::UInt requestId, ACE_CDR::UInt replyStatus, OutputStream& output) {
  output.write_uint(requestId);
  output.write_uint(replyStatus);
  return true;
}

int IcmProtocol::sendMessage(IcmTransport* transport, OutputStream& stream, Communicator* comm, TimeValue* maxWaitTime,
    Reference* ref, int twoWay) {
  if(transport == 0)
    return -1;

  char* buf = (char*) stream.buffer();

  size_t totalLen = stream.total_length();
  size_t headerLen = ICM_HEADER_LEN;

  ACE_CDR::UInt bodyLen = (ACE_CDR::UInt) (totalLen - headerLen);

  if(stream.do_byte_swap()) {
    std::reverse_copy((char*)&bodyLen, (char*)&bodyLen + 4, buf + ICM_MESSAGE_SIZE_OFFSET);
  } else {
    std::copy((char*)&bodyLen, (char*)&bodyLen + 4, buf + ICM_MESSAGE_SIZE_OFFSET);
  }

  for (const MessageBlock *i = stream.begin(); i != stream.end(); i = i->cont()) {
    ssize_t toSend = i->length();
    if(toSend == 0) {
      continue;
    }

    ssize_t n = transport->send(i, maxWaitTime);
    if (n == -1) {
      ICC_ERROR("IcmProtocol::sendMessage closing conn: %d after fault: %s, bytes to send: %d",
          (int) transport->handle(), OS::strerror(OS::map_errno(OS::last_error())), i->length());
      return -1;
    }

    // EOF
    if (n == 0) {
      ICC_DEBUG("IcmProtocol::sendMessage(), send() returns 0, handle: %d", transport->handle());
      return -1;
    }
  }

  return 1;
}

ssize_t IcmProtocol::readBuffer(IcmTransport* transport, char *buf, size_t len, TimeValue* maxWaitTime) {
  ssize_t bytesRead = transport->recv(buf, len, maxWaitTime);

  if (bytesRead <= 0) {
    ICC_ERROR("IcmProtocol::readBuffer error: %s, transport = %d, bytes = %d, len = %d",
        OS::strerror(OS::map_errno(OS::last_error())), (int) transport->handle(), bytesRead, len);
//    LogMsg << "IcmTransport::recv: " << perr << ", transport = " << (int) transport->handle() << ",bytes = "
//        << bytesRead << ",len = " << len << endl;
  }

//  if ((bytesRead == -1) && errno == ECONNRESET) {
//    // We got a connection reset (TCP RSET) from the other side,
//    // i.e., they didn't initiate a proper shutdown.
//    //
//    // Make it look like things are OK to the upper layer.
//    bytesRead = 0;
//    errno = 0;
//  }

  return bytesRead;
}

int IcmProtocol::parseHeader(InputStream& input, IcmMessageState& state) {
  char* buf = input.rdPtr();

  state.messageType = buf[ICM_MESSAGE_TYPE_OFFSET];
  state.byteOrder = 1;

  input.reset_byte_order(state.byteOrder);
  input.read_uint(state.messageSize);

  return 0;
}

int IcmProtocol::readHeader(IcmTransport* transport, Communicator* comm, IcmMessageState& state,
    unsigned long& headerSize, InputStream& input, TimeValue* maxWaitTime) {
  headerSize = ICM_HEADER_LEN;

  if (input.grow(headerSize) == -1)
    return -1;

  char* buf = input.rdPtr();
  ssize_t n;

  for (int t = headerSize; t != 0; t -= n) {
    n = transport->recv((char*) buf, t, maxWaitTime);
    if (n == -1) {
      ICC_ERROR("IcmProtocol::readHeader error: %s, %d", OS::strerror(OS::map_errno(OS::last_error())),errno);
      return -1;
    } else if (n == 0) {
//      ICC_DEBUG("IcmProtocol::readHeader, connection closed");
      return -1;
    }
    buf += n;
  }

  if (IcmProtocol::parseHeader(input, state) == -1) {
    //senderr, no use
    return -1;
  }

  return headerSize;
}

int IcmProtocol::handleInput(IcmTransport* transport, Communicator* comm, IcmMessageState& state,
    TimeValue* maxWaitTime) {
  if (state.headerReceived() == 0) {
    unsigned long headerSize;
    if (IcmProtocol::readHeader(transport, comm, state, headerSize, state.stream, maxWaitTime) == -1) {
      return -1;
    }

    if (state.stream.grow(headerSize + state.messageSize) == -1) {
      LogMsg << "IcmProtocol::handleInput, CDR::grow: " << perr << endl;
      return -1;
    }

    state.stream.skip_bytes(headerSize);
  }

  size_t missingData = state.messageSize - state.currentOffset;
  ssize_t n = IcmProtocol::readBuffer(transport, (char*) (state.stream.rdPtr() + state.currentOffset), missingData, maxWaitTime);
  if (n == -1 && errno == EAGAIN) {
//    TimeValue tv(0,100 * 1000);
    OS::sleep(1);
    n = IcmProtocol::readBuffer(transport, (char*) (state.stream.rdPtr() + state.currentOffset), missingData, maxWaitTime);
  }
  if (n == -1) {
    LogMsg << "IcmProtocol::handleInput, readBuffer[1]" << perr << endl;
    return -1;
  } else if (n == 0) {
    LogMsg << "IcmProtocol::handleInput, readBuffer[1]" << perr << endl;
    return -1;
  }

  state.currentOffset += n;

  if (state.currentOffset == state.messageSize) {
    /////////// dump msg
  }

  return state.isComplete();
}

int IcmProtocol::parseReply(IcmTransport* transport, Communicator* comm, IcmMessageState& state,
    ACE_CDR::UInt& requestId, ACE_CDR::UInt& replyStatus) {
  switch (state.messageType) {
  case IcmProtocol::Request: {
//    ICC_DEBUG("IcmProtocol::parseReply: request");
    return -1;
  }

  case IcmProtocol::Reply:
    break;
  }

  if (!state.stream.read_uint(requestId)) {
//    ICC_DEBUG("IcmProtocol::parseReply, extracting requestId");
    return -1;
  }

  if (!state.stream.read_uint(replyStatus)) {
//    ICC_DEBUG("IcmProtocol::parseReply, extracting replyStatus");
    return -1;
  }

  return 0;
}

int IcmProtocol::processServerMessage(IcmTransport* transport, Communicator* comm, InputStream& input,
    unsigned char messageType) {
  char repbuf[ACE_CDR::DEFAULT_BUFSIZE];
  OutputStream output(repbuf, sizeof repbuf, 1);

  switch (messageType) {
  case IcmProtocol::Request:
    return IcmProtocol::processServerRequest(transport, comm, input, output);
  case IcmProtocol::Reply:
  default:
    ICC_ERROR("Illegal message received by server");
    break;
    //return IcmProtocol::sendError (transport);
  }

  return 0;
}

int IcmProtocol::processServerRequest(IcmTransport* transport, Communicator* comm, InputStream& input,
    OutputStream& output) {
  int responseRequired = 0;
  int parseError;

  ServerRequest request(input, output, transport, comm, parseError);

  responseRequired = request.responseExpected();

  if (parseError != 0)
    return -ICM_MARSHAL;

  Identity& identity = request.identity();

  ObjectAdapter* oa = transport->adapter();
  if (oa == 0)
    return -1;

  Object* object = oa->find(request, identity);
  if (object == 0) {
    ICC_ERROR("IcmProtocol::processServerRequest error, no match request identity: %s", identity.name.c_str());
    return -1;
  }

  // Do this before the reply is sent.
  DispatchStatus ds = object->__dispatch(request);
  if (ds == DispatchAsync)
    return 0;

  int result = 0;
  if (responseRequired) {
    result = IcmProtocol::sendMessage(transport, output, comm);
    if (result == -1) {
      //Error
      LogMsg << "IcmProtocol::processServerRequest, cannot send reply: " << perr << endl;
    }
  }

  return result;
}
