/**
* File "Client.h"
* Created by Sina on Sun May 31 13:39:05 2015.
*/

#pragma once

#include "SuperClient.h"
#include "address.h"

class Client: public SuperClient{
public:
    Client(string name, address IP, address serverIp, int routerPort);
    void selectGroup(string g);
    void run();
    void parseCmd(string line);
    void parsePacket(Packet p);
private:
    string name;
    map<string, address> groups;
    string selGroup;
};
