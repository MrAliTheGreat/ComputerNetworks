#ifndef __USER_H__
#define __USER_H__

#include <string>
#include <unistd.h>

using namespace std;

class User
{
public:
	User(string _username , string _password , string _isAdmin , string _allowedFileSize);
	string get_username();
	string get_password();
	bool get_isAdmin();
	int get_allowedFileSize();
	void printUserInfo();
	void updateAllowedFileSize(int fileSize);
	void setUserCurrentDirectory(string dir);
	string getUserCurrentDirectory();
	void setCurrDirUser(string new_dir);

private:
	string username;
	string password;
	bool isAdmin;
	int allowedFileSize;
	string userDirectory;
};

#endif