/**
* File "Framer.h"
* Created by Sina on Fri May  8 18:20:11 2015.
*/

#pragma once

#include "primary_header.h"
#include "Packet.h"

bool isFileExist(string path);

string readAllFile(string path);

void sendFrame(char* data, int count, address source, address dest, int sock);

void reciveFrame( char* data, int sock);

string reciveFrame(int sock);
