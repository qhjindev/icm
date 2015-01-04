
CPPFLAGS+=-I../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../lib -lpthread -lrt -licc -licm -licmmsg

all: Client AmiClient AmdServer

Client: Client.o Hello.o 
	g++ $(LDFLAGS) -o Client Client.o Hello.o

AmiClient: AmiClient.o Hello.o
	g++ $(LDFLAGS) -o AmiClient AmiClient.o Hello.o

AmdServer: AmdServer.o Hello.o 
	g++ $(LDFLAGS) -o AmdServer AmdServer.o Hello.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o Client AmiClient AmdServer
