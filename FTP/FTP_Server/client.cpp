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
#include <string>
#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "json.hpp"

using namespace std;
using json = nlohmann::json;
    
#define SERVER_ADDRESS "127.0.0.1" 
#define MAX_CLIENTS 10 

string readJsonFile(string address){
	char c;
	string allFile;

	ifstream file(address);
	file >> noskipws;
	while ( file >> c ) allFile += c;
	file.close();

	return allFile;
}

int makeNewSocket(){
	int newSocket , opt = 1;
	if((newSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0){ 
		write(1 , "Socket failed!" , 15); 
		exit(EXIT_FAILURE);
		return -1;
	}
    if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }		
	return newSocket;
}

sockaddr_in setAddress(sockaddr_in addr , int port){
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port);

	if(inet_pton(AF_INET, SERVER_ADDRESS , &addr.sin_addr) <= 0){ 
		write(1 , "\nInvalid address \n" , 19); 
		exit(EXIT_FAILURE);
	}

	return addr;
}

void connectToServer(int socket , sockaddr_in addr){
	if (connect(socket, (struct sockaddr *)&addr, sizeof(addr)) < 0){ 
		printf("\nConnection Failed \n"); 
		exit(EXIT_FAILURE); 
	}
}

vector<string> decodeCommand(string str){
	vector<string> cmd_components;
	istringstream ss(str);

	string word;
	while (ss >> word){
		cmd_components.push_back(word);
	}
	return cmd_components;
}


int main(){

	json configJson = json::parse(readJsonFile("config.json"));
	int commandChannelPort = configJson["commandChannelPort"];
	int dataChannelPort = configJson["dataChannelPort"];


	int command_socket = makeNewSocket();
	struct sockaddr_in command_addr = setAddress(command_addr , commandChannelPort);
	connectToServer(command_socket , command_addr);

	int data_socket = makeNewSocket();
	struct sockaddr_in data_addr = setAddress(data_addr , dataChannelPort);
	connectToServer(data_socket , data_addr);

	string message; 
	char buffer[1024];

	int valread;
	memset(buffer, 0, 1024);
	valread = read(command_socket , buffer, 1024); 
	write(1 , buffer , 1024);

	bool isCommandSocket;
	
	while(1){
		// Send command to server (command socket)
		message = "";
		memset(buffer, 0, 1024);
		write(1 , "YOU: " , 6);
		getline(cin, message);
		vector<string> cmd_components = decodeCommand(message);
		send(command_socket , &message[0] , message.length() , 0);

		isCommandSocket = (cmd_components[0] == "ls" || cmd_components[0] == "retr") ? false : true;

		if(isCommandSocket){
			// Get command result (command socket)
			if (message == "help"){
				memset(buffer, 0, 1024);
				valread = read(command_socket , buffer, 1024); 
				write(1 , buffer , 1024);		
			}
			memset(buffer, 0, 1024);
			valread = read(command_socket , buffer, 1024); 
			write(1 , buffer , 1024);
			
			if(message == "exit"){
				break;
			}
		}
		else{
			memset(buffer, 0, 1024);
			valread = read(data_socket , buffer, 1024);
			write(1 , buffer , 1024);
			memset(buffer, 0, 1024);
			valread = read(command_socket , buffer, 1024); 
			write(1 , buffer , 1024);
		}
	} 

	return 0;
}