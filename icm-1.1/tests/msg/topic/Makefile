
CPPFLAGS+=-I../../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm -licmmsg

all: Publisher Subscriber

Publisher: Publisher.o Network.o 
	g++ $(LDFLAGS) -o Publisher Publisher.o Network.o

Subscriber: MySubscriber.o Network.o NetworkI.o
	g++ $(LDFLAGS) -o Subscriber MySubscriber.o Network.o NetworkI.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o Publisher Subscriber
