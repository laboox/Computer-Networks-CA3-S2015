/**
* File "Packet.h"
* Created by Sina on Thu May  7 18:41:22 2015.
*/

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "crc.h"
#include "primary_header.h"


#define ADDR_LEN 128
#define DATA_LEN 23
#define INIT_TTL 20

typedef bitset<ADDR_LEN> address;

enum PacketType{
    ACK,
    UPDATE,
    DATA,
    ROUTER_CONNECT,
    CLIENT_CONNECT,
    DISCONNECT,
    SETUPSERVICE,
    ERROR,
    GET_GROUPS_LIST,
    REQ_READ,
    REQ_WRITE,
    SEND_FILE,
    REQ_SERVER,
    ACCEPT_CONNECTION
};

class Packet {
public:
    void setType(PacketType);
    void setDest(address dest) { this->dest = dest; }
    void setSource(address source) { this->source = source; }
    void setTtl(unsigned int ttl) { this->ttl = bitset<32>(ttl); }
    void setData(const char data[23], unsigned int); 
    void setData(string data);
    PacketType getType() { return (PacketType)type.to_ulong(); }
    address getDest() { return dest; }
    address getSource() { return source; }
    unsigned int getTtl() { return ttl.to_ulong(); }
    unsigned int getLength() { return length.to_ulong(); }
    int getData(char*);
    string getDataStr();
    void decTtl();

    void putCrc();
    int checkCrc();

    bitset<336> getPacketBitsetKam();
    bitset<384> getPacketBitset();
    void getMessageByPacket ( char* );
    void getPacketByMessage ( char* );

    void send(int sockfd);
    void recive(int sockfd);

    Packet();
    Packet(char* ss);
private:
    bitset<16> type;
    address dest;
    address source;
    bitset<32>  ttl;
    bitset<40> length;
    bitset<184> data;
    bitset<48> crc;

};
