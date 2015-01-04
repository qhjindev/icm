
#include "icm/Communicator.h"
#include "icm/ObjectAdapter.h"
#include "icm/Object.h"
#include "icm/Endpoint.h"
#include "icc/Configuration.h"
#include "SubscriberProxy.h"
#include "PublisherProxy.h"
#include "MsgBrokerI.h"
#include "TopicManager.h"

using namespace std;

Int QueryStateI::queryStates(const ::MsgBroker::QueryStateReq& req, ::MsgBroker::QueryStateRes& res) {
  res.ss = TopicManager::instance()->queryState(req.ts);
  return 0;
}


int MessageBroker::svc() {
  Configuration config;
  if(config.open("../conf/icmmsg.ini") == -1){
    ICC_ERROR("open conf/icmmsg.ini failed");
    return -1;
  }

  short publishPort = config.getConfigShort("General", "publishPort");
  short subscribePort = config.getConfigShort("General", "subscribePort");
  short queryPort = config.getConfigShort("General", "queryPort");
  SubscriberHandler::keepAliveTryTimes = config.getConfigInt("General", "keepAliveTryTimes");
  SubscriberHandler::keepAliveTimeSpan = config.getConfigInt("General", "keepAliveTimeSpan");
  SubscriberHandler::enableBuffering = config.getConfigBool("General", "enableBuffering");

  string logOutput = config.getConfigStr("Log", "logOutput");
  string logFileName;
  if(logOutput == "file") {
    logFileName = "../log/log";
  } else if(logOutput == "console") {
    logFileName = "";
  }

  int maxLogFileSize = config.getConfigInt("Log", "maxLogFileSize");
  int maxLogFiles = config.getConfigInt("Log", "maxLogFiles");
  string logLevel = config.getConfigStr("Log", "logLevel");

  LogPriority logPriority;
  if(logLevel == "info") {
    logPriority = LM_INFO;
  } else if(logLevel == "debug") {
    logPriority = LM_DEBUG;
  } else if(logLevel == "warning") {
    logPriority = LM_WARNING;
  } else if(logLevel == "error") {
    logPriority = LM_ERROR;
  } else {
    logPriority = LM_DEBUG;
  }

//  cout << "logfile name: " << logFileName << ", logPriority: " << logPriority << ", maxLogFileSize: " << maxLogFileSize;
//  cout << ", maxLogFiles: " << maxLogFiles << endl;
  Log::instance()->open(logFileName.c_str(), logPriority, maxLogFileSize * 1024 * 1024, maxLogFiles);

  Communicator* comm = Communicator::instance();
  if (comm->init () == -1)
    return -1;
  Reactor* reactor = comm->reactor();

  PubAcceptor pubAcceptor;
  InetAddr pubListenAddr(publishPort);
  if (pubAcceptor.open(pubListenAddr, reactor) == -1) {
    ICC_ERROR("MessageBroker::svc pub acceptor error: %s", OS::strerror(OS::map_errno(OS::last_error())));
    return -1;
  }

  SubAcceptor subAcceptor;
  InetAddr subListenAddr(subscribePort);
  if (subAcceptor.open(subListenAddr, reactor) == -1) {
    ICC_ERROR("MessageBroker::svc sub acceptor error: %s", OS::strerror(OS::map_errno(OS::last_error())));
    return -1;
  }

  Endpoint* endpoint = new Endpoint("TCP", "", queryPort);
  ObjectAdapter* oa = Communicator::instance()->createObjectAdapterWithEndpoint ("QueryState", endpoint);
  Object* object = new QueryStateI;
  oa->add (object, "QueryState");

  TimeValue timeout(1);
  while(true) {
    reactor->handleEvents(&timeout);
  }

  return 0;
}

