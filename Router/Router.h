#pragma once

#include "primary_header.h"	
#include "Packet.h"
#include "Sock.h"

typedef pair<string, int> eth_fd;
typedef pair<eth_fd, int> eth_fd_cost; 

#define DEFAULT_COST 1
#define ETH first.first
#define FD first.second
#define COST second
#define DEST first
#define INFINITY 1000

class Router
{
public:
	Router(int router_port);
	void Eth(string ethCardNum);
	void NoEth(string ethCardNum);
	void connectEth(string myEthCard, string peerEthCard, int peer_listenPort);
	void acceptConnection(Packet p, int client_fd);
	void pass_data(Packet p);
	void join_update(Packet p, int client_fd);
	void update(Packet p, int client_fd);
	void update_and_broadcast(address dest, int cost);
	void parse_packet(Packet p, int client_fd);
	void parse_cmd(string cmd);
	void run();

private:
	int router_port;
	int router_fd;

	map<address, vector<address> > multicast_ip;
	
	map<address, vector<eth_fd_cost> > routing_table;
	map<string, vector<int> > fdsOfeth; 
	
	vector<string> ethernet_cards;
	map<string, int> eth_cost;

	map<string, int> connected_client;
};