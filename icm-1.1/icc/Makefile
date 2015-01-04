

all: libicc.so libicc.a

icc: libicc.so

CPPFLAGS +=  -Wall -fPIC -g -O0 -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I../
LDFLAGS += -L.

#  OS_NS_ctype.o \
#  OS_NS_dirent.o \
#  OS_NS_dlfcn.o \
#  OS_NS_poll.o \
#  OS_NS_pwd.o \
#  OS_NS_regex.o \
#  OS_NS_signal.o \
#  OS_NS_stdlib.o \

ICCOBJ = \
  ../os/ACE.o \
  ../os/ace_wchar.o \
  ../os/OS_Errno.o \
  ../os/OS_NS_arpa_inet.o \
  ../os/OS_NS_ctype.o \
  ../os/OS_NS_errno.o \
  ../os/OS_NS_fcntl.o \
  ../os/OS_NS_math.o \
  ../os/OS_NS_netdb.o \
  ../os/OS_NS_stdio.o \
  ../os/OS_NS_stdlib.o \
  ../os/OS_NS_string.o \
  ../os/OS_NS_strings.o \
  ../os/OS_NS_stropts.o \
  ../os/OS_NS_sys_mman.o \
  ../os/OS_NS_sys_resource.o \
  ../os/OS_NS_sys_select.o \
  ../os/OS_NS_sys_socket.o \
  ../os/OS_NS_sys_stat.o \
  ../os/OS_NS_sys_time.o \
  ../os/OS_NS_sys_utsname.o \
  ../os/OS_NS_sys_wait.o \
  ../os/OS_NS_Thread.o \
  ../os/OS_NS_time.o \
  ../os/OS_NS_unistd.o \
  ../os/OS_NS_wchar.o \
  ../os/OS_QoS.o \
  ../os/Flag_Manip.o \
  ../os/Auto_Ptr.o \
  ../os/Basic_Types.o \
  ../icc/Addr.o \
  ../icc/AsynchIo.o \
  ../icc/AsynchIoImpl.o \
  ../icc/AutoEvent.o \
  ../icc/ConditionThreadMutex.o \
  ../icc/ConditionRecursiveThreadMutex.o \
  ../icc/Configuration.o \
  ../icc/DateTime.o \
  ../icc/Event.o \
  ../icc/EventHandler.o \
  ../icc/Guard.o \
  ../icc/HandleSet.o \
  ../icc/InetAddr.o \
  ../icc/Log.o \
  ../icc/MessageBlock.o \
  ../icc/Pipe.o \
  ../icc/Proactor.o \
  ../icc/Reactor.o \
  ../icc/RecursiveThreadMutex.o \
  ../icc/SelectReactor.o \
  ../icc/Socket.o \
  ../icc/SocketAcceptor.o \
  ../icc/SocketConnector.o \
  ../icc/SocketStream.o \
  ../icc/SocketDgram.o \
  ../icc/SvcHandler.o \
  ../icc/Thread.o \
  ../icc/ThreadManager.o \
  ../icc/ThreadMutex.o \
  ../icc/TimeValue.o \
  ../icc/TimerQueueAdapter.o

libicc.a: $(ICCOBJ) 
	rm -rf libicc.a
	ar cru libicc.a $(ICCOBJ)
	ranlib libicc.a
	mv libicc.a ../lib/

libicc.so: $(ICCOBJ)
	g++ -shared -Wl,-soname,libicc.so -o ../lib/libicc.so $(ICCOBJ)
	
%o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean: 
	rm -rf ../os/*.o ../icc/*.o ../lib/libicc.a ../lib/libicc.so



