
CPPFLAGS+=-I../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../lib -lpthread -lrt -licc -licm -licmmsg

all: PublishExample SubscribeExample

PublishExample: PublishExample.o
	g++ $(LDFLAGS) -o PublishExample PublishExample.o

SubscribeExample: SubscribeExample.o
	g++ $(LDFLAGS) -o SubscribeExample SubscribeExample.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o PublishExample SubscribeExample
