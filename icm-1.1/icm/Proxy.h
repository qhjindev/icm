#ifndef ICM_PROXY_H
#define ICM_PROXY_H

#include <string>
#include "icm/Reference.h"
#include "icm/IcmStream.h"
#include "Pluggable.h"
#include "icc/ThreadMutex.h"

class InputStream;
class OutputStream;
class Communicator;
class Reference;
class IcmTransport;

namespace IcmProxy
{

class Object
{
public:
  Object (Reference* ref = 0, IcmTransport* transport = 0) :
      mTransport (transport)
  {
    if (ref)
      this->mReference = new Reference(*ref);
    else
      this->mReference = 0;
  }

  ~Object ()
  {
    if (mReference)
      delete mReference;
  }

  void ping();
  //void ping(const Context&);

  void setReference(Reference* ref)
  {
    if (mReference)
      delete mReference;

    this->mReference = new Reference(*ref);
  }

  Reference* getReference (void)
  {
    return this->mReference;
  }

  void transport (IcmTransport* transport)
  {
    this->mTransport = transport;
  }

  IcmTransport*& transport (void)
  {
    return this->mTransport;
  }

  Identity identity () const
  {
  	return mReference->identity();
  }

  void copyFrom(IcmProxy::Object*);

  //T uncheckedCast(IcmProxy::Object*);

protected:
  ThreadMutex mutex;
private:

  Reference* mReference;

  IcmTransport* mTransport;
};

extern bool IsCallSuccess();
extern void setCallErrno( int callErrno );
}

class ProxyFactory
{
public:
  ProxyFactory(Communicator*);

  IcmProxy::Object* toProxy (const char* name,
                             const char* category,
                             const char* prot,
                             const char* host,
                             unsigned short port);

  void proxyToStream(IcmProxy::Object* proxy, OutputStream* s) const;

  IcmProxy::Object* streamToProxy(InputStream* s) const;

  IcmProxy::Object* stringToProxy(const std::string&) const;

  IcmProxy::Object* referenceToProxy(Reference*) const;

private:
  Communicator* mCommunicator;
};

#endif //ICM_PROXY_H
