#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>
#include <limits.h>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <ctime>

#include "json.hpp"
#include "User.hpp"
#include "System.hpp"

using namespace std;
using json = nlohmann::json;


class Server
{
public:
	Server(int commandChannelPort , int dataChannelPort);
	void runServer();
	void initializeServer(json configJson);

private:
	int makeNewSocket();
	sockaddr_in bindSocket(int socket , int port);
	void startListening(int socket);
	int initializeClientFD(fd_set& readfds , int* client_socket , int socket);
	void handleIncomingConnection(int command_socket , struct sockaddr_in cmd_addr , int cmd_addrlen , int* client_socket_cmd ,
								  int data_socket , struct sockaddr_in data_addr , int data_addrlen , int* client_socket_data);
	void handleClientIO(int* client_socket , fd_set readfds , struct sockaddr_in address , int addrlen ,
						int* client_socket_data , sockaddr_in data_addr , int data_addrlen);
	void SendResponseToClient(int cmd_fd , int data_fd , char* buffer, int clientIndex);

	void writeLog(string log);


	int commandChannelPort;
	int dataChannelPort;

	System* ftpBackEnd;
};

#endif