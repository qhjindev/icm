
CPPFLAGS+=-I../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../lib -lpthread -lrt -licc -licm -licmmsg

all: ReactorServer ReactorClient dgram

ReactorServer: ReactorServer.o
	g++ $(LDFLAGS) -o ReactorServer ReactorServer.o

ReactorClient: ReactorClient.o
	g++ $(LDFLAGS) -o ReactorClient ReactorClient.o

dgram: Dgram.o
	g++ $(LDFLAGS) -o dgram Dgram.o
	
%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o ReactorServer ReactorClient dgram
