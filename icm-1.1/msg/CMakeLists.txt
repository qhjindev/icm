cmake_minimum_required(VERSION 2.6)
project(icmmsg C CXX)
include(../env_inc.make)
include(../icm-common.cmake)

set(ICM_SRC
  Msg.cpp
  MsgProtocol.cpp
  PublisherProxy.cpp
  SubscriberProxy.cpp
  TopicManager.cpp
  PublishClient.cpp
  SubscribeClient.cpp
  MsgServer.cpp
  MsgClient.cpp
  MsgBroker.cpp
  MsgBrokerI.cpp
)
link_libraries( pthread rt icc icm)

add_library(stlib_icmmsg ${ICM_SRC} )

add_executable(icmmsg Main.cpp)
target_link_libraries(icmmsg stlib_icmmsg)

add_library(icmmsglib SHARED ${ICM_SRC} )
target_link_libraries(icmmsglib stlib_icmmsg)
set_target_properties(icmmsglib PROPERTIES OUTPUT_NAME icmmsg)

if(NOT ( "${ON_HOST}" STREQUAL "true" ))
install(TARGETS
        icmmsg
        PUBLIC_HEADER DESTINATION "include/msg"
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION stlib
        RUNTIME DESTINATION bin
)
endif()

install(TARGETS
        icmmsglib
        PUBLIC_HEADER DESTINATION "include/msg"
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION stlib
        RUNTIME DESTINATION bin
)
