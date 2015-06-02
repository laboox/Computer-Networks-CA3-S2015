#include "Router.h"

Router::Router(int router_port)
{
	struct sockaddr_in router_address;
    router_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (router_fd < 0) 
        throw Exeption("Problem in openning socket!");
    build_server(router_fd, &router_address, router_port); 

    this->router_port = router_port;
    cout<<"Router bind on port "<<router_port<<endl;
}

void Router::Eth(string ethCardNum)
{
	if(eth_cost.find(ethCardNum) != eth_cost.end())
		throw Exeption("Repeated EthCardNum");
	ethernet_cards.push_back(ethCardNum);
	eth_cost[ethCardNum]=DEFAULT_COST;
	cout<<"Ethernet Card "<<ethCardNum<<" added successfully";
}

void Router::NoEth(string ethCardNum)
{
	for(int i=0; i<ethernet_cards.size(); i++)
		if(ethernet_cards[i] == ethCardNum)
		{
			ethernet_cards.erase(ethernet_cards.begin()+i);
			eth_cost.erase(ethCardNum);
			cout<<"Ethernet Card "<<ethCardNum<<" removed successfully";
			return;
		}
	throw Exeption(ethCardNum+"Not found to remove");

	//TODO send remove packet to adjacents
}

string itoa(int n)
{
    string s;
    while(n>0)
    {
        char c=n%10+'0';
        s=c+s;
        n/=10;
    }
    return s;
}   

void Router::connectEth(string myEthCard, string peerEthCard, int peer_listenPort)
{
	if(peer_listenPort == router_port)
        throw Exeption("You can't connect yourself");
    
    if(eth_to_eth_fd.find(myEthCard)==eth_to_eth_fd.end())
    	throw Exeption("You must add this eth first");

    int peer_fd;
    connect("localhost", peer_listenPort, &peer_fd);
	cout<<"connect to socket of the router on "<<peer_listenPort<<" ..."<<endl;    

    Packet p;
    p.setType(ROUTER_CONNECT);
    p.setSource(address(router_port));
    p.setData(myEthCard+" "+peerEthCard);
    p.send(peer_fd);

    cout<<"send connection request packet to router on "<<peer_listenPort<<" ..."<<endl;
    
    p.recive(peer_fd);
    cout<<"recive response from router on "<<peer_listenPort<<" ..."<<endl;

    if(p.getType()==ERROR)
    	throw Exeption(p.getDataStr());

    for(map<string, vector<eth_fd_cost> >::iterator it=routing_table.begin(); it!=routing_table.end(); ++it)
    {
    	vector<eth_fd_cost> v=routing_table[it->DEST];
    	int minCost=INF;
    	for(int i=0; i<v.size(); i++)
    		minCost=min(minCost, v[i].COST);

        Packet p;
        p.setType(UPDATE);
        p.setDest(address(it->DEST));
        p.setData(peerEthCard+" "+itoa(minCost));
        p.send(peer_fd);
    }

    cout<<"send update packets to router on "<<peer_listenPort<<" ..."<<endl;

    eth_to_eth_fd[myEthCard].push_back(eth_fd(peerEthCard, peer_fd));
	
	cout<<myEthCard<<" connect to "<<peerEthCard<<" of port "<<peer_listenPort<<"successfully"<<endl;

}

void Router::accept_connection(Packet p, int client_fd)
{
	string myEthCard, peerEthCard;
	int peer_listPort=p.getSource().to_ulong();
	stringstream ss(p.getDataStr());
	ss >> peerEthCard >> myEthCard;
	
	bool found=false;
	for(int i=0; i<ethernet_cards.size(); i++)
		if(ethernet_cards[i]==myEthCard)
			found=true;
	if(! found)
	{
		cout<<"I recive connection request to "<<myEthCard<<" witch i couldnt find it in my eth list"<<endl;

    	Packet p;
    	p.setType(ERROR);
    	p.setData("this port has'nt any "+myEthCard);
    	p.send(client_fd);
    	return;
    }
    else
		p.send(ACK);
	cout<<"I send ack packet to "<<client_fd<<endl;

	vector<eth_fd> v=eth_to_eth_fd[myEthCard];
	for(int i=0; i<v.size(); i++)
		if(v[i]._FD==client_fd && v[i]._ETH==peerEthCard)
			return;

	connectEth(myEthCard, peerEthCard, peer_listPort);
	cout<<"Connection accept with my "<<myEthCard<<" whith peer "<<peerEthCard<<" of peer port "<<peer_listPort<<endl;
}


