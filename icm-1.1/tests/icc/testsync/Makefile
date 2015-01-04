CPPFLAGS+=-I../../.. -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./
LDFLAGS+=-L../../../lib -lpthread -lrt -licc

all: TestCondition

TestCondition: TestCondition.o
	g++ $(LDFLAGS) -o TestCondition TestCondition.o

%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o TestCondition

remake:
	clean
	all
