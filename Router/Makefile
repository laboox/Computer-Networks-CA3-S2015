include ../Makefile

all: Router.out
Router.out: main.o
	$(CC) *.o $(CLIBS) -o Router.out
main.o: main.cpp Router.o
	$(CC) $(CFLAGS) -c main.cpp
Router.o: Router.cpp Router.h
	$(CC) $(CFLAGS) -c Router.cpp
	
clean:
	rm *.o *.out
