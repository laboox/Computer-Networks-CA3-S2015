/**
* File "Framer.cpp"
* Created by Sina on Fri May  8 18:20:06 2015.
*/

#include "Framer.h"

bool isFileExist(string path){
   std::ifstream t(path);
    if(t.good()){
        t.close();
        return true;
    }
    else{
        t.close();
        return false;
    }   
}

string readAllFile(string path){
    std::ifstream t(path);
    if(!t.good()){
        throw Exeption("file does not exist.\n");
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
    return str;
}

void sendFrame(char* data, int count, address source, address dest, int sock){
    int base=0;
    unsigned char packetNumber=0;
    char* first = (char*)data;
    while(base < count){
        char inPacketData[23] = {0};
        Packet p;
        p.setType(DATA);
        p.setDest(dest);
        p.setSource(source);

        if(count-base>DATA_LEN-1){
            inPacketData[0] = packetNumber;
            char* startOfData = inPacketData+1;
            memcpy(startOfData, first, DATA_LEN-1);
            p.setData(inPacketData, DATA_LEN);
            p.putCrc();
            first+=DATA_LEN-1;
            base+=DATA_LEN-1;
        }
        else {
            inPacketData[0] = packetNumber;
            char* startOfData = inPacketData+1;
            memcpy(startOfData, first, count-base+1);
            p.setData(inPacketData, count-base + 1);
            p.putCrc();
            first+=count-base;
            base+=count-base;
        }
        p.send(sock);
        packetNumber++;
    }
}

void reciveFrame( char* data, int sock) {
    char* first = data;
    int count=0;

    while(1){
        char temparr[30] = {0};
        char* temp = temparr;
        Packet p;
        p.recive(sock);
        if(p.getType()!=DATA){
            throw Exeption(p.getDataStr());
        }
        int n = p.getData(temp);
        if(count!=temp[0])
            throw Exeption("error reciving frame");
        temp = (char*)temp + 1;
        memcpy(first, temp, DATA_LEN-1);
        first += n-1;
        if(n<DATA_LEN)
            break;
        count++;
    }
}

string reciveFrame(int sock){
    int count=0;
    string ret;
    while(1){
        char temparr[30] = {0};
        char* temp = temparr;
        Packet p;
        p.recive(sock);
        int n = p.getData(temp);
        ret+=temparr;
        if(count!=temp[0])
            throw Exeption("error reciving frame");
        if(n<DATA_LEN)
            break;
        count++;
    }
    return ret;
}
