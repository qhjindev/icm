
CPPFLAGS+=-I../../../ -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm

all: Client AmhServer

Client: Client.o my-amd.o
	g++ $(LDFLAGS) -o Client Client.o my-amd.o

AmhServer: AmhServer.o AmhMyHelloI.o my-amd.o
	g++ $(LDFLAGS) -o AmhServer AmhServer.o AmhMyHelloI.o my-amd.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o Client AmhServer
