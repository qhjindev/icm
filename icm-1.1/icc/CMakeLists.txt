cmake_minimum_required(VERSION 2.6.2)
include("../env_inc.make")
project(icc CXX C)
include(../icm-common.cmake)

set(OS_SRC
  ../os/ACE.cpp
  ../os/ace_wchar.cpp
  ../os/OS_Errno.cpp
  ../os/OS_NS_arpa_inet.cpp
  ../os/OS_NS_ctype.cpp
  ../os/OS_NS_errno.cpp
  ../os/OS_NS_fcntl.cpp
  ../os/OS_NS_math.cpp
  ../os/OS_NS_netdb.cpp
  ../os/OS_NS_stdio.cpp
  ../os/OS_NS_stdlib.cpp
  ../os/OS_NS_string.cpp
  ../os/OS_NS_strings.cpp
  ../os/OS_NS_stropts.cpp
  ../os/OS_NS_sys_mman.cpp
  ../os/OS_NS_sys_resource.cpp
  ../os/OS_NS_sys_select.cpp
  ../os/OS_NS_sys_socket.cpp
  ../os/OS_NS_sys_stat.cpp
  ../os/OS_NS_sys_time.cpp
  ../os/OS_NS_sys_utsname.cpp
  ../os/OS_NS_sys_wait.cpp
  ../os/OS_NS_Thread.cpp
  ../os/OS_NS_time.cpp
  ../os/OS_NS_unistd.cpp
  ../os/OS_NS_wchar.cpp
  ../os/OS_QoS.cpp
  ../os/Flag_Manip.cpp
  ../os/Auto_Ptr.cpp
  ../os/Basic_Types.cpp
)
set(SRC_FILES
    Addr.cpp
  AsynchIo.cpp
  AsynchIoImpl.cpp
  AutoEvent.cpp
  ConditionThreadMutex.cpp
  Configuration.cpp
  DateTime.cpp
  Event.cpp
  EventHandler.cpp
  Guard.cpp
  HandleSet.cpp
  InetAddr.cpp
  Log.cpp
  MessageBlock.cpp
  Pipe.cpp
  Proactor.cpp
  Reactor.cpp
  RecursiveThreadMutex.cpp
  SelectReactor.cpp
  Socket.cpp
  SocketAcceptor.cpp
  SocketConnector.cpp
  SocketStream.cpp
  SocketDgram.cpp
  SvcHandler.cpp
  Thread.cpp
  ThreadManager.cpp
  ThreadMutex.cpp
  TimeValue.cpp
)
add_library(icc SHARED ${OS_SRC} ${SRC_FILES})
target_link_libraries(icc pthread)

install(TARGETS
        icc
        PUBLIC_HEADER DESTINATION "include/icc"
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION stlib
        RUNTIME DESTINATION bin
)

