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

    Packet p;
    p.setType(CLIENT_CONNECT);
    p.setSource(IP);
    p.send(routerFd);

    cout<<"Connect to router "<<routerPort<<" successfully"<<endl;
}

void SuperClient::sendUnicast(address destIP, string msg)
{
    Packet p;
    p.setType(DATA);
    p.setSource(IP);
    p.setDest(destIP);
    p.send(routerFd);
    cout<<"I send packet to router port"<<endl;
}

void SuperClient::reciveUnicast(Packet p)
{
	cout <<"I recive packet with data = "<<p.getDataStr()<<" from source = "<<addrToString(p.getSource())<<endl;
}

void SuperClient::sendError(string message, address dest){
    Packet p;
    p.setType(ERROR);
    p.setSource(IP);
    p.setDest(dest);
    p.setData(message);
    p.send(routerFd);
}