void Router::broadcast(string dest, int cost)
{
	for(int j=0; j<ethernet_cards.size(); j++)
	{
		for(int k=0; k<eth_to_eth_fd[ethernet_cards[j]].size(); k++)
		{
			int peer_fd = eth_to_eth_fd[ethernet_cards[j]][k]._FD;
	        string peer_EthCard = eth_to_eth_fd[ethernet_cards[j]][k]._ETH;
	        
			Packet p;
	        p.setType(UPDATE);
	        p.setDest(stringToAddr(dest));
	        p.setData(peer_EthCard+" "+itoa(cost));
	        p.send(peer_fd);
		}
	}
}

void Router::update_routing_table(string dest, int cost, int announcer_fd, string myEthCard)
{

	if(routing_table.find(dest)==routing_table.end())
	{
		routing_table[dest].push_back(eth_fd_cost(eth_fd(myEthCard, announcer_fd), cost+1));
		broadcast(dest, cost+1);
		return;
	}

	vector<eth_fd_cost> v=routing_table[dest];
	for(int i=0; i<v.size(); i++)
	{
		if(v[i].ETH==myEthCard && v[i].FD==announcer_fd && v[i].COST>=(cost+1)) //replace 1 with exact cost = max(eth1,eth2)
		{
			routing_table[dest][i].COST=cost+1;//replace lator with exact  cost;
			broadcast(dest, cost+1);
		}
	}
}

void Router::update(Packet p, int announcer_fd)
{
	string myEthCard;
	int cost;
	stringstream ss(p.getDataStr());
	ss >> myEthCard >> cost;

	update_routing_table(addrToString(p.getDest()), cost, announcer_fd, myEthCard);
}

void Router::pass_data(Packet p)
{
	cout<<"passing packet from source:"<<addrToString(p.getSource())<<" to dest:"<<addrToString(p.getDest())<<" ..."<<endl;
    string dest = addrToString(p.getDest());
    if(connected_client.find(dest) != connected_client.end())
    {
      	p.send(connected_client[dest]);
      	cout<<"I have direct connection to dest and I send it"<<endl;
    }
    else if(routing_table.find(dest) != routing_table.end())
    {
        p.setTtl(p.getTtl()-1);
        int indx=0;
        int minCost=routing_table[dest][0].COST;
        for (int i=0; i <routing_table[dest].size(); ++i)
        {
        	if(routing_table[dest][i].COST<minCost)
        	{
        		minCost=routing_table[dest][i].COST;
        		indx=i;
        	}
        }
        p.send(routing_table[dest][indx].FD);   
        cout<<"I have indirect connection to dest and I send by my routing table"<<endl;
    }
    else
        throw Exeption("I dont know any path to send this packet");
}

void Router::connect_client(Packet p, int client_fd)
{
	for(map<string, int> ::iterator it=connected_client.begin(); it!=connected_client.end(); it++)
        if(it->_FD == client_fd)
            throw Exeption("I recive a connect request from a clinent whitch I connected before");

    //assume that packet data is ip of client
    string client_ip=addrToString(p.getSource());
    connected_client[client_ip] = client_fd;

    p.setSource(address(router_port));
    p.setDest(stringToAddr(client_ip));
    p.setType(ACCEPT_CONNECTION);
    p.send(client_fd);

    broadcast(client_ip, 1);
    cout<<"I connect to client "<<client_ip<<endl;
}

void Router::parse_packet(Packet p, int client_fd)
{
	if(p.getTtl()==0) 
        return;
	if(p.getType()==UPDATE)
		update(p, client_fd);
	else if(p.getType()==DATA)
		pass_data(p);
	else if(p.getType()==ROUTER_CONNECT)
		accept_connection(p, client_fd);
	else if(p.getType()==CLIENT_CONNECT)
		connect_client(p, client_fd);
}

