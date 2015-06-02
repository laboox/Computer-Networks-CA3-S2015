/**
* File "Server.h"
* Created by Sina on Sun May 31 13:39:05 2015.
*/

#pragma once

#include "SuperClient.h"
#include "address.h"

class GroupServer: public SuperClient{
public:
    GroupServer(string name, address multiIP, address IP, address serverIp, int routerPort);
    void run();
private:
    string name;
    address multiIP;
    set<string> aza;
};
