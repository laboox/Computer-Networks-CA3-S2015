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
	cout<<"Ethernet Card "<<ethCardNum<<" added successfully"<<endl;
}

void Router::NoEth(string myEthCardNum)
{
	map<string, vector<eth_fd_cost> > routing_table;
	map<string, vector<eth_fd> > eth_to_eth_fd; 
	
	for(int i=0; i<ethernet_cards.size(); i++)
		if(ethernet_cards[i] == ethCardNum)
		{
			ethernet_cards.erase(ethernet_cards.begin()+i);
			eth_cost.erase(ethCardNum);
			cout<<"Ethernet Card "<<ethCardNum<<" removed successfully"<<endl;
			return;
		}
	throw Exeption(ethCardNum+"Not found to remove");
	

	for(map<string, vector<eth_fd> >::iterator it=eth_to_eth_fd.begin(); it!=eth_to_eth_fd.end(); ++it)
    {
    	vector<eth_fd> v=it->ETH_FDs;

    }
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

void Router::connectEth(string myEthCard, string peerEthCard, int peer_listenPort, int& peer_fd)
{
	if(peer_listenPort == router_port)
        throw Exeption("You can't connect yourself");
    
    bool found=false;
	for(int i=0; i<ethernet_cards.size(); i++)
		if(ethernet_cards[i]==myEthCard)
			found=true;
	if(! found)
		throw Exeption("You must add this eth first");

	vector<eth_fd> v=eth_to_eth_fd[myEthCard];
	for(int i=0; i<v.size(); i++)
		if(fd_to_port[v[i]._FD]==peer_listenPort && v[i]._ETH==peerEthCard)
			throw Exeption("I connect with my "+myEthCard+" to "+peerEthCard+" by port I connected before");


    connect("localhost", peer_listenPort, &peer_fd);
	cout<<"connect to socket of the router on "<<peer_listenPort<<" and set fd = "<<peer_fd<<" for it ..."<<endl;    

    Packet p;
    p.setType(ROUTER_CONNECT);
    p.setSource(address(router_port));
    p.setData(myEthCard+" "+peerEthCard);
    p.send(peer_fd);

    cout<<"send connection request packet to router on "<<peer_listenPort<<" ..."<<endl;
    
    /*
    p.recive(peer_fd);
    cout<<"recive response from router on "<<peer_listenPort<<" ..."<<endl;

    if(p.getType()==ERROR)
    	throw Exeption(p.getDataStr());
	*/

    for(map<string, vector<eth_fd_cost> >::iterator it=routing_table.begin(); it!=routing_table.end(); ++it)
    {
    	vector<eth_fd_cost> v=routing_table[it->DEST];
    	int minCost=INF;
    	for(int i=0; i<v.size(); i++)
    		minCost=min(minCost, v[i].COST);

        Packet p;
        p.setType(UPDATE);
        p.setDest(stringToAddr(it->DEST));
        p.setData(peerEthCard+" "+itoa(minCost));
        p.send(peer_fd);
    }

    cout<<"send update packets to router on port = "<<peer_listenPort<<" and fd = "<<peer_fd<<" ..."<<endl;

    eth_to_eth_fd[myEthCard].push_back(eth_fd(peerEthCard, peer_fd));
    fd_to_port[peer_fd]=peer_listenPort;
	
	cout<<myEthCard<<" connect to "<<peerEthCard<<" of port "<<peer_listenPort<<" successfully"<<endl;
}

void Router::accept_connection(Packet p, int peer_fd)
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
    	p.setData("No "+myEthCard);
    	p.send(peer_fd);
    	return;
    }
    /*else
		p.send(ACK);
	*/

	//cout<<"I send ack packet to "<<client_fd<<endl;

	int peer_listenPort=p.getSource().to_ulong();
	vector<eth_fd> v=eth_to_eth_fd[myEthCard];
	for(int i=0; i<v.size(); i++)
		if(fd_to_port[v[i]._FD]==peer_listenPort && v[i]._ETH==peerEthCard)
			throw Exeption("I recive a connect request to my "+myEthCard+" from "+peerEthCard+" by port witch I connected before");

	
	cout<<"I recive request to connect from fd = "<<peer_fd<<" and port = "<<peer_listenPort<<endl;
	
	//connectEth(myEthCard, peerEthCard, peer_listPort);

	for(map<string, vector<eth_fd_cost> >::iterator it=routing_table.begin(); it!=routing_table.end(); ++it)
    {
    	vector<eth_fd_cost> v=routing_table[it->DEST];
    	int minCost=INF;
    	for(int i=0; i<v.size(); i++)
    		minCost=min(minCost, v[i].COST);
    	Packet p;
        p.setType(UPDATE);
        p.setDest(stringToAddr(it->DEST));
        p.setData(peerEthCard+" "+itoa(minCost));
        p.send(peer_fd);
        cout<<"sending update packet of dest = "<<(it->DEST)<<" to fd = "<<peer_fd<<endl;
    }

    cout<<"finish sending update packets to router on port = "<<peer_listenPort<<" and fd = "<<peer_fd<<" ..."<<endl;
    
    eth_to_eth_fd[myEthCard].push_back(eth_fd(peerEthCard, peer_fd));
    fd_to_port[peer_fd]=peer_listenPort;
	
	cout<<"Connection accept with my "<<myEthCard<<" whith peer "<<peerEthCard<<" of peer port "<<peer_listPort<<endl;
}


