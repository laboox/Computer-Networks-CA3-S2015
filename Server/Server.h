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
private:
    string name;
    vector<pair<string, address> > groups;
};