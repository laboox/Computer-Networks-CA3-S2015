#pragma once

#include "primary_header.h"	
#include "Packet.h"
#include "Sock.h"
#include "address.h"

typedef pair<string, int> eth_fd;
typedef pair<eth_fd, int> eth_fd_cost; 

#define DEFAULT_COST 1
#define _ETH first
#define _FD second
#define ETH first.first
#define FD first.second
#define COST second
#define DEST first
#define INF 1000

class Router
{
public:
	Router(int router_port);
	void accept_connection(Packet p, int client_fd);
	void pass_data(Packet p);
	void update_routing_table(string dest, int cost, int announcer_fd, string myEthCard);
	void update(Packet p, int client_fd);
	void broadcast(string dest, int cost);
	void connect_client(Packet p, int client_fd);
	void parse_packet(Packet p, int client_fd);
	
	void Eth(string ethCardNum);
	void NoEth(string ethCardNum);
	void connectEth(string myEthCard, string peerEthCard, int peer_listenPort);
	void parse_cmd(string cmd);
	
	void run();

private:
	int router_port;
	int router_fd;

	map<string, vector<address> > multicast_ip;
	
	map<string, vector<eth_fd_cost> > routing_table;
	map<string, vector<eth_fd> > eth_to_eth_fd; 
	
	vector<string> ethernet_cards;
	map<string, int> eth_cost;

	map<string, int> connected_client;
};