/**
* File "address.cpp"
* Created by Sina on Sun May 31 10:56:12 2015.
*/

#include "address.h"

string addrToString(address addr){
    ostringstream iss;
    unsigned char ip[4] = {0};
    for(int i=0;i<addr.size();i++){
        ip[i/8] |= addr[i] << i%8;
    }
    iss<<(int)ip[0];
    iss<<".";
    iss<<(int)ip[1];
    iss<<".";
    iss<<(int)ip[2];
    iss<<".";
    iss<<(int)ip[3];
    return iss.str();
}

address stringToAddr(string str){
    address ret;
    ret.reset();
    unsigned char ip[4] = {0};
    if(sscanf(str.c_str(), "%d.%d.%d.%d",ip, ip+1, ip+2, ip+3)<=0)
        throw Exeption("error parsing address");
    for(int i=0;i<ret.size();i++){
        ret[i] = ip[i/8] & (1<<i%8);
    }
    return ret;
}
