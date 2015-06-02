/**
* File "Server.h"
* Created by Sina on Sun May 31 13:39:05 2015.
*/

#pragma once

#include "SuperClient.h"
#include "address.h"

class Server: public SuperClient{
public:
    Server(address IP, address serverIp, int routerPort);
    void run();
    void sendGroupsList(address dest);
    void updateGroups(address source, string data);
    void joinReq(address user, string group);
    void showMyGroups(address dest);
private:
    vector<pair<string, address> > groups;
    map<string, vector<string> > perToGroups;
};