void Router::broadcast(string dest, int cost)
{
	for(int j=0; j<ethernet_cards.size(); j++)
	{
		//sh(ethernet_cards[j]);
		for(int k=0; k<eth_to_eth_fd[ethernet_cards[j]].size(); k++)
		{
			int peer_fd = eth_to_eth_fd[ethernet_cards[j]][k]._FD;
	        string peer_EthCard = eth_to_eth_fd[ethernet_cards[j]][k]._ETH;
	        
	        //sh(peer_EthCard);sh(peer_fd);
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
		cout<<"insert unicast ip "<<dest<<" to my roating table with announcer_fd = "<<announcer_fd<<endl;
		routing_table[dest].push_back(eth_fd_cost(eth_fd(myEthCard, announcer_fd), cost+1));
		broadcast(dest, cost+1);
		return;
	}

	vector<eth_fd_cost> v=routing_table[dest];
	for(int i=0; i<v.size(); i++)
	{
		if(v[i].ETH==myEthCard && v[i].FD==announcer_fd && v[i].COST>=(cost+1)) //replace 1 with exact cost = max(eth1,eth2)
		{
			cout<<"update unicast ip "<<dest<<" in my roating table"<<endl;
			routing_table[dest][i].COST=cost+1;//replace lator with exact  cost;
			broadcast(dest, cost+1);
		}
	}
}

void Router::update(Packet p, int announcer_fd)
{
	cout<<"Update packet recived from "<<announcer_fd<<endl;
	string myEthCard;
	int cost;
	stringstream ss(p.getDataStr());
	ss >> myEthCard >> cost;

	//sh(myEthCard);sh(cost);
	update_routing_table(addrToString(p.getDest()), cost, announcer_fd, myEthCard);
}

void Router::pass_unicast_data(Packet p)
{

	string dest = addrToString(p.getDest());
    string source = addrToString(p.getSource());
	cout<<"passing packet from source:"<<source<<" to dest:"<<dest<<" ..."<<endl;
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
        int next_hop_fd=routing_table[dest][indx].FD;
        p.send(routing_table[dest][indx].FD);   
        cout<<"I have indirect connection to "<<dest<<" and I send by my unicast routing table to fd = "<<next_hop_fd<<endl;
    }
    else
        throw Exeption("I dont know any path to send this packet to "+dest);
}

