/**
* File "SuperClient.cpp"
* Created by Sina on Sun May 31 10:50:23 2015.
*/

#include "SuperClient.h"

SuperClient::SuperClient(address IP, address serverIp, int routerPort){
    char portstr[128]={0};
    this->IP = IP;
    this->serverIP = serverIP;
    connect("localhost", routerPort, &routerFd);
    //TODO other connect procedurals
}
