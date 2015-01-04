
CPPFLAGS+=-I../ -I./ -g

all: testsocket1

testsocket1: testsocket1.o 
	g++ -o testsocket1 testsocket1.o ../libicc.a
	@echo build testsocket1 completed!

%.o: %.c
	g++ -c $(CPPFLAGS) $< -o $@

clean:
	rm -rf *.o 
