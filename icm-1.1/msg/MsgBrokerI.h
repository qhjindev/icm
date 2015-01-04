#ifndef MSG_BROKER_I_H
#define MSG_BROKER_I_H

#include "icc/Task.h"
#include "MsgBroker.h"

class QueryStateI : public MsgBroker::QueryState {
public:
  virtual Int queryStates(const ::MsgBroker::QueryStateReq&, ::MsgBroker::QueryStateRes&);
};


class MessageBroker : public Task<MT_SYNCH> {
public:

  virtual int svc();
};


#endif //MSG_BROKER_I_H
