
CPPFLAGS+=-I../../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm -licmmsg

all: client1 client2

client1: client1.o mclient.o 
	g++ $(LDFLAGS) -o client1 client1.o mclient.o 

client2: client2.o mclient.o
	g++ $(LDFLAGS) -o client2 client2.o mclient.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o client1 client2
