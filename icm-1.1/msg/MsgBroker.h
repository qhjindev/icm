
#ifndef __MsgBroker_h__
#define __MsgBroker_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
#include <icm/IcmError.h>
class ServerRequest;

namespace IcmProxy
{

namespace MsgBroker
{

class QueryState;

}

}

namespace MsgBroker
{

void __write(OutputStream*, IcmProxy::MsgBroker::QueryState* v);
void __read(InputStream*, IcmProxy::MsgBroker::QueryState* &v);

}

namespace MsgBroker
{

typedef ::std::vector< ::std::string> Topics;

class __U__Topics { };
std::string __toString(const Topics&);

struct QueryStateReq
{
  QueryStateReq();
  ::MsgBroker::Topics ts;

  bool operator==(const QueryStateReq&) const;
  bool operator!=(const QueryStateReq&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const QueryStateReq&);

struct State
{
  State();
  ::std::string topic;
  bool s;

  bool operator==(const State&) const;
  bool operator!=(const State&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const State&);

typedef ::std::vector< ::MsgBroker::State> States;

class __U__States { };
std::string __toString(const States&);
void __write(OutputStream*, const ::MsgBroker::State*, const ::MsgBroker::State*, __U__States);
void __read(InputStream*, States&, __U__States);

struct QueryStateRes
{
  QueryStateRes();
  ::MsgBroker::States ss;

  bool operator==(const QueryStateRes&) const;
  bool operator!=(const QueryStateRes&) const;

  void __write(OutputStream*) const;
  void __read(InputStream*);
  
  std::string toString() const;
};
std::string __toString(const QueryStateRes&);

}

namespace MsgBroker
{

class AMI_QueryState_queryStates : public ReplyHandler
{
public:

  virtual void response(Int, const ::MsgBroker::QueryStateRes&) = 0;
protected:

  virtual void __response(InputStream* in, ACE_CDR::UInt replyStatus);
};

}

namespace MsgBroker
{

class QueryState : virtual public Object
{
public:

  QueryState() {}

  virtual Int queryStates(const ::MsgBroker::QueryStateReq&, ::MsgBroker::QueryStateRes&) = 0;
  DispatchStatus ___queryStates(ServerRequest&);

  virtual DispatchStatus __dispatch(ServerRequest& in);
};

}

namespace IcmProxy
{

namespace MsgBroker
{

class QueryState : virtual public IcmProxy::Object
{
public:

  Int queryStates(const ::MsgBroker::QueryStateReq& req, ::MsgBroker::QueryStateRes& res);
  void queryStates_async(::MsgBroker::AMI_QueryState_queryStates*, const ::MsgBroker::QueryStateReq&);
};

}

}

#endif
