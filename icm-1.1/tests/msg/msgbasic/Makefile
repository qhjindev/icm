
CPPFLAGS+=-I../../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm -licmmsg

all: Server Client

Server: Server.o MsgIdl.o 
	g++ $(LDFLAGS) -o Server Server.o MsgIdl.o

Client: Client.o MsgIdl.o
	g++ $(LDFLAGS) -o Client Client.o MsgIdl.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o Server Client