void Router::pass_multicast_data(Packet p)
{
	string source = addrToString(p.getSource());
    string multicast_IP = addrToString(p.getDest());
    
	cout<<"passing packet from source:"<<source<<" to multicast_IP:"<<multicast_IP<<" ..."<<endl;

	vector<string> dests = multicast_ip[multicast_IP];
	set<int> next_hop_fds;
	for(int i=0; i<dests.size(); i++)
	{
		string unicast_dest = dests[i];
		if(connected_client.find(unicast_dest) != connected_client.end())
	    {
	      	next_hop_fds.insert(connected_client[unicast_dest]);
	      	//p.send(connected_client[dest]);
	      	cout<<"I have direct connection to dest "<<unicast_dest<<" and I assume it as one of next hop fd"<<endl;
	    }
	    else if(routing_table.find(unicast_dest) != routing_table.end())
	    {
	        p.setTtl(p.getTtl()-1);
	        int indx=0;
	        int minCost=routing_table[unicast_dest][0].COST;
	        for (int i=0; i <routing_table[unicast_dest].size(); ++i)
	        {
	        	if(routing_table[unicast_dest][i].COST<minCost)
	        	{
	        		minCost=routing_table[unicast_dest][i].COST;
	        		indx=i;
	        	}
	        }
	        int next_hop_fd=routing_table[unicast_dest][indx].FD;
	        next_hop_fds.insert(next_hop_fd);
	        //p.send(routing_table[dest][indx].FD);   
	        cout<<"I have indirect connection to "<<unicast_dest<<" and I send by my multicast routing table to fd = "<<next_hop_fd<<endl;
	    }
	    else
	        throw Exeption("I dont know any path to send this packet to "+unicast_dest);
	}

	cout<<"Finish finding all next hops, start sending ..."<<endl;
	for(set<int> ::iterator it=next_hop_fds.begin(); it!=next_hop_fds.end(); it++)
		p.send(*it);
}

void Router::connect_client(Packet p, int client_fd)
{
	for(map<string, int> ::iterator it=connected_client.begin(); it!=connected_client.end(); it++)
        if(it->_FD == client_fd)
            throw Exeption("I recive a connect request from a clinent whitch I connected before");

    //assume that packet data is ip of client
    string client_ip=addrToString(p.getSource());
    connected_client[client_ip] = client_fd;

    /*p.setSource(address(router_port));
    p.setDest(stringToAddr(client_ip));
    p.setType(ACCEPT_CONNECTION);
    p.send(client_fd);
	*/

    cout<<"Broadcast every body that I connect to "<<client_ip<<" ..."<<endl;
    broadcast(client_ip, 1);
    cout<<"I connect to client "<<client_ip<<endl;
}

void Router::join(Packet p)
{
	string server_group_IP = addrToString(p.getSource());
	string multicast_IP = p.getDataStr();
	string joined_IP = addrToString(p.getDest());

	
	vector<string> ips = multicast_ip[multicast_IP];
	for(int i=0; i<ips.size(); i++)
		if(ips[i]==joined_IP)
			return;

	multicast_ip[multicast_IP].push_back(joined_IP);
	cout<<"I insert ip = "<<joined_IP<<" in the multicats ip = "<<multicast_IP<<endl;

	for(map<string, vector<eth_fd> > ::iterator it=eth_to_eth_fd.begin(); it!=eth_to_eth_fd.end(); ++it)
    {
    	vector<eth_fd> v =it->ETH_FDs;
    	p.setTtl(p.getTtl()-1);
    	for(int i=0; i<v.size(); i++)
    		p.send(v[i]._FD);
    }
	cout<<"finish broadcasting the join news"<<endl;

}

