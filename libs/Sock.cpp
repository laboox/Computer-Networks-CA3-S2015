#include "Sock.h"

void connect(char* ip, int port, int* socketfd){
	int char_number;
	struct sockaddr_in server_address;
	struct hostent *server;

	*socketfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &server_address, sizeof(server_address));
	if (*socketfd < 0) 
		throw Exeption("Server not found");
	server = gethostbyname(ip);
	if (server == 0)
		throw Exeption("Perver not found");
	server_address.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr,server->h_length);
	server_address.sin_port = htons(port);
	if (connect(*socketfd,(struct sockaddr *)&server_address,sizeof(server_address)) == -1) 
		throw Exeption("Connection error");
}

void send_message(string message, int sockfd){
    string buff;
    buff = message;
    if(buff.size()>MAX_BUFFER_SIZE)
        throw Exeption("Too long message");
    int nwrite = write(sockfd, buff.c_str(), MAX_BUFFER_SIZE);
    if(nwrite<=0){
        throw Exeption("Connection error");
    }
}

void build_server(int socketfd, struct sockaddr_in *server_address, int port_number){
	bzero((char *) server_address, sizeof(server_address));
	server_address->sin_family = AF_INET;
	server_address->sin_port = htons(port_number);
	server_address->sin_addr.s_addr = INADDR_ANY;

	if (bind(socketfd, (struct sockaddr *) server_address, sizeof(*server_address)) ==-1){ 
		throw Exeption("binding has problem!\n");
    }
	if(listen(socketfd,7)==-1)
		throw Exeption("problem in listening!");
}
