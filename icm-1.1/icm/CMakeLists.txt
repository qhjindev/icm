cmake_minimum_required(VERSION 2.6)
project(icm C CXX)

include(../env_inc.make)
include(../icm-common.cmake)

set(ICM_SRC
  AmhResponseHandler.cpp
  ClientStrategyFactory.cpp
  Communicator.cpp
  Endpoint.cpp
  Identity.cpp
  IcmProtocol.cpp
  IcmStream.cpp
  Invocation.cpp
  Object.cpp
  ObjectAdapter.cpp
  ObjectAdapterFactory.cpp
  Pluggable.cpp
  Proxy.cpp
  Reference.cpp
  ReplyDispatcher.cpp
  ResourceFactory.cpp
  ServantManager.cpp
  ServerRequest.cpp
  TcpAcceptor.cpp
  TcpConnection.cpp
  TcpConnector.cpp
  TcpTransport.cpp
  TransportCacheManager.cpp
  TransportMuxStrategy.cpp
  WaitStrategy.cpp
)

add_library(icm SHARED ${ICM_SRC})

install(TARGETS
        icm
        PUBLIC_HEADER DESTINATION "include/icm"
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION stlib
        RUNTIME DESTINATION bin
)
