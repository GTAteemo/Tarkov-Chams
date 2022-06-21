#include "MAC.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

MAC::MAC()
{
	srand((unsigned)time(0));
}

MAC::~MAC()
{
}

std::string MAC::GenRandomMAC()
{
	std::stringstream temp;
	int number = 0;
	std::string result;

	for (int i = 0; i < 6; i++)
	{
		number = rand() % 254;
		temp << std::setfill('0') << std::setw(2) << std::hex << number;
		if (i != 5)
		{
			temp << "-";
		}
	}
	result = temp.str();

	for (auto& c : result)
	{
		c = toupper(c);
	}

	return result;
}

void MAC::ShowAdapterList()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		std::cerr << "Error allocating memory needed to call GetAdaptersinfo." << std::endl;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			std::cerr << "Error allocating memory needed to call GetAdaptersinfo" << std::endl;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			std::cout << "\n\tComboIndex: \t" << pAdapter->ComboIndex << std::endl;
			std::cout << "\tAdapter Name: \t" << pAdapter->AdapterName << std::endl;
			std::cout << "\tAdapter Desc: \t" << pAdapter->Description << std::endl;
			std::cout << "\tAdapter Addr: \t";
			for (i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
					printf("%.2X\n", (int)pAdapter->Address[i]);
				else
					printf("%.2X-", (int)pAdapter->Address[i]);
			}
			std::cout << "\tIP Address: \t" << pAdapter->IpAddressList.IpAddress.String << std::endl;
			std::cout << "\tIP Mask: \t" << pAdapter->IpAddressList.IpMask.String << std::endl;
			std::cout << "\tGateway: \t" << pAdapter->GatewayList.IpAddress.String << std::endl;
			pAdapter = pAdapter->Next;
		}
	}
	else {
		std::cerr << "GetAdaptersInfo failed with error: " << dwRetVal << std::endl;
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
}

std::unordered_map<std::string, std::string> MAC::GetAdapters()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	std::unordered_map<std::string, std::string> result;
	std::stringstream temp;
	std::string str_mac;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		std::cerr << "Error allocating memory needed to call GetAdaptersinfo" << std::endl;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			std::cerr << "Error allocating memory needed to call GetAdaptersinfo\n" << std::endl;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			for (UINT i = 0; i < pAdapter->AddressLength; i++) {
				temp << std::setfill('0') << std::setw(2) << std::hex << (int)pAdapter->Address[i];
				if (i != pAdapter->AddressLength - 1)
				{
					temp << "-";
				}
			}
			str_mac = temp.str();
			temp.str("");
			temp.rdbuf();
			for (auto& c : str_mac)
			{
				c = toupper(c);
			}

			result.insert({ pAdapter->Description, str_mac });
			pAdapter = pAdapter->Next;
		}
	}
	else {
		std::cerr << "GetAdaptersInfo failed with error: " << dwRetVal << std::endl;
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return result;
}

