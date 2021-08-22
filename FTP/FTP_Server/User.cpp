#include "User.hpp"
#include <iostream>

User::User(string _username , string _password , string _isAdmin , string _allowedFileSize){
	username = _username;
	password = _password;
	isAdmin = (_isAdmin == "true");
	allowedFileSize = std::stoi(_allowedFileSize);
	char temp[256];
	getcwd(temp, 256);
	string current_working_dir(temp);
	this->userDirectory = current_working_dir;
}


void User::printUserInfo(){
	cout << "username: " << this->username << "\n";
	cout << "password: " << this->password << "\n";
	cout << "isAdmin: " << this->isAdmin << "\n";
	cout << "allowedFileSize: " << this->allowedFileSize << "\n";
}

string User::get_username(){ return username; }

string User::get_password(){ return password; }

bool User::get_isAdmin(){ return isAdmin; }

int User::get_allowedFileSize(){ return allowedFileSize; }

void User::updateAllowedFileSize(int fileSize){
	this->allowedFileSize -= fileSize;
}

string User::getUserCurrentDirectory(){ return this->userDirectory; }

void User::setUserCurrentDirectory(string dir){
	this->userDirectory = dir;
}

void User::setCurrDirUser(string new_dir){
	this->userDirectory += "/";
	this->userDirectory += new_dir;
}
