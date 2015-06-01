include ../Makefile

mylib.a: Packet.o Framer.o Sock.o address.o
	ar -cvr -o mylib.a Framer.o Packet.o Sock.o address.o
Packet.o: Packet.cpp Packet.h crc.h
	$(CC) -c Packet.cpp
Framer.o: Framer.h Framer.cpp
	$(CC) -c Framer.cpp
Sock.o: Sock.h Sock.cpp
	$(CC) -c Sock.cpp
address.o: address.h address.cpp
	$(CC) -c address.cpp
clean: 
	rm *.o *.a
