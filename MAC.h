#pragma once
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <Windows.h>
#include <iphlpapi.h>
#include <tchar.h>
#include <netcon.h>

#undef max

#pragma comment(lib, "IPHLPAPI.lib")

class MAC
{
public:
	MAC();
	~MAC();

	std::string GenRandomMAC();
	void ShowAdapterList();
	std::unordered_map<std::string, std::string> GetAdapters();
	void AssingRandomMAC();
	HRESULT DisableEnableConnections(BOOL Enable, const wchar_t* AdapterName);
};
