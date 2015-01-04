
CPPFLAGS+=-I../../../ -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm

all: AsyncClient Server

AsyncClient: AsyncClient.o Hello.o
	g++ $(LDFLAGS) -o AsyncClient AsyncClient.o Hello.o

Server: Server.o Hello.o HelloI.o
	g++ $(LDFLAGS) -o Server Server.o Hello.o HelloI.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o AsyncClient Server
