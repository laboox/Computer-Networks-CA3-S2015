class Router
{
public:
	Router(int port);
	~Router();

	void run();

private:
	int router_port;
	int router_fd;

	map<IP, fd> unicast_routing_table;
	map<IP, vector<IP> > multicats_routing_table;
	
	string ethernet_card_name;
	int cost;

};