
#ifndef __Hello_h__
#define __Hello_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace Demo
{

}

namespace Demo
{

class MyHello : virtual public Object
{
public:

    MyHello() {}

    virtual ::std::string sayHello(Int) = 0;
    DispatchStatus ___sayHello(ServerRequest&);

    virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace Demo
{

class MyHello : virtual public IcmProxy::Object
{
public:

    ::std::string sayHello(Int i);
};

}

}

#endif