void Router::parse_cmd(string cmd)
{
	stringstream ss(cmd);
	string cmd_type;
	ss>>cmd_type;
	if(cmd_type=="Eth")
	{
		string ethCardNum;
		if(ss>>ethCardNum)
			Eth(ethCardNum);
		else
			throw Exeption("Invalid Oprand, Usage: Eth #EthernetCardNumber");
	}
	else if(cmd_type=="NoEth")
	{
		string ethCardNum;
		if(ss>>ethCardNum)
			NoEth(ethCardNum);
		else
			throw Exeption("Invalid Oprand, Usage: NoEth #EthernetCardNumber");
	}
	else if(cmd_type=="Connect")
	{
		string myEthCard,peerEthCard;
		int peer_listenPort;
		if(ss >> myEthCard >> peerEthCard >> peer_listenPort)
			connectEth(myEthCard, peerEthCard, peer_listenPort);
		else
			throw Exeption("Invalid Oprand, Usage: Connect #my_EthernetCard #peer_EthernetCard #peer_listenPort");
	}
	/*
	else if(cmd_type=="ChangeCost")
	{
		string ethCard;
		int newCost;
		if(ss >> ethCard >> newCost)
			changeCost(ethCard, newCost);
		else
			throw Exeption("Invalid Oprand, Usage: ChangeCost #EthernetCard #newCost");
	}
	else if(cmd_type=="Disconnect")
	{
		string ethCard;
		if(ss>>ethCard)
			disconnect(ethCard);
		else
			throw Exeption("Invalid Oprand, Usage: Disconnect #EthernetCard");
	}
	else if(cmd_type=="Show")
	{
		show();
	}
	else
		throw Exeption("Invalid Command");
	*/
}

void Router::run()
{
    fd_set router_fds, read_fds;
    FD_ZERO(&router_fds);
    FD_ZERO(&read_fds);
    FD_SET(0, &router_fds);
    FD_SET(router_fd, &router_fds);
    int i=0;
    int needfd[2];
    
    int max_fd = router_fd;
	while(true){
        read_fds = router_fds;
        if(select(max_fd+1, &read_fds, NULL, NULL, NULL) < 0)
            throw Exeption("problem in sockets select!");
        for(int client_fd=0; client_fd<=max_fd ; client_fd++)
        {
        	try
        	{
        		if(FD_ISSET(client_fd , &read_fds))
        		{
	                if(client_fd==0)
	                {
	                    string cmd;
	                    getline(cin,cmd);
	                    parse_cmd(cmd);
        			}
	                else if(client_fd!=router_fd)
	                {
	                	Packet p;
                		p.recive(client_fd);
                		parse_packet(p, client_fd); 
	                }
	                /*
	                else if(client_fd!=router_fd)
	                {
                        cout<<"forwarding\n";
                        Packet p;
	                    p.recive(client_fd);
                        if(client_fd == needfd[0]){
                            cerr<<"send to: "<<needfd[1]<<endl;
                            p.send(needfd[1]);
                        }
                        else{
                            cerr<<"send to: "<<needfd[0]<<endl;
                            p.send(needfd[0]);
                        }
	                }
	                */
	                else
	                {
	                    socklen_t client_address_size;
	                    struct sockaddr_in client_address;
	                    client_address_size = sizeof(client_address);
	                    int socket_accept_fd = accept(router_fd, (struct sockaddr *) &client_address, &client_address_size);
	                    if (socket_accept_fd ==-1)
	                        throw Exeption("Problem in accept");
	                    FD_SET(socket_accept_fd, &router_fds);
	                    if(socket_accept_fd > max_fd)
	                        max_fd = socket_accept_fd;
                     	//needfd[i++] = socket_accept_fd;
	                    //cout<<"accept: "<<socket_accept_fd<<endl;
	                }
     			}
     		}
        	catch(Exeption ex)
        	{
        		cout<<ex.get_error()<<endl;
        	}
        }
    }
}

