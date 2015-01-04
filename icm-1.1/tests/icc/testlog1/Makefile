
ICM_HOME=/home/qinghua/projects/tvm4.2/icm/icm-1.1
CPPFLAGS+=-I../../../ -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL -I./ 
LDFLAGS+=-L${ICM_HOME}/lib -lpthread -lrt -licc

all: testlog1

testlog1: testlog1.o
	g++ $(LDFLAGS) -o testlog1 testlog1.o 

testlogi:
	g++ $(CPPFLAGS) -c -E testlog1.cpp > testlog1.i
	
%.o : %.c
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o testlog1 testlog1.i
