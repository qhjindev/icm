#include "Proxy.h"
#include "Reference.h"
#include "Communicator.h"
#include <errno.h>

//static bool s_bCallSuccess = false;
bool IcmProxy::IsCallSuccess() { return errno == 0; }
void IcmProxy::setCallErrno( int callErrno ) {
  errno = callErrno;
//	if ( callErrno == 0 ) {
//		errno = callErrno;
//	} else if ( errno == 0 ) {
//		errno = callErrno;
//	} else {
//		// don't recover the real errno.
//	}
}
//
void IcmProxy::Object::ping()
{
  //this->ping(this->mReference->getContext());
}
/*
void IcmProxy::Object::ping(const Context & ctx)
{

}
*/

void
IcmProxy::Object::copyFrom(IcmProxy::Object* from)
{
	this->mReference = from->getReference();
}

ProxyFactory::ProxyFactory(Communicator *com) :
  mCommunicator(com)
{
}

IcmProxy::Object*
ProxyFactory::toProxy (const char* name,
                       const char* category,
                       const char* prot,
                       const char* host,
                       unsigned short port)
{
  Reference* reference = new Reference (mCommunicator, name, category, prot, host, port);
  return this->referenceToProxy (reference);
}

void
ProxyFactory::proxyToStream(IcmProxy::Object* proxy, OutputStream* s) const
{
	if(proxy)	{
		proxy->getReference()->identity().write(s);
		proxy->getReference()->write(s);
	} else {
		Identity ident;
		ident.write(s);
	}
}

IcmProxy::Object*
ProxyFactory::streamToProxy(InputStream* s) const
{
	Identity ident;
	ident.read(s);

	Reference* ref = mCommunicator->referenceFactory()->create(ident, s);
	return referenceToProxy(ref);
}

IcmProxy::Object*
ProxyFactory::stringToProxy(const std::string& str) const
{
  Reference* ref = this->mCommunicator->referenceFactory()->create(str);
  return this->referenceToProxy(ref);
}

IcmProxy::Object*
ProxyFactory::referenceToProxy(Reference* ref) const
{
  if(ref) {
    IcmProxy::Object* obj = new IcmProxy::Object;
    obj->setReference(ref);
    return obj;
  } else  {
    return 0;
  }
}
