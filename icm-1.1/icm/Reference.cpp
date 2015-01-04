#include "icm/Communicator.h"
#include "icm/Reference.h"
#include "icm/IcmStream.h"
#include "icm/Identity.h"
#include "icc/SocketConnector.h"

Reference::Reference(Communicator* comm, const char* name, const char* category, const char* protocol, const char* host,
    unsigned short port, TimeValue* maxWaitTime) :
    mCommunicator(comm), mIdentity(name, category), mEndpoint(protocol, host, port), mMaxWaitTime(maxWaitTime) {
}

Reference::Reference(Communicator* comm, const Identity& identity, const Endpoint& endpoint, TimeValue* maxWaitTime) :
    mCommunicator(comm), mIdentity(identity), mEndpoint(endpoint), mMaxWaitTime(maxWaitTime) {

}

Reference::Reference(Communicator* comm, const Identity& identity, TimeValue* maxWaitTime) :
    mCommunicator(comm), mIdentity(identity), mMaxWaitTime(maxWaitTime) {

}

Reference::Reference(const Identity& identity, TimeValue* maxWaitTime) :
    mIdentity(identity), mMaxWaitTime(maxWaitTime) {
  mCommunicator = Communicator::instance();
}

Reference::Reference(const Identity& identity, const Endpoint& endpoint, TimeValue* maxWaitTime) :
    mIdentity(identity), mEndpoint(endpoint), mMaxWaitTime(maxWaitTime) {
  mCommunicator = Communicator::instance();
}

Reference::Reference(const Reference& r) :
    mCommunicator(r.mCommunicator), mIdentity(r.mIdentity), mContext(r.mContext), mEndpoint(
        r.endpoint()->protocol.c_str(), r.endpoint()->host.c_str(), r.endpoint()->port), mMaxWaitTime(r.mMaxWaitTime) {
}

Reference::~Reference() {
}

bool
Reference::pingPeer() {
  InetAddr serverAddr(mEndpoint.port, mEndpoint.host.c_str());
  SocketStream stream;

  SocketConnector connector;
  bool result = (connector.connect(stream, serverAddr) == 0);
  if(result) {
    stream.close();
  }
  return result;
}

const Identity&
Reference::identity() const {
  return this->mIdentity;
}

const Context&
Reference::context() const {
  return this->mContext;
}

Communicator*
Reference::communicator() const {
  return this->mCommunicator;
}

Endpoint*
Reference::endpoint() const {
  return (Endpoint*) &mEndpoint;
}

void Reference::read(InputStream* s) {
  mEndpoint.read(s);
}

void Reference::write(OutputStream* s) const {
  mEndpoint.write(s);
}

Reference*
ReferenceFactory::create(const Identity& ident, InputStream* s) {
  if (ident.name.empty() && ident.category.empty()) {
    return 0;
  }

  Reference* ref = new Reference(ident);
  ref->read(s);
  return ref;
}

Reference*
ReferenceFactory::create(const Identity& ident, const Endpoint& endpoint) {
  if (ident.name.empty() && ident.category.empty()) {
    return 0;
  }

  return new Reference(ident, endpoint);
}

Reference*
ReferenceFactory::create(const std::string &str) {
  return 0;
}

