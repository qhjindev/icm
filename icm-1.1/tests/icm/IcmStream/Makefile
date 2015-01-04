CPPFLAGS+=-I../../../ -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../../lib -lpthread -lrt -licc -licm

all: StreamTest

StreamTest: StreamTest.o
	g++ $(LDFLAGS) -o StreamTest StreamTest.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o StreamTest
