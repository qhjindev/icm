#ifndef ICM_REFERENCE_H
#define ICM_REFERENCE_H

#include <string>
#include <map>
#include "Endpoint.h"
#include "icm/Identity.h"
#include "icc/TimeValue.h"

typedef std::map<std::string, std::string> Context;

class Communicator;

class Reference
{
public:

  const Identity& identity() const;

  const Context& context() const;

  Communicator* communicator() const;

  Endpoint* endpoint() const;

  void write(OutputStream* s) const;

  void read(InputStream*) ;

  TimeValue* getMaxWaitTime() {
    return mMaxWaitTime;
  }

public:
  Reference(Communicator*,
            const char* name,
            const char* category,
            const char* protocol,
            const char* host,
            unsigned short port,
            TimeValue* maxWaitTime = 0);

  Reference(Communicator*,
            const Identity& identity,
            const Endpoint& endpoint,
            TimeValue* maxWaitTime = 0);

  Reference(Communicator*,const Identity&, TimeValue* maxWaitTime = 0) ;

  Reference(const Identity&, TimeValue* maxWaitTime = 0) ;

  Reference(const Identity&, const Endpoint&, TimeValue* maxWaitTime = 0);

  Reference(const Reference&);

  virtual ~Reference();

  bool pingPeer();

private:

  Communicator* mCommunicator;

  Identity mIdentity;

  Context mContext;

  Endpoint mEndpoint;

  TimeValue* mMaxWaitTime;
};

class ReferenceFactory
{
public:
	Reference* create(const Identity& ident, InputStream* s);

	Reference* create(const Identity& ident, const Endpoint& endpoint);

  Reference* create(const std::string&);

};

#endif //ICM_REFERENCE_H
