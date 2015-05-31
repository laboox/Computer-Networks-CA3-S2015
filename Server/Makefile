include ../Makefile

all: Server.out
Server.out: main.o
	$(CC) *.o $(CLIBS) -o Server.out
main.o: main.cpp Server.o
	$(CC) $(CFLAGS) -c main.cpp
Server.o: Server.cpp Server.h
	$(CC) $(CFLAGS) -c Server.cpp
	
clean:
	rm *.o *.out
