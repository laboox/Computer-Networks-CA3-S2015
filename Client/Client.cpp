/**
* File "Client.cpp"
* Created by Sina on Sun May 31 13:39:03 2015.
*/

#include "Client.h"

Client::Client(string name, address IP, address serverIp, int routerPort)
                : SuperClient(IP, serverIp, routerPort)
{
    this->name = name;
}

void Client::run(){
    fd_set router_fds, read_fds;
    FD_ZERO(&router_fds);
    FD_ZERO(&read_fds);
    FD_SET(0, &router_fds);
    FD_SET(routerFd, &router_fds);
    sh(routerFd);
    
    int max_fd = routerFd;
	while(true){
        read_fds = router_fds;
        if(select(max_fd+1, &read_fds, NULL, NULL, NULL) < 0)
            throw Exeption("problem in sockets select!");
        for(int client_fd=0; client_fd<=max_fd ; client_fd++)
        {
        	try
        	{
        		if(FD_ISSET(client_fd , &read_fds))
        		{
	                if(client_fd==0)
	                {
                        cerr<<"in recive\n";
	                    string cmd;
                        getline(cin, cmd);
	                    parseCmd(cmd);
	                }
	                else if(client_fd==routerFd)
	                {
                        cerr<<"sock recive\n";
                        Packet p;
                        p.recive(routerFd);
                        parsePacket(p);
	                }
     			}
     		}
        	catch(Exeption ex)
        	{
        		cout<<ex.get_error()<<endl;
        	}
        }
    }
}

void Client::updateGroups(string data){
    istringstream iss(data);
    string name, addr;
    while(iss>>name>>addr){
        groups[name] = stringToAddr(addr);
    }
}

void Client::parsePacket(Packet p){
    if(p.getType() == GET_GROUPS_LIST){
        cout<<"Groups are:\n";
        cout<<p.getDataStr();
    }
}

void Client::parseCmd(string line){
    string cmd0, cmd1, cmd2;
    istringstream iss(line);
    iss>>cmd0;
    if(cmd0=="Get"){
        if(iss>>cmd1>>cmd2 && cmd1=="group" && cmd2=="list"){
            getGroupList();
            cout<<"group list request sent.\n";
        } else {
            throw Exeption("invalid cmd");
        }
    }
    else if(cmd0=="Select"){
        if(iss>>cmd1){
            selectGroup(cmd1);
        } else {
            throw Exeption("invalid cmd");
        }
    }
    else if(cmd0=="Join"){
        if(iss>>cmd1){
            //joinGroup(cmd1);
        } else {
            throw Exeption("invalid cmd");
        }
    }
}

void Client::getGroupList(){
    Packet p;
    p.setType(GET_GROUPS_LIST);
    p.setSource(IP);
    p.setDest(serverIP);
    p.send(routerFd);
}

void Client::selectGroup(string g){
    if(groups.count(g)<=0)
        throw Exeption("Group does not exist");
    selGroup = g;
    cout<<"group "<< g << " with ip " << addrToString( groups[g] ) <<" selected!\n";
}
