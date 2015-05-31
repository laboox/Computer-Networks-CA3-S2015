include ../Makefile

mylib.a: Packet.o Framer.o address.o SuperClient.o
	ar -cvr -o mylib.a Framer.o Packet.o address.o SuperClient.o
Packet.o: Packet.cpp Packet.h crc.h
	$(CC) -c Packet.cpp
Framer.o: Framer.h Framer.cpp
	$(CC) -c Framer.cpp
address.o: address.h address.cpp
	$(CC) -c address.cpp
SuperClient.o: SuperClient.h SuperClient.cpp
	$(CC) -c SuperClient.cpp

clean: 
	rm *.o *.a
