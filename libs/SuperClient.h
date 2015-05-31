/**
* File "SuperClient.h"
* Created by Sina on Sun May 31 10:50:26 2015.
*/

#pragma once

#include "Packet.h"

class SuperClient{
public:
    SuperClient(address IP, address serverIp, int routerPort);
private:
    address IP;
    address serverIP;
    int routerFd;
};
