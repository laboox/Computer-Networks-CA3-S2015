Router::Router(int port)
{
	struct sockaddr_in router_address;
    router_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (router_fd < 0) 
        throw Exception("Problem in openning socket!");
    build_server(router_fd, &router_address, port); 
}

Router::run()
{
    fd_set router_fds, read_fds;
    FD_ZERO(&router_fds);
    FD_ZERO(&read_fds);
    FD_SET(0, &router_fds);
    FD_SET(router_fd, &router_fds);
    
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
	                    getline(cin, cmd);
	                    cout<<cmd;
	                    //parseCmd(cmd);
	                }
	                else if(client_fd!=router_fd)
	                {
	                    unsigned char packet[MAX_MSG_SIZE];
	                    read(client_fd, packet, MAX_MSG_SIZE);
	                    send_message(packet, client_fd);
	                    //send_message(result, client_fd);
	                    //FD_CLR(client_fd, &router_fds); DC
	                    /*
	                    if(!strcmp((char*)order,"DC"))
	                    {
	                        close(client_fd);
	                        FD_CLR(client_fd, &router_fds);
	                        cout<<"client disconnected!\n";
	                    }

	                    else 
	                    {
	                        string result=em.parseClientCmd((const char*)order, client_fd, caSockfd);
	                        if(result!="")
	                            send_message(result, client_fd);
	                    }*/
	                    
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
	                    
	                    //em.addBox(socket_accept_fd);
	                    //cout<<"new election center connected.\n";
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

