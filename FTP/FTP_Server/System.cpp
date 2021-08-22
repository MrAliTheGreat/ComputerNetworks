#include "System.hpp"
#include <iostream>

using namespace std;

System::System(){
	for(int i = 0 ; i < MAX_CLIENTS ; i++){
		this->onlineUsers[i] = NULL;
	}
}


void System::addUserToSystem(User* newUser){
	this->allUsers.push_back(newUser);
}


void System::addAdminFileNameToSystem(string fileName){
	this->adminFileNames.push_back(fileName);
}

void System::printAllUsers(){
	for(int i = 0 ; i < this->allUsers.size() ; i++){
		cout << "User " << i + 1 << " :\n";
		allUsers[i]->printUserInfo();
	}
}

void System::printAdminFileNames(){
	for(int i = 0 ; i < this->adminFileNames.size() ; i++){
		cout << "File " << i + 1 << " : " << adminFileNames[i] << "\n";
	}
}

bool System::checkUserExistence(string username){
	for(int i = 0 ; i < this->allUsers.size() ; i++){
		if(this->allUsers[i]->get_username() == username){
			return true;
		}
	}
	return false;
}

void System::addToRequestedForLoginUsers(string username){
	for(int i = 0 ; i < this->allUsers.size() ; i++){
		if(this->allUsers[i]->get_username() == username){
			this->requestedForLoginUsers.push_back(this->allUsers[i]);
		}
	}
}

bool System::checkRequestForLoginExistence(){
	return (this->requestedForLoginUsers.size() > 0);
}

bool System::checkPasswordValidity(string password){
	if(this->requestedForLoginUsers[this->requestedForLoginUsers.size() - 1]->get_password() == password){
		return true;
	}
	return false;
}

void System::addToOnlineUsers(int index){
	this->onlineUsers[index] = this->requestedForLoginUsers[this->requestedForLoginUsers.size() - 1];
	this->requestedForLoginUsers.erase(this->requestedForLoginUsers.begin() + this->requestedForLoginUsers.size() - 1);
}

void System::setCurrentDir(string new_dir , int userIndex){
	this->onlineUsers[userIndex]->setCurrDirUser(new_dir);
}

string System::getCurrentDir(int userIndex){
	return this->onlineUsers[userIndex]->getUserCurrentDirectory();
}

void System::dirBackward(int userIndex){
	current_dir = this->onlineUsers[userIndex]->getUserCurrentDirectory();
	char last;
	last = current_dir.back();
	while (last != '/')
	{
		current_dir.pop_back();
		last = current_dir.back();
	}
	current_dir.pop_back();
	this->onlineUsers[userIndex]->setUserCurrentDirectory(current_dir);

}
void System::resetDir(string new_dir , int userIndex){
	this->onlineUsers[userIndex]->setUserCurrentDirectory(new_dir);
}


void System::logoutUser(int index){
	this->onlineUsers[index] = NULL;
}


bool System::hasUserLoggedInBefore(string username){
	for(int i = 0 ; i < MAX_CLIENTS ; i++){
		if(this->onlineUsers[i] == NULL){
			continue;
		}

		if(this->onlineUsers[i]->get_username() == username){
			return true;
		}
	}

	return false;
}


bool System::isUserOnline(int index){
	if(this->onlineUsers[index] != NULL){
		return true;
	}

	return false;
}


string System::getUserInfo(int index){
	return this->onlineUsers[index]->get_username();
}


bool System::handleDownloadFile(int fileSize , int userIndex){
	if(fileSize <= this->onlineUsers[userIndex]->get_allowedFileSize()){
		this->onlineUsers[userIndex]->updateAllowedFileSize(fileSize);
		return true;
	}
	return false;
}


bool System::canAccessFile(int userIndex , string fileName){
	for(int i = 0 ; i < this->adminFileNames.size() ; i++){
		if(this->adminFileNames[i] == fileName){
			if(this->onlineUsers[userIndex]->get_isAdmin()){
				return true;
			}
			return false;
		}
	}
	return true;
}