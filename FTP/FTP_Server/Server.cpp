#include "Server.hpp"

#define SERVER_ADDRESS "127.0.0.1" 
#define MAX_CLIENTS 10

char* itoa_recursive(char *dest, int i) {
	if (i <= -10){
		dest = itoa_recursive(dest, i/10);
	}
	*dest++ = '0' - i%10;
	return dest;
}

char *itoa_main(char *dest, int i) {
	char *s = dest;
	if (i < 0) {
		*s++ = '-';
	} else {
		i = -i;
	}
	*itoa_recursive(s, i) = '\0';
	return dest;

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



Server::Server(int commandChannelPort , int dataChannelPort){
	this->commandChannelPort = commandChannelPort;
	this->dataChannelPort = dataChannelPort;
	this->ftpBackEnd = new System();
}


int Server::makeNewSocket(){
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


sockaddr_in Server::bindSocket(int socket , int port){
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS); 
	address.sin_port = htons( port );

	if (bind(socket, (struct sockaddr *)&address, sizeof(address)) < 0){ 
		write(1 , "bind failed" , 12); 
		exit(EXIT_FAILURE);
	}
	return address;
}


void Server::startListening(int socket){
	//Maximum of 3 pending connections 
	if(listen(socket, 3) < 0){ 
		write(1 , "listen" , 7); 
		exit(EXIT_FAILURE); 
	}
}


int Server::initializeClientFD(fd_set& readfds , int* client_socket , int socket){
	int max_sd , sd;
	FD_ZERO(&readfds); 
	FD_SET(socket, &readfds); 
	max_sd = socket; 
	    
	for (int i = 0 ; i < MAX_CLIENTS ; i++){  
		sd = client_socket[i]; 
		     
		if(sd > 0){
			FD_SET(sd , &readfds); 
		}
		     
		if(sd > max_sd){
			max_sd = sd; 
		}
	}
	return max_sd;
}


void Server::handleIncomingConnection(int command_socket , struct sockaddr_in cmd_addr , int cmd_addrlen , int* client_socket_cmd ,
									  int data_socket , struct sockaddr_in data_addr , int data_addrlen , int* client_socket_data){
	int new_socket_cmd;
	char new_socket_s[256] , addr_port_s[256] , i_s[256];

	if ((new_socket_cmd = accept(command_socket, (struct sockaddr *)&cmd_addr, (socklen_t*)& cmd_addrlen)) < 0){ 
		write(1 , "accept" , 7); 
		exit(EXIT_FAILURE); 
	}

	int new_data_socket;
	if ((new_data_socket = accept(data_socket, (struct sockaddr *)&data_addr, (socklen_t*)& data_addrlen)) < 0){ 
		write(1 , "accept" , 7); 
		exit(EXIT_FAILURE); 
	}	
	
	write(1 , "--------------------\n" , 21); 
	write(1 , "New connection , socket fd is " , 31);
	write(1 , itoa_main(new_socket_s , new_socket_cmd) , strlen(itoa_main(new_socket_s , new_socket_cmd)));
	write(1 , " , ip is: " , 11);
	write(1 , inet_ntoa(cmd_addr.sin_addr) , strlen(inet_ntoa(cmd_addr.sin_addr)));
	write(1 , " , port : " , 11);
	write(1 , itoa_main(addr_port_s , ntohs(cmd_addr.sin_port)) , strlen(itoa_main(addr_port_s , ntohs(cmd_addr.sin_port))));
	write(1 , "\n" , 2); 

	if(send(new_socket_cmd, "You're now live! \r\n", strlen("You're now live! \r\n"), 0) != strlen("You're now live! \r\n")){ 
		write(1 , "send" , 5); 
	} 
         
	for (int i = 0; i < MAX_CLIENTS; i++){ 
		if( client_socket_cmd[i] == 0){ 
			client_socket_cmd[i] = new_socket_cmd; 
			write(1 , "Adding to list of sockets as " , 30);
			write(1 , itoa_main(i_s , i) , strlen(itoa_main(i_s , i)));
			write(1 , "\n" , 2);
			write(1 , "--------------------\n" , 22); 
			break; 
		} 
	}

	for (int i = 0; i < MAX_CLIENTS; i++){ 
		if( client_socket_data[i] == 0){ 
			client_socket_data[i] = new_data_socket;
			break; 
		} 
	}	
}


void Server::handleClientIO(int* client_socket_cmd , fd_set readfds , struct sockaddr_in address , int addrlen ,
							int* client_socket_data , sockaddr_in data_addr , int data_addrlen){
	int sd , valread;
    char buffer[1025];
    char addr_port_s[256];
    char addr_port_s_data[256];

	for (int i = 0; i < MAX_CLIENTS; i++){ 
	    sd = client_socket_cmd[i]; 
	    if (FD_ISSET(sd , &readfds)){
	        if((valread = read( sd , buffer, 1024)) == 0){ 
	            getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
	            write(1 , "++++++++++++++++++++\n" , 21);
	            write(1 , "Client disconnected , ip " , 24);
	            write(1 , inet_ntoa(address.sin_addr) , strlen(inet_ntoa(address.sin_addr)));
	            write(1 , " , port " , 9);
	            write(1 , itoa_main(addr_port_s , ntohs(address.sin_port)) , strlen(itoa_main(addr_port_s , ntohs(address.sin_port))));
	            write(1 , "\n" , 2);
	            write(1 , "++++++++++++++++++++\n" , 21);  

	            close(sd); 
	            close(client_socket_data[i]);
	            client_socket_cmd[i] = 0;
	            client_socket_data[i] = 0;
	        }   
	        else{
	            buffer[valread] = '\0';

	            string buffer_str(buffer);
				vector<string> cmd_components = decodeCommand(buffer_str);

				if(strcmp(buffer , "ls") != 0 && cmd_components[0]!="retr"){
		            getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
		            write(1 , "CLIENT port " , 13);
		            write(1 , itoa_main(addr_port_s , ntohs(address.sin_port)) , strlen(itoa_main(addr_port_s , ntohs(address.sin_port))));
		            write(1 , ": " , 3);
		            write(1 , buffer , strlen(buffer));
		            write(1 , "\n" , 2);
		        }else{
		            getpeername(client_socket_data[i] , (struct sockaddr*)&data_addr , (socklen_t*)&data_addrlen);
		            write(1 , "CLIENT port " , 13);
		            write(1 , itoa_main(addr_port_s_data , ntohs(data_addr.sin_port)) , strlen(itoa_main(addr_port_s_data , ntohs(data_addr.sin_port))));
		            write(1 , ": " , 3);
		            write(1 , buffer , strlen(buffer));
		            write(1 , "\n" , 2);		        	
		        }

            	SendResponseToClient(sd , client_socket_data[i] , buffer , i);

            	if(strcmp(buffer , "exit") == 0){
		            getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
		            write(1 , "++++++++++++++++++++\n" , 21);
		            write(1 , "Client disconnected , ip " , 24);
		            write(1 , inet_ntoa(address.sin_addr) , strlen(inet_ntoa(address.sin_addr)));
		            write(1 , " , port " , 9);
		            write(1 , itoa_main(addr_port_s , ntohs(address.sin_port)) , strlen(itoa_main(addr_port_s , ntohs(address.sin_port))));
		            write(1 , "\n" , 2);
		            write(1 , "++++++++++++++++++++\n" , 21); 

		            close(sd); 
		            close(client_socket_data[i]);
		            client_socket_cmd[i] = 0;
		            client_socket_data[i] = 0;
            	}
	        } 
	    } 
	} 
}

void Server::SendResponseToClient(int cmd_fd , int data_fd , char* buffer , int clientIndex){
	string response_str = "";
	string log = "";
	string buffer_str(buffer);

	vector<string> cmd_components = decodeCommand(buffer_str);
	
	if(cmd_components.size() == 2 && cmd_components[0] == "user"){
		if(!this->ftpBackEnd->hasUserLoggedInBefore(cmd_components[1])){
			if(this->ftpBackEnd->checkUserExistence(cmd_components[1])){
				this->ftpBackEnd->addToRequestedForLoginUsers(cmd_components[1]);

				response_str += "331: User name okay, need password.\n";
				send(cmd_fd, &response_str[0] , response_str.length(), 0);
			}
			else{
				response_str += "430: Invalid username or password\n";
				send(cmd_fd, &response_str[0] , response_str.length(), 0);			
			}
		}
		else{
			response_str += "430: User is online no need for logging in!\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}
	
	}

	else if(cmd_components.size() == 2 && cmd_components[0] == "pass"){
		if(this->ftpBackEnd->checkRequestForLoginExistence()){
			if(this->ftpBackEnd->checkPasswordValidity(cmd_components[1])){
				this->ftpBackEnd->addToOnlineUsers(clientIndex);

				response_str += "230: User logged in, proceed. Logged out if appropriate.\n";
				send(cmd_fd, &response_str[0] , response_str.length(), 0);

				log += this->ftpBackEnd->getUserInfo(clientIndex);
				log += " logged in!\n";
				this->writeLog(log);
			}
			else{
				response_str += "430: Invalid username or password\n";
				send(cmd_fd, &response_str[0] , response_str.length(), 0);
			}
		}
		else{
			response_str += "503: Bad sequence of commands.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}
	}

    else if(cmd_components[0] == "pwd"){
    	if(this->ftpBackEnd->isUserOnline(clientIndex)){
	    	response_str += "257: ";
			response_str += this->ftpBackEnd->getCurrentDir(clientIndex);
			response_str += "\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);

			log += this->ftpBackEnd->getUserInfo(clientIndex);
			log += " called pwd command!\n";
			this->writeLog(log);				
    	}
    	else{
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);	    		
    	}
    }

	else if(cmd_components[0] == "mkd"){
		if(this->ftpBackEnd->isUserOnline(clientIndex)){
			response_str += "257: ";

			char* new_dir;
			string dir_name("");
			dir_name += this->ftpBackEnd->getCurrentDir(clientIndex);
			dir_name += "/";
			dir_name += cmd_components[1];
			new_dir = &dir_name[0];

			if (mkdir(new_dir, 0777)){
				send(cmd_fd, "An error accured during making new directory.\n" , strlen("An error accured during making new directory.\n"), 0);
				cerr << "Error :  " << strerror(errno) << endl;
			}else{
				response_str += cmd_components[1];
				response_str += " created.\n";
				send(cmd_fd, &response_str[0] , response_str.length(), 0);

				log += this->ftpBackEnd->getUserInfo(clientIndex);
				log += " called mkd command!";
				log += " created directory: ";
				log += cmd_components[1];
				log += "\n";
				this->writeLog(log);
			}
		}
    	else{
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);	    		
    	}
    }

	else if(cmd_components[0] == "dele"){
		if(this->ftpBackEnd->isUserOnline(clientIndex)){
			if(cmd_components[1] == "-f"){
				if(this->ftpBackEnd->canAccessFile(clientIndex , cmd_components[2])){
					response_str += "250: ";
					int status;
					string correct_name = this->ftpBackEnd->getCurrentDir(clientIndex) + "/" + cmd_components[2];					
					char fileName[correct_name.length() + 1];
					strcpy(fileName, correct_name.c_str());		
					status = remove(fileName);
					if(status==0){
						response_str += fileName;
						response_str += " deleted.\n";
						send(cmd_fd, &response_str[0] , response_str.length(), 0);

						log += this->ftpBackEnd->getUserInfo(clientIndex);
						log += " called dele -f command!";
						log += " deleted file: ";
						log += cmd_components[2];
						log += "\n";
						this->writeLog(log);					
					}
					else{		
						send(cmd_fd, "An error accured during deleting new file.\n" , strlen("An error accured during deleting new file.\n"), 0);
						cout<<"\nErorr Occurred!";
					}
				}
				else{
					send(cmd_fd, "550: File unavailable.\n" , strlen("550: File unavailable.\n"), 0);
				}
			}
			else if(cmd_components[1] == "-d"){
				response_str += "250: ";
				int status;
				char fileName[cmd_components[2].length() + 1];
				strcpy(fileName, cmd_components[2].c_str());		
				status = rmdir(fileName);
				if(status!=-1){
					response_str += fileName;
					response_str += " deleted.\n";
					send(cmd_fd, &response_str[0] , response_str.length(), 0);

					log += this->ftpBackEnd->getUserInfo(clientIndex);
					log += " called dele -d command!";
					log += " deleted directory: ";
					log += cmd_components[2];
					log += "\n";
					this->writeLog(log);					
				}
				else{	
					send(cmd_fd, "An error accured during deleting new directory.\n" , strlen("An error accured during deleting new directory.\n"), 0);
	      			cerr << "Error: " << strerror(errno) << endl;
	      		}
		  	}
		  	else{
				send(cmd_fd, "501: Syntax error in parameters or arguments.\n" , strlen("501: Syntax error in parameters or arguments.\n"), 0);
			}
		}
		else{
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}
    }

	else if(cmd_components[0] == "rename"){
		if(this->ftpBackEnd->isUserOnline(clientIndex)){
			if(this->ftpBackEnd->canAccessFile(clientIndex , cmd_components[1])){
				response_str += "250: ";
				string curr_dir("");
				string new_name("");
				curr_dir += this->ftpBackEnd->getCurrentDir(clientIndex);
				struct stat buffer;
				int new_dir_exist;
				curr_dir += "/";
				new_name += curr_dir;
				curr_dir += cmd_components[1];
				new_dir_exist = (stat (curr_dir.c_str(), &buffer) == 0);
				new_name += cmd_components[2];
				if (new_dir_exist){

					char char_array[curr_dir.length() + 1];
					strcpy(char_array, curr_dir.c_str());

					char char_array2[new_name.length() + 1];
					strcpy(char_array2, new_name.c_str());

					rename(char_array, char_array2);
					response_str += "Successful change.\n";
					send(cmd_fd, &response_str[0] , response_str.length(), 0);

					log += this->ftpBackEnd->getUserInfo(clientIndex);
					log += " called rename command!";
					log += " renamed ";
					log += cmd_components[1];
					log += " to ";
					log += cmd_components[2];
					log += "\n";
					this->writeLog(log);					
				}
				else{
					response_str += "Unsuccessful change.\n";
					send(cmd_fd, &response_str[0] , response_str.length(), 0);
				}
			}
			else{
				send(cmd_fd, "550: File unavailable.\n" , strlen("550: File unavailable.\n"), 0);
			}
		}
		else{
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}
    }

	else if(cmd_components[0] == "help"){
		response_str += "214\n";
		response_str += "#1 USER [name], Its argument is used to specify the user's string. It is used for user authentication.\n";
		response_str += "#2 PASS [password], Its argument is password for user's account. It is used for logging in.\n";
		response_str += "#3 PWD , It prints your current directory.\n";
		response_str += "#4 MKD [new name] , It is used to making a new directory. Its argument is new directory name.\n";
		response_str += "#5 dele -f [file name] , It is used for deleting a file. Its argument is the name of file user wants to delete.\n";
		response_str += "#6 dele -d [directory name] , It is used for deleting a directory. Its argument is the name of directory user wants to delete.\n";
		response_str += "#7 ls , It is used for showing list of files in current directory.\n";
		send(cmd_fd, &response_str[0] , response_str.length(), 0);
		response_str = "";
		response_str += "#8 CWD [Directory name] , It is used for changing directory. Its argument is the name of directory user wants to enter.\n";
		response_str += "#9 CWD .. , It is used for changing directory(backward).\n";
		response_str += "#10 CWD , It is used for reseting directory.\n";
		response_str += "#11 rename [old name] [new name]  , It is used for renaming a file. Its arguments are old and new name of destination file.\n";
		response_str += "#12 retr [name]] , It is used for downloading a file. Its argument is the name of file user wants to download.\n";
		response_str += "#13 quit , It is used for logging out.\n";
		response_str += "#14 help, recursively read all the document shits!\n";
		
		send(cmd_fd, &response_str[0] , response_str.length(), 0);
    }

	else if(cmd_components[0] == "cwd"){
		if(this->ftpBackEnd->isUserOnline(clientIndex)){
			string curr_dir("");
			curr_dir += this->ftpBackEnd->getCurrentDir(clientIndex);
		

			struct stat buffer;
			int new_dir_exist;
			curr_dir += "/";
			curr_dir += cmd_components[1];
			new_dir_exist = (stat (curr_dir.c_str(), &buffer) == 0);
			response_str += "250: ";
			if (cmd_components.size() == 1){

					char temp[256];
					getcwd(temp, 256);
					string current_working_dir(temp);
					this->ftpBackEnd->resetDir(current_working_dir , clientIndex);

				
					response_str += "Successful change.\n";
					send(cmd_fd, &response_str[0] , response_str.length(), 0);

					log += this->ftpBackEnd->getUserInfo(clientIndex);
					log += " called cwd command!";
					log += " went to base directory";
					log += "\n";
					this->writeLog(log);						
			}else 
			if ( cmd_components[1] == ".." ){
				this->ftpBackEnd->dirBackward(clientIndex);

				response_str += "Successful change.\n";
				send(cmd_fd, &response_str[0] , response_str.length(), 0);

				log += this->ftpBackEnd->getUserInfo(clientIndex);
				log += " called cwd command!";
				log += " went to previous directory";
				log += "\n";
				this->writeLog(log);

			}else{
				if (new_dir_exist){
					this->ftpBackEnd->setCurrentDir(cmd_components[1] , clientIndex);
					response_str += "Successful change.\n";
					send(cmd_fd, &response_str[0] , response_str.length(), 0);
					curr_dir = "";

					log += this->ftpBackEnd->getUserInfo(clientIndex);
					log += " called cwd command!";
					log += " went to ";
					log += cmd_components[1];
					log += " directory";
					log += "\n";
					this->writeLog(log);					
				}else{
					response_str += "Unsuccessful change.\n";
					send(cmd_fd, &response_str[0] , response_str.length(), 0);
				}
			}
		}
		else{
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}
    }

    else if(cmd_components[0] == "ls"){
    	if(this->ftpBackEnd->isUserOnline(clientIndex)){
			string current_working_dir(this->ftpBackEnd->getCurrentDir(clientIndex));

			struct dirent *entry;
			char* path;
			path = &current_working_dir[0];
			DIR *dir = opendir(path);
			
			if (dir == NULL) {
				return;
			}
			string dr;
			string finalDir = "";
			while ((entry = readdir(dir)) != NULL) {
				dr += entry->d_name;
				dr += '\n';
				finalDir += dr;
				dr = "";
			}
			closedir(dir);
			send(data_fd , &finalDir[0] , finalDir.length() , 0);
			
			send(cmd_fd, "226: List transfer done.\n" , strlen("501: List transfer done.\n"), 0);

			log += this->ftpBackEnd->getUserInfo(clientIndex);
			log += " called ls command!\n";
			this->writeLog(log);
		}
		else{
			send(data_fd , "___________\n" , strlen("___________\n") , 0);
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}

    }

    else if(cmd_components[0] == "exit"){
    	if(this->ftpBackEnd->isUserOnline(clientIndex)){
	    	this->ftpBackEnd->logoutUser(clientIndex);
	    }
	    send(cmd_fd, "221: Successful Exit.\n" , strlen("221: Successful Exit.\n"), 0);

		log += this->ftpBackEnd->getUserInfo(clientIndex);
		log += " left the server!\n";
		this->writeLog(log);	    
    }

    else if(cmd_components[0] == "quit"){
    	if(this->ftpBackEnd->isUserOnline(clientIndex)){
	     	this->ftpBackEnd->logoutUser(clientIndex);
	    	send(cmd_fd, "221: Successful Quit.\n" , strlen("221: Successful Quit.\n"), 0);
			log += this->ftpBackEnd->getUserInfo(clientIndex);
			log += " logged out!\n";
			this->writeLog(log);	    			
    	}else{
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);	    		
    	}
    }

	else if(cmd_components[0] == "retr"){
		if(this->ftpBackEnd->isUserOnline(clientIndex)){
			string file_addr(this->ftpBackEnd->getCurrentDir(clientIndex));

			file_addr += "/";
			file_addr += cmd_components[1];

			struct stat buffer_file;   
  			if((stat (file_addr.c_str(), &buffer_file) == 0)){
  				if(this->ftpBackEnd->canAccessFile(clientIndex , cmd_components[1]) ){
					string line;
					ifstream ini_file {file_addr};

					ini_file.seekg(0, ios::end);
		   			int file_size = ini_file.tellg();

					ini_file.clear();
					ini_file.seekg(0);
		   			if(this->ftpBackEnd->handleDownloadFile(file_size , clientIndex)){
		   				string finalStr = "";
						while(getline(ini_file,line)){
							line += "\n";
							finalStr += line;
						}
						send(data_fd , &finalStr[0] , finalStr.length() , 0);

						send(cmd_fd, "226: Successful Download.\n" , strlen("226: Successful Download.\n"), 0);

						log += this->ftpBackEnd->getUserInfo(clientIndex);
						log += " downloaded file: ";
						log += cmd_components[1];
						log += "\n";
						this->writeLog(log);
					}
					else{
						send(data_fd , "___________\n" , strlen("___________\n") , 0);
						send(cmd_fd, "500: Error no remaining user file size\n" , strlen("500: Error no remaining user file size\n"), 0);
					}
				}
				else{
					send(data_fd , "___________\n" , strlen("___________\n") , 0);
					send(cmd_fd, "550: File unavailable.\n" , strlen("550: File unavailable.\n"), 0);
				}
  			}else{
  				send(data_fd , "___________\n" , strlen("___________\n") , 0);
				send(cmd_fd, "500: Error file not found\n" , strlen("500: Error file not found\n"), 0);  				
  			}

		}
		else{
			send(data_fd , "___________\n" , strlen("___________\n") , 0);
			response_str += "332: Need account for login.\n";
			send(cmd_fd, &response_str[0] , response_str.length(), 0);			
		}

	}       

    else{
    	response_str += "501: Syntax error in parameters or arguments.\n";
    	send(cmd_fd, &response_str[0] , response_str.length(), 0);
    }
}

