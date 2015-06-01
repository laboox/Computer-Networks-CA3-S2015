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
                p.setDest(multiIP);
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
        if(p.getType()==GET_GROUPS_LIST){
            sendGroupsList(p.getSource());
        }
    }
}

void GroupServer::sendGroupsList(address dest){
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