void Router::leave(Packet p)
{
	string server_group_IP = addrToString(p.getSource());
	string multicast_IP = p.getDataStr();
	string left_IP = addrToString(p.getDest());

	vector<string> ips = multicast_ip[multicast_IP];
	for(int i=0; i<ips.size(); i++)
		if(ips[i]==left_IP)
		{
			multicast_ip[multicast_IP].erase(multicast_ip[multicast_IP].begin()+i);
			cout<<"I remove ip = "<<left_IP<<" from the multicats ip = "<<multicast_IP<<endl;

			for(map<string, vector<eth_fd> > ::iterator it=eth_to_eth_fd.begin(); it!=eth_to_eth_fd.end(); ++it)
		    {
		    	vector<eth_fd> v =it->ETH_FDs;
		    	p.setTtl(p.getTtl()-1);
		    	for(int i=0; i<v.size(); i++)
		    		p.send(v[i]._FD);
		    }
			cout<<"finish broadcasting the leave news"<<endl;		
			return;
		}
}

void Router::parse_packet(Packet p, int client_fd)
{
	cout<<"I recive packet"<<endl;
	if(p.getTtl()==0) 
        return;
	if(p.getType()==UPDATE)
		update(p, client_fd);
	else if(p.getType()==DATA)
		pass_multicast_data(p);
	else if(p.getType()==ROUTER_CONNECT)
		accept_connection(p, client_fd);
	else if(p.getType()==CLIENT_CONNECT)
		connect_client(p, client_fd);
	else if(p.getType()==JOIN)
		join(p);
	else if(p.getType()==LEAVE)
		leave(p);
	else
	{
		pass_unicast_data(p);
		//throw Exeption("I recive a packet with unknown type");
	}
}

void Router::parse_cmd(string cmd, int& peer_fd)
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
			connectEth(myEthCard, peerEthCard, peer_listenPort, peer_fd);
		else
			throw Exeption("Invalid Oprand, Usage: Connect #my_EthernetCard #peer_EthernetCard #peer_listenPort");
	}
	else if(cmd_type=="Show")
	{
		show();
	}
	/*else if(cmd_type=="ChangeCost")
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
	else
		throw Exeption("Invalid Command");
	*/
}

void Router::show()
{
	cout<<"Multicast IP Table:"<<endl;
	for(map<string, vector<string>> ::iterator it=multicast_ip.begin(); it!=multicast_ip.end(); it++)
	{
		string multicast_ip=it->MULTICAST_IP;
		cout<<"\tmulticast ip = "<<multicast_ip<<" whitch include unicast ips:"<<endl;
		vector<string> v=it->UNICAST_IPs;
		for(int i=0; i<v.size(); i++)
			cout<<"\t\t"<<v[i]<<endl;
		cout<<endl;
	}
	cout<<endl<<endl;

	cout<<"Unicast Routing table:"<<endl;
	for(map<string, vector<eth_fd_cost> >  ::iterator it=routing_table.begin(); it!=routing_table.end(); it++)
	{
		string client_ip=it->CLIENT_IP;
		cout<<"\tclient ip = "<<client_ip<<" whitch I have diffrent path from below eth, fd and cost"<<endl;
		vector<eth_fd_cost> v=it->ETH_FD_COSTs;
		for (int i=0; i<v.size(); ++i)
			cout<<"\t\tfd = "<<v[i].FD<<" eth = "<<v[i].ETH<<" cost = "<<v[i].COST<<endl;
		cout<<endl;
	}
	cout<<endl<<endl;
	cout<<"Direct Connected IPs:"<<endl;
	for(map<string, int> ::iterator it=connected_client.begin(); it!=connected_client.end(); it++)
		cout<<"\tclient ip = "<<(it->CLIENT_IP)<<" fd "<<(it->CLIENT_FD)<<endl;
		
	cout<<endl<<endl;
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
	                    int peer_fd=-1;
	                    parse_cmd(cmd, peer_fd);
	                    if(peer_fd != -1)
	                    {
	                    	FD_SET(peer_fd, &router_fds);
	                   	 	if(peer_fd > max_fd)
	                        	max_fd = peer_fd;
                     	}
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

