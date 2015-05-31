include ../Makefile

mylib.a: Packet.o Framer.o
	ar -cvr -o mylib.a Framer.o Packet.o
Packet.o: Packet.cpp Packet.h crc.h
	$(CC) -c Packet.cpp
Framer.o: Framer.h Framer.cpp
	$(CC) -c Framer.cpp
clean: 
	rm *.o *.a