void Server::runServer(){
	int command_socket , data_socket;
       
    int client_socket_cmd[MAX_CLIENTS]; 
    for (int i = 0; i < MAX_CLIENTS; i++){ 
        client_socket_cmd[i] = 0; 
    }

    int client_socket_data[MAX_CLIENTS]; 
    for (int i = 0; i < MAX_CLIENTS; i++){ 
        client_socket_data[i] = 0; 
    }


    command_socket = makeNewSocket();
    data_socket = makeNewSocket();

	struct sockaddr_in command_addr = bindSocket(command_socket , this->commandChannelPort);
	struct sockaddr_in data_addr = bindSocket(data_socket , this->dataChannelPort);
	int command_addrlen = sizeof(command_addr);
	int data_addrlen = sizeof(data_addr); 

	char port_cmd_s[256]; itoa_main(port_cmd_s , this->commandChannelPort);
	char port_data_s[256]; itoa_main(port_data_s , this->dataChannelPort);
	write(1 , "Listener on port ", 18);
	write(1 , port_cmd_s , strlen(port_cmd_s));
	write(1 , " for command ", 14);
	write(1 , port_data_s , strlen(port_data_s));
	write(1 , " for data", 10);
	write(1 , "\n" , 2);

	startListening(command_socket);
	startListening(data_socket);

	write(1 , "Waiting for connections ..." , 28);
	write(1 , "\n" , 2); 
    
	int max_sd_cmd;
	fd_set readfds_cmd;

	int max_sd_data;
	fd_set readfds_data;

	while(1){
		max_sd_cmd = initializeClientFD(readfds_cmd , client_socket_cmd , command_socket);
		max_sd_data = initializeClientFD(readfds_data , client_socket_data , data_socket);

		int activity_cmd = select( max_sd_cmd + 1 , &readfds_cmd , NULL , NULL , NULL);

		if ((activity_cmd < 0) && (errno != EINTR)){ 
			write(1 , "select error" , 13); 
		}

		// Something happend on command_socket --> Incoming connection
		if (FD_ISSET(command_socket, &readfds_cmd)){ 
			handleIncomingConnection(command_socket , command_addr , command_addrlen , client_socket_cmd , 
									 data_socket , data_addr , data_addrlen , client_socket_data);
		}

		// Something happend on one of the clients
		handleClientIO(client_socket_cmd , readfds_cmd , command_addr , command_addrlen , client_socket_data , data_addr , data_addrlen);
	}
}

void Server::initializeServer(json configJson){
	for(int i = 0 ; i < configJson["users"].size() ; i++){
		User* newUser = new User(configJson["users"][i]["user"] , configJson["users"][i]["password"] ,
								 configJson["users"][i]["admin"] , configJson["users"][i]["size"]);

		ftpBackEnd->addUserToSystem(newUser);
	}

	for(int i = 0 ; i < configJson["files"].size() ; i++){
		ftpBackEnd->addAdminFileNameToSystem(configJson["files"][i]);
	}

}

void Server::writeLog(string log){
	time_t now = time(0);
	char* dt = ctime(&now);
	string timeDateStr(dt);

	string finalLog = timeDateStr;
	finalLog += "==> ";
	finalLog += log;
	finalLog += "\n";

	std::ofstream logFile;
	logFile.open("log.txt", std::ios_base::app);
	logFile << finalLog;	
}
