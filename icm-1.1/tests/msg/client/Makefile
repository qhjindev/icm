
CPPFLAGS+=-I../../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm -licmmsg

all: MyPub MySub

MyPub: MyPub.o IcmEvent.o 
	g++ $(LDFLAGS) -o MyPub MyPub.o IcmEvent.o

MySub: MySub.o IcmEvent.o
	g++ $(LDFLAGS) -o MySub MySub.o IcmEvent.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o MyPub MySub
