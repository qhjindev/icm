

CPPFLAGS+=-I../../.. -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./
LDFLAGS+=-L../../../lib -lpthread -lrt -licc

all: TestThread1

TestThread1: TestThread1.o
	g++ $(LDFLAGS) -o TestThread1 TestThread1.o

%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o TestThread1

remake:
	clean
	all
