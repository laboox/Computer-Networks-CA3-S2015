/**
* File "Server.cpp"
* Created by Sina on Sun May 31 13:39:03 2015.
*/

#include "GroupServer.h"

GroupServer::GroupServer(string name, address multiIP, address IP, address serverIp, int routerPort)
                : SuperClient(IP, serverIp, routerPort)
{
    this->name = name;
    this->multiIP = multiIP;
}

void GroupServer::run(){
    string cmd0, cmd1, line;
    while(getline(cin, line)){
        istringstream iss(line);
        if(iss>>cmd0>>cmd1){
            if(cmd0=="Add" && cmd1=="server"){
                cout<<"adding to server.\n";
                Packet p;
                p.setType(REQ_SERVER);
                p.setSource(IP);
                p.setDest(serverIP);
                p.setData(name + "\t" + addrToString(multiIP));
                p.send(routerFd);
                break;
            }
        }
        cout<<"you should add server\n";
    }
    while(true){
        Packet p;
        p.recive(routerFd);
        if(p.getType()==REQ_JOIN){
            string ip = p.getDataStr();
            stringToAddr(ip);
            //TODO add ip to broadcast ip
        }
        
    }
}

