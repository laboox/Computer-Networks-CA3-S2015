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
	                    int port; cin>>port;
	                    string msg; cin>>msg;
	                    int test_sock;
	                    connect("localhost", port, &test_sock);
                        cout<<"connect: "<<test_sock<<endl;
        				send_message(msg, test_sock);
	                }
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
                        needfd[i] = socket_accept_fd;
                        i++;
	                    cout<<"accept: "<<socket_accept_fd<<endl;
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

