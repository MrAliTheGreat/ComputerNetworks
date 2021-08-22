#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <vector>
#include <string>

#include "User.hpp"

#define MAX_CLIENTS 10

using namespace std;

class System
{
public:
	System();
	void addUserToSystem(User* newUser);
	void addAdminFileNameToSystem(string fileName);
	void printAllUsers();
	void printAdminFileNames();
	bool checkUserExistence(string username);
	void addToRequestedForLoginUsers(string username);
	bool checkRequestForLoginExistence();
	bool checkPasswordValidity(string password);
	void addToOnlineUsers(int index);
	void setCurrentDir(string new_dir , int userIndex);
	string getCurrentDir(int userIndex);
	void dirBackward(int userIndex);
	void resetDir(string new_dir , int userIndex);
	void logoutUser(int index);
	bool hasUserLoggedInBefore(string username);
	bool isUserOnline(int index);
	string getUserInfo(int index);
	bool handleDownloadFile(int fileSize , int userIndex);
	bool canAccessFile(int userIndex , string fileName);

private:
	vector<User*> allUsers;
	vector<string> adminFileNames;
	vector<User*> requestedForLoginUsers;
	User* onlineUsers[MAX_CLIENTS];
	string current_dir;
};

#endif