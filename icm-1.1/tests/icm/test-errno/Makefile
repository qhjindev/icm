

CPPFLAGS+=-I../../../ -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm

all: client server

client: client.o my.o
	g++ $(LDFLAGS) -o client client.o my.o

server: server.o my.o MyHelloI.o
	g++ $(LDFLAGS) -o server server.o my.o MyHelloI.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o client server
