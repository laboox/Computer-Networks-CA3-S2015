include ../Makefile

all: GroupServer.out
GroupServer.out: main.o
	$(CC) *.o $(CLIBS) -o GroupServer.out
main.o: main.cpp GroupServer.o
	$(CC) $(CFLAGS) -c main.cpp
GroupServer.o: GroupServer.cpp GroupServer.h
	$(CC) $(CFLAGS) -c GroupServer.cpp
	
clean:
	rm *.o *.out
