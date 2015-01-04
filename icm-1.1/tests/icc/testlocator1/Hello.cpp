
#include <algorithm>
#include <string>
#include "Hello.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

static const ::std::string __Demo__MyHello_ids[2] =
{
    "::Demo::MyHello",
    "::Ice::Object"
};

DispatchStatus
Demo::MyHello::___sayHello(ServerRequest& __inS)
{
    InputStream& __is = __inS.incoming();
    OutputStream& __os = __inS.outgoing();
    Int i;
    __is.read_int(i);
    ::std::string __ret = sayHello(i);
    __inS.initReply ();
    __os.write_string(__ret);
    return DispatchOK;
}

static ::std::string __Demo__MyHello_all[] =
{
    "sayHello"
};

DispatchStatus
Demo::MyHello::__dispatch(ServerRequest& in)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__Demo__MyHello_all, __Demo__MyHello_all + 1, in.operation());
    if(r.first == r.second)
    {
	return DispatchOperationNotExist;
    }

    switch(r.first - __Demo__MyHello_all)
    {
	case 0:
	{
	    return ___sayHello(in);
	}
    }

    assert(false);
    return DispatchOperationNotExist;
}

::std::string
IcmProxy::Demo::MyHello::sayHello(Int i)
{
    static const char* __operation("sayHello");
    Reference* ref = this->reference ();
    TwowayInvocation _invocation (ref, __operation, ref->communicator ());
    _invocation.start ();
    _invocation.prepareHeader (1);
    OutputStream& __os = _invocation.outStream();
    __os.write_int(i);
    int  _invokeStatus = _invocation.invoke();
    if (_invokeStatus != 0)
return "";
    InputStream& __is = _invocation.inpStream();
    ::std::string __ret;
    __is.read_string(__ret);
    return __ret;
}
