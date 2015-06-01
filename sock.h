#ifndef SCOK
#define SOCK

#include "primary_header.h"
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_CLIENT 10
#define MAX_BUFFER_SIZE 2048

void build_server(int socketfd, struct sockaddr_in *server_address, int port_number);
void connect(char* ip, char* port, int* socketfd);
void send_message(string message, int sockfd);

#endif
