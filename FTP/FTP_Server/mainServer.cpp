#include <iostream>

#include <fstream>
#include <string>

#include "Server.hpp"

using namespace std;

string readJsonFile(string address){
	char c;
	string allFile;

	ifstream file(address);
	file >> noskipws;
	while ( file >> c ) allFile += c;
	file.close();

	return allFile;
}

int main(){
	json configJson = json::parse(readJsonFile("config.json"));
	Server* ftpServer = new Server(configJson["commandChannelPort"] , configJson["dataChannelPort"]);

	ftpServer->initializeServer(configJson);
	ftpServer->runServer();

	return 0;
}