void MAC::AssingRandomMAC()
{
	std::vector <std::string> list;
	std::unordered_map<std::string, std::string> AdapterDetails = GetAdapters();
	for (auto& itm : AdapterDetails)
	{
		list.push_back(itm.first);
	}

	std::cout << "\n[+]List of Available Adapters: " << std::endl;
	int range = 0;
	for (auto itm = list.begin(); itm != list.end(); itm++)
	{
		std::cout << '\t' << range + 1 << ")" << *itm << std::endl;
		range++;
	}

	std::cout << "[*]Selection: ";

	int selection = 0;
	std::cin >> selection;
	if (std::cin.fail() || (selection < 1) || (selection > range))
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cerr << "[!]Invalid Selection Input!" << std::endl;
		return;
	}

	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "[-]Selected Adapter is: " << list.at(selection - 1) << std::endl;
	std::cout << "[+]Old MAC: " << AdapterDetails.at(list.at(selection - 1)) << std::endl;

	std::wstring wstr(list.at(selection - 1).begin(), list.at(selection - 1).end());
	const wchar_t* wAdapterName = wstr.c_str();

	bool bRet = false;
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}"),
		0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{

		DWORD dwIndex = 0;
		TCHAR Name[1024];
		DWORD cName = 1024;
		while (RegEnumKeyEx(hKey, dwIndex, Name, &cName,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hSubKey = NULL;
			if (RegOpenKeyEx(hKey, Name, 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS)
			{
				BYTE Data[1204];
				DWORD cbData = 1024;
				if (RegQueryValueEx(hSubKey, _T("DriverDesc"), NULL, NULL, Data, &cbData) == ERROR_SUCCESS)
				{

					if (_tcscmp((TCHAR*)Data, (const char*)wAdapterName) == 0)
					{
						std::string temp = GenRandomMAC();
						std::string newMAC = temp;
						temp.erase(std::remove(temp.begin(), temp.end(), '-'), temp.end());

						std::wstring wstr_newMAC(temp.begin(), temp.end());
						const wchar_t* newMACAddr = wstr_newMAC.c_str();

						if (RegSetValueEx(hSubKey, _T("NetworkAddress"), 0, REG_SZ,
							(const BYTE*)newMACAddr, sizeof(TCHAR) * ((DWORD)_tcslen((const char*)newMACAddr) + 1)) == ERROR_SUCCESS)
						{
							std::cout << "[+]New Random MAC: " << newMAC << std::endl;
							DisableEnableConnections(false, wAdapterName);
							DisableEnableConnections(true, wAdapterName);
						}
					}
				}
				RegCloseKey(hSubKey);
			}
			cName = 1024;
			dwIndex++;
		}
		RegCloseKey(hKey);
	}
	else
	{
		std::cerr << "[!]Cannot Access Registry - Maybe you don't have Administer permission." << std::endl;
		return;
	}

	std::cout << "----------------------------------------------" << std::endl;

}

HRESULT MAC::DisableEnableConnections(BOOL Enable, const wchar_t* AdapterName)
{
	HRESULT hr = E_FAIL;

	CoInitialize(NULL);

	INetConnectionManager* pNetConnectionManager = NULL;
	hr = CoCreateInstance(CLSID_ConnectionManager,
		NULL,
		CLSCTX_LOCAL_SERVER | CLSCTX_NO_CODE_DOWNLOAD,
		IID_INetConnectionManager,
		reinterpret_cast<LPVOID*>(&pNetConnectionManager)
	);
	if (SUCCEEDED(hr))
	{
		IEnumNetConnection* pEnumNetConnection;
		pNetConnectionManager->EnumConnections(NCME_DEFAULT, &pEnumNetConnection);

		ULONG ulCount = 0;
		BOOL fFound = FALSE;
		hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		HRESULT hrT = S_OK;

		do
		{
			NETCON_PROPERTIES* pProps = NULL;
			INetConnection* pConn;

			hrT = pEnumNetConnection->Next(1, &pConn, &ulCount);

			if (SUCCEEDED(hrT) && 1 == ulCount)
			{
				hrT = pConn->GetProperties(&pProps);

				if (S_OK == hrT)
				{
					if (Enable && (_tcscmp((TCHAR*)pProps->pszwDeviceName, (const char*)AdapterName) == 0))
					{
						printf("[+]Enabling adapter: %S...\n", pProps->pszwDeviceName);
						hr = pConn->Connect();
					}
					else if (_tcscmp((TCHAR*)pProps->pszwDeviceName, (const char*)AdapterName) == 0)
					{
						printf("[+]Disabling adapter: %S...\n", pProps->pszwDeviceName);
						hr = pConn->Disconnect();
					}

					CoTaskMemFree(pProps->pszwName);
					CoTaskMemFree(pProps->pszwDeviceName);
					CoTaskMemFree(pProps);
				}

				pConn->Release();
				pConn = NULL;
			}

		} while (SUCCEEDED(hrT) && 1 == ulCount && !fFound);

		if (FAILED(hrT))
		{
			hr = hrT;
		}

		pEnumNetConnection->Release();
	}

	if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_RETRY))
	{
		printf("Could not enable or disable connection (0x%08x)\r\n", hr);
	}

	pNetConnectionManager->Release();
	CoUninitialize();

	return hr;
}