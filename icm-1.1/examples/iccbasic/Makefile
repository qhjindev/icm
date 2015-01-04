
CPPFLAGS+=-I../.. -I./  -g -D_REENTRANT -DACE_HAS_AIO_CALLS -D_GNU_SOURCE -DACE_NO_INLINE -DACE_HAS_LINUX_NPTL
LDFLAGS+=-L../../lib -lpthread -lrt -licc -licm -licmmsg

all: LogExample MessageBlockExample ThreadExample ConditionMutexExample

ConditionMutexExample: ConditionMutexExample.o
	g++ $(LDFLAGS) -o ConditionMutexExample ConditionMutexExample.o

ThreadExample: ThreadExample.o
	g++ $(LDFLAGS) -o ThreadExample ThreadExample.o

LogExample: LogExample.o
	g++ $(LDFLAGS) -o LogExample LogExample.o

MessageBlockExample: MessageBlockExample.o
	g++ $(LDFLAGS) -o MessageBlockExample MessageBlockExample.o

%.o : %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf *.o LogExample MessageBlockExample ThreadExample ConditionMutexExample
