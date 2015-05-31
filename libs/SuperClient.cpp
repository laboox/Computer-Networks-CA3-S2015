/**
* File "SuperClient.cpp"
* Created by Sina on Sun May 31 10:50:23 2015.
*/

#include "SuperClient.h"

SuperClient::SuperClient(address IP, address serverIp, int routerPort){
    char portstr[128]={0};
    this->IP = IP;
    this->serverIP = serverIP;
    itoa(routerPort, portstr, 10);
    connect("localhost", portstr, &routerFd);
    //TODO other connect procedurals
}
