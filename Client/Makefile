include ../Makefile

all: Client.out
Client.out: main.o
	$(CC) *.o $(CLIBS) -o Client.out
main.o: main.cpp Client.o
	$(CC) $(CFLAGS) -c main.cpp
Client.o: Client.cpp Client.h
	$(CC) $(CFLAGS) -c Client.cpp
	
clean:
	rm *.o *.out
