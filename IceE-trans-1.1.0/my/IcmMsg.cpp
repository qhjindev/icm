
#include <algorithm>
#include <string>
#include "IcmMsg.h"
#include <icm/ServerRequest.h>
#include <icm/IcmStream.h>
#include <icm/Invocation.h>
#include <icm/AmhResponseHandler.h>

void
IcmMsg::__write(OutputStream* __os, IcmProxy::IcmMsg::Topic* v)
{
  __os->write_obj(v);
}

void
IcmMsg::__read(InputStream* __is, IcmProxy::IcmMsg::Topic* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::IcmMsg::Topic;
    v->copyFrom(proxy);
  }
}

void
IcmMsg::__write(OutputStream* __os, IcmProxy::IcmMsg::TopicManager* v)
{
  __os->write_obj(v);
}

void
IcmMsg::__read(InputStream* __is, IcmProxy::IcmMsg::TopicManager* &v)
{
  ::IcmProxy::Object* proxy;
  __is->read_obj(proxy);
  if(!proxy)
  {
    v = 0;
  }
  else
  {
    v = new ::IcmProxy::IcmMsg::TopicManager;
    v->copyFrom(proxy);
  }
