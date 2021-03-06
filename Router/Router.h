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
#define MULTICAST_IP first
#define UNICAST_IPs second
#define CLIENT_IP first
#define CLIENT_FD second
#define ETH_FD_COSTs second
#define ETH_FDs second

class Router
{
public:
	Router(int router_port);
	void accept_connection(Packet p, int client_fd);
	void pass_unicast_data(Packet p);
	void pass_multicast_data(Packet p);
	void update_routing_table(string dest, int cost, int announcer_fd, string myEthCard);
	void update(Packet p, int client_fd);
	void broadcast(string dest, int cost);
	void connect_client(Packet p, int client_fd);
	void join(Packet p);
	void leave(Packet p);
	void remove_connection(Packet p, int client_fd);
	void parse_packet(Packet p, int client_fd);
	
	void Eth(string ethCardNum);
	void NoEth(string ethCardNum);
	void connectEth(string myEthCard, string peerEthCard, int peer_listenPort, int& peer_fd);
	void show();
	void parse_cmd(string cmd, int& peer_fd);
	
	void run();

private:
	int router_port;
	int router_fd;

	map<string, vector<string> > multicast_ip;
	
	map<string, vector<eth_fd_cost> > routing_table;
	map<string, vector<eth_fd> > eth_to_eth_fd; 
	map<int, int> fd_to_port;

	vector<string> ethernet_cards;
	map<string, int> eth_cost;

	map<string, int> connected_client;
};