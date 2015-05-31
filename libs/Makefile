include ../Makefile

mylib.a: Packet.o Framer.o address.o
	ar -cvr -o mylib.a Framer.o Packet.o address.o
Packet.o: Packet.cpp Packet.h crc.h
	$(CC) -c Packet.cpp
Framer.o: Framer.h Framer.cpp
	$(CC) -c Framer.cpp
address.o: address.h address.cpp
	$(CC) -c address.cpp
clean: 
	rm *.o *.a
