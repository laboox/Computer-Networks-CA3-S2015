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

void Eth(string ethCardNum)
{
	if(eth_cost.find(ethCardNum) != eth_cost.end())
		throw Exeption("Repeated EthCardNum");
	ethernet_cards.push_back(ethCardNum);
	eth_cost[ethCardNum]=DEFAULT_COST;
	cout<<"Ethernet Card "<<ethCardNum<<" added successfully";
}

void NoEth(string ethCardNum)
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
    
    if(fdsOfeth.find(myEthCard)==fdsOfeth.end())
    	throw Exeption("You must add this eth first");

    int connect_fd;
    connect("localhost", peer_listenPort, &connect_fd);
    

    Packet p;
    p.setType(CONNECT);
    p.setSource(address(router_port));
    p.setData(myEthCard+"\n"+peerEthCard);
    p.send(connect_fd);

    p.recive(connect_fd);
    if(p.getType()==ERROR)
    	throw Exeption(p.getDataStr());

    for(map<address, vector<eth_fd_cost> >::iterator it=routing_table.begin(); it!=routing_table.end(); ++it)
    {
    	vector<eth_fd_cost> v=routing_table[it->DEST];
    	int minCost=INFINITY;
    	for(int i=0; i<v.size(); i++)
    		minCost=min(minCost, v[i].COST);

        Packet p;
        p.setType(JOIN_UPDATE);
        //p.setSource(address(peerEthCard));
        p.setDest(address(it->DEST));
        p.setData(peerEthCard+"\n"+itoa(minCost));
        p.send(connect_fd);
    }

    fdsOfeth[myEthCard].push_back(connect_fd);
	
	cout<<myEthCard<<" connect to "<<peerEthCard<<" of port "<<peer_listenPort<<"successfully"<<endl;

	//TODO accept conection
}

void Router::acceptConnection(Packet p, int client_fd)
{
	string myEthCard, peerEthCard;
	int peer_listPort=p.getSource().u_long();
	string data=p.getDataStr();
	stringstream ss(data);
	getline(ss,peerEthCard);
	getline(ss,myEthCardNum);

	if(fdsOfeth.find(myEthCard)==fdsOfeth.end())
    {
    	Packet p;
    	p.setType(ERROR);
    	p.setData("port "+router_port+"has'nt any "+myEthCardNum);
    	p.send(client_fd);
    	return;
    }

	vector<int> v=fdsOfeth[myEthCard];
	for(int i=0; i<v.size(); i++)
		if(v[i]==client_fd)
			return;

	connectEth(myEthCard, peerEthCard, peer_listPort);
	cout<<"Connection accept with my "<<myEthCard<<" whith peer "<<peerEthCard<<" of peer port "<<peer_listPort<<endl;
}

void Router::pass_data(Packet p)
{
	//cout<<"passing packet from "<<p.getSource().to_string()<<" to "<<p.getDest().to_string()<<"."<<endl;
    string dest = p.getDest().to_string();
    if(connected_client.find(dest) != connected_client.end())
       p.send(connected_client[dest]);
    else if(routing_table.find(dest) != routing_table.end())
    {
        p.setTtl(p.getTtl()-1);
        p.send(routing_table[dest].FD);   
    }
    else
        throw Exeption("I dont know any path to send this packet");
}

void Router::join_update(Packet p, int client_fd)
{
	string myEthCard;
	int cost;
	string data=p.getDataStr();
	stringstream ss(data);
	getline(ss,myEthCard);
	getline(ss,cost);

	update_routing_table(p.getDest(),  cost, client_fd, myEthCard);
}

void Router::update(Packet p, int client_fd)
{
	update_routing_table(p.getDest(), p.getDataStr(), client_fd, 0);
}

void Router::update_and_broadcast(address dest, int cost)
{
	for(int j=0; j<ethernet_cards; j++)
	{
		for(int k=0; k<fdsOfeth[ethernet_cards[j]]; k++)
		{
			Packet p;
	        p.setType(UPDATE);
	        p.setDest(dest);
	        p.setData(itoa(cost));
	        p.send(fdsOfeth[ethernet_cards[j]][k]);
		}
	}
}

void Router::update_routing_table(address dest, int cost, int announcer_fd, string myEthCard)
{

	if(routing_table.find(dest)==routing_table.end())
	{
		routing_table[dest].push_back(eth_fd_cost(eth_fd(announcer_fd, myEthCard), cost));
		update_and_broadcast(dest, cost+1);
	}

	vector<eth_fd_cost> v=routing_table[dest];
	for(int i=0; i<v.size(); i++)
	{
		if(v[i].FD==announcer_fd && v[i].COST>=(cost+1)) //replace 1 with exact cost = max(eth1,eth2)
		{
			routing_table[dest][i].COST=cost+1;//replace lator with exact  cost;
			update_and_broadcast(dest, cost+1);
		}
	}
}

void Router::parse_packet(Packet p, int client_fd)
{
	if(p.getTtl()==0) 
        return;
	if(p.Type==UPDATE)
		update(p, clinet_fd);
	if(p.Type==UPDATE)
		joinUpdate(p, clinet_fd);
	else if(p.Type==DATA)
		pass_data(p);
	else if(p.Type==CONNECT)
		acceptConnection(p, client_fd);
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
		int peer_listen_port;
		if(ss >> myEthCard >> peerEthCard >> peer_listenPort)
			connectEth(myEthCardNum, peerEthCardNum, peer_listenPort);
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
	                    /*int port; cin>>port;
	                    string msg; cin>>msg;
	                    int test_sock;
	                    connect("localhost", port, &test_sock);
	                    cout<<test_sock;
        				send_message(msg, test_sock);*/
        			}
	                else if(client_fd!=router_fd)
	                {
	                	Packet p;
                		p.recive(client_fd);
                		parse_packet(p, client_fd);  
	                    //char packet[MAX_BUFFER_SIZE];
	                    //read(client_fd, packet, MAX_BUFFER_SIZE);
	                    //cout<<packet<<endl;
                        cout<<"connect: "<<test_sock<<endl;
        				send_message(msg, test_sock);
	                }
	                else if(client_fd!=router_fd)
	                {
                        Packet p;
	                    p.recive(client_fd);
                        p.send();
	                }
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
	                    // cout<<"accept: "<<socket_accept_fd<<endl;
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

