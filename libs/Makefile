include ../Makefile

mylib.a: Packet.o Framer.o address.o SuperClient.o
include ../Makefile

mylib.a: Packet.o Framer.o Sock.o address.o SuperClient.o
	ar -cvr -o mylib.a Framer.o Packet.o Sock.o address.o SuperClient.o
Packet.o: Packet.cpp Packet.h crc.h
	$(CC) -c Packet.cpp
Framer.o: Framer.h Framer.cpp
	$(CC) -c Framer.cpp
Sock.o: Sock.h Sock.cpp
	$(CC) -c Sock.cpp
address.o: address.h address.cpp
	$(CC) -c address.cpp
SuperClient.o: SuperClient.h SuperClient.cpp
	$(CC) -c SuperClient.cpp

clean: 
	rm *.o *.a
