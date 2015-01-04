
CPPFLAGS+=-I../../../ -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm

all: Client Server

Client: Client.o Progress.o MyCallbackI.o
	g++ $(LDFLAGS) -o Client Client.o Progress.o MyCallbackI.o

Server: Server.o Progress.o VmServiceI.o
	g++ $(LDFLAGS) -o Server Server.o Progress.o VmServiceI.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o Client Server
