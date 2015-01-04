
CPPFLAGS+=-I../../ -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./
LDFLAGS+=-L../..I -lpthread -lrt

all: testproactor1

testproactor1: testproactor1.o
	g++ $(LDFLAGS) -o testproactor1 testproactor1.o ../../libicc.a
	@echo build testproactor1 completed!

%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o testproactor1
