/**
* File "Server.cpp"
* Created by Sina on Sun May 31 13:39:03 2015.
*/

#include "Server.h"

Server::Server(address IP, address serverIp, int routerPort)
                : SuperClient(IP, serverIp, routerPort)
{
}

void Server::run(){
    while(true){
        Packet p;
        p.recive(routerFd);
        if(p.getType()==GET_GROUPS_LIST){
            sendGroupsList(p.getSource());
        }
    }
}

void Server::sendGroupsList(address dest){
    string gr;
    Packet res;
    res.setType(GET_GROUPS_LIST);
    res.setDest(dest);
    res.setSource(IP);
    gr+="g";
    for(int i=0;i<groups.size();i++){
        gr+=groups[i].first + " " + addrToString(groups[i].second) + "\n";
    }
    res.setData(gr);
    cerr<<"groups list sent\n";
    res.send(routerFd);
}
