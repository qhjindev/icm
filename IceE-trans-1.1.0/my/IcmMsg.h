
#ifndef __IcmMsg_h__
#define __IcmMsg_h__

#include <string>
#include <vector>
#include <os/OS.h>
#include <icm/Proxy.h>
#include <icm/Object.h>
#include <icm/ReplyDispatcher.h>
#include <icm/AmhResponseHandler.h>
class ServerRequest;

namespace IcmProxy
{

namespace IcmMsg
{

class Topic;

class TopicManager;

}

}

namespace IcmMsg
{

void __write(OutputStream*, IcmProxy::IcmMsg::Topic* v);
void __read(InputStream*, IcmProxy::IcmMsg::Topic* &v);

void __write(OutputStream*, IcmProxy::IcmMsg::TopicManager* v);
void __read(InputStream*, IcmProxy::IcmMsg::TopicManager* &v);

