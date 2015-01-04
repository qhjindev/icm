

CPPFLAGS+=-I../../../ -I${ACE_ROOT} -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./ 
#LDFLAGS+=-L/home/qinghua/projects/icc/icc-1.0/lib -lpthread -lrt -licc
LDFLAGS+=-L../../../lib -lpthread -lrt -licc

all: testlog2

testlog1: testlog1.o
	g++ $(LDFLAGS) -o testlog2 testlog2.o 

%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o testlog2 log*.txt
