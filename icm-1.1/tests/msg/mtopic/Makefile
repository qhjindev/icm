
CPPFLAGS+=-I../../../ -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm -licmmsg

all: App1 App2

App1: App1.o Event.o EventI.o
	g++ $(LDFLAGS) -o App1 App1.o Event.o EventI.o

App2: App2.o Event.o EventI.o
	g++ $(LDFLAGS) -o App2 App2.o Event.o EventI.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o App1 App2
