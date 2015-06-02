#pragma once

#include "primary_header.h"	
#include "Packet.h"
#include "Sock.h"
#include "address.h"

typedef pair<string, int> eth;
typedef int fd;

#define NAME first
#define COST second

class Router
{
public:
	Router(int router_port);
	void run();

private:
	int router_port;
	int router_fd;

	map<address, int> unicast_routing_table;
	map<address, vector<address> > multicats_routing_table;
	
	vector<eth> ethernet_cards;
};
