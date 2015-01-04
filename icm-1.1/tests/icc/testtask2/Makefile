

CPPFLAGS+=-I../../.. -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./
LDFLAGS+=-L../../.. -lpthread -lrt

all: TestTask2

TestTask2: TestTask2.o
	g++ $(LDFLAGS) -o TestTask2 TestTask2.o ../../libicc.a

%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o TestTask2

remake:
	clean
	all
