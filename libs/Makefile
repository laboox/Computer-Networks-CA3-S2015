include ../Makefile

mylib.a: Packet.o Framer.o Sock.o
	ar -cvr -o mylib.a Framer.o Packet.o Sock.o
Packet.o: Packet.cpp Packet.h crc.h
	$(CC) -c Packet.cpp
Framer.o: Framer.h Framer.cpp
	$(CC) -c Framer.cpp
Sock.o: Sock.h Sock.cpp
	$(CC) -c Sock.cpp
clean: 
	rm *.o *.a
