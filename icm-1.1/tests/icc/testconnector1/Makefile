

CPPFLAGS+=-I../../../ -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./
LDFLAGS+=-L../../../lib -lpthread -lrt -licc

all: testconnector1

testconnector1: testconnector1.o
	g++ $(LDFLAGS) -o testconnector1 testconnector1.o 
	@echo build testconnector1 completed!

%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o testconnector1
