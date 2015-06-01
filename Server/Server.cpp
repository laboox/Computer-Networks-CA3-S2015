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
        else if(p.getType() == REQ_SERVER){
            updateGroups(p.getSource(), p.getDataStr());
        }
        else if(p.getType() == REQ_JOIN){
            joinReq(p.getSource(), p.getDataStr());
        }
        else if(p.getType() == REQ_LEAVE){
            
        }
    }
}

void Server::joinReq(address user, string group){
    for(int i=0;i<groups.size();i++){
        if(groups[i].first == group){
            perToGroups[addrToString(user)].push_back(group);
            Packet p;
            p.setType(REQ_JOIN);
            p.setDest(groups[i].second);
            p.setSource(IP);
            p.setData(addrToString(user));
            p.send(routerFd);
        }
    }
    sendError("group not exist.\n", user);
}

void Server::updateGroups(address source, string data){
    string name, mulIp;
    istringstream iss(data);
    iss>>name>>mulIp;
    groups.push_back(pair<string, address>(name, source));
    cout<<"group "<<name<<" with ip "<<mulIp<<" added.\n";
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
