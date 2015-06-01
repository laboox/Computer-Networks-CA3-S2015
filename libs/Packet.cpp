#include "Packet.h" 

void Packet::setType(PacketType pt){
    type = bitset<16>(pt);
}

void Packet::setData(const char data[23], unsigned int size){
    length = bitset<40>(size);
    this->data.reset();
    for ( int i=0; i< size*8; i++ ) {
        this->data[i] = data[i/8] & 1<<(i%8);
    }
}

void Packet::setData(string data){
    if(data.size()>22)
        throw Exeption("Message is too large too send\n");
    length = bitset<40>(data.size()+1);
    setData(data.c_str(), data.size()+1);
}

int Packet::getData(char* data){
    int size = length.to_ulong();
    memset(data, 0, size);
    for ( int i=0; i< size; i++ ) {
        data[i] = 0;
    }
    for ( int i=0; i< size*8; i++ ) {
        data[i/8] |= (this->data[i])<<(i%8);
    }
    return size;
}

string Packet::getDataStr(){
    char cdata[DATA_LEN] = {0};
    getData(cdata);
    return cdata;
}

void Packet::putCrc() {
    char packchar[MSG_LEN] = {0};
    crc.reset();
    Crc32 crcgen;

    getMessageByPacket(packchar);
    crcgen.AddData((unsigned char*)packchar, 42);
    crc = bitset<48>((crcgen.getCrc32()));
}

int Packet::checkCrc() {
    char packchar[MSG_LEN] = {0};
    Crc32 crcgen;

    getMessageByPacket(packchar);
    crcgen.AddData((unsigned char*)packchar, 42);
    bitset<48> crccheck(crcgen.getCrc32());
    return crccheck.to_ulong() == crc.to_ulong();
}

bitset<336> Packet::getPacketBitsetKam() {
    bitset<336> ret;
    bitset<336> temp;
    ret.reset();
    temp.reset();
    
    temp = bitset<336>(type.to_string());
    ret |= temp;

    temp = bitset<336>(dest.to_string());
    ret |= (temp << 16);

    temp = bitset<336>(source.to_string());
    ret |= (temp << 48);
    
    temp = bitset<336>(ttl.to_string());
    ret |= (temp << 80);
    
    temp = bitset<336>(length.to_string());
    ret |= (temp << 112);
    
    temp = bitset<336>(data.to_string());
    ret |= (temp << 152);
    
    return ret;
}

bitset<384> Packet::getPacketBitset(){
    bitset <384> ret;
    bitset <384> temp;
    ret.reset();
    temp.reset();
    ret = bitset<384>(getPacketBitsetKam().to_string());
    temp = bitset<384>(crc.to_string());
    ret |= temp << 336;
    return ret;
}

void Packet::getMessageByPacket ( char* pack ){
    bitset<384> packSet;
    packSet = getPacketBitset();
    for( int i=0; i<384; i++ ) {
        pack[i/8] |= (packSet[i] << (i%8));
    }
}

void Packet::getPacketByMessage ( char* pack ){
    type.reset();
    dest.reset();
    source.reset();
    ttl.reset();
    length.reset();
    data.reset();
    crc.reset();
    for ( int i=0; i<384; i++ ) {
        if(i/8 < 2){
            type[(i-0)%16] = pack[i/8] & (1<<(i%8)) ;
        }
        else if( (2 <= i/8) && (i/8 < 6)){
            dest[(i-16)%32] = pack[i/8] & (1<<(i%8)) ;
        }
        else if( (6 <= i/8) && (i/8 < 10)){
            source[(i-48)%32] = pack[i/8] & (1<<(i%8)) ;
        }
        else if( (10 <= i/8) && ( i/8 < 14) ){
            ttl[(i-80)%32] = pack[i/8] & (1<<(i%8)) ;
        }
        else if( (14 <= i/8) && ( i/8 < 19) ){
            length[(i-112)%40] = pack[i/8] & (1<<(i%8)) ;
        }
        else if( (19 <= i/8) && (i/8 < 42) ){
            data[(i-152)%184] = pack[i/8] & (1<<(i%8)) ;
        }
        else if( (42 <= i/8) && ( i/8 < 48) ){
            crc[(i-336)%48] = pack[i/8] & (1<<(i%8)) ;
        }
    }
    cerr<<"packet recived\n";
}

void Packet::decTtl(){
    if(ttl.to_ulong()>0)
        ttl = bitset<32>(ttl.to_ulong()-1);
}

Packet::Packet()
{
    setTtl(INIT_TTL);
}

Packet::Packet(char* ss) 
{ 
    setTtl(INIT_TTL);
    getPacketByMessage(ss); 
}

void Packet::send(int sockfd)
{
    putCrc();
    char msg[MSG_LEN] = {0};
    this->getMessageByPacket(msg);

    int n=write(sockfd, msg, MSG_LEN);
    if(n<0) 
        throw Exeption("Error in sendto");
}

void Packet::recive(int sockfd)
{
    char msg[MSG_LEN] = {0};
    int n=read(sockfd, msg, MSG_LEN);
    sh(n);
    if(n<0)
        throw Exeption("Error in recvfrom");
    this->getPacketByMessage(msg);
    if(!checkCrc())
        throw Exeption("CRC Error.");
}
