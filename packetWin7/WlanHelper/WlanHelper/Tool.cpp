
#include "tchar.h"
#include <algorithm>
#include "Tool.h"

vector<tstring> g_strAdapterNames;
vector<tstring> g_strAdapterGUIDs;

tstring strToLower(const tstring &str)
{
	tstring strTmp = str;
	transform(strTmp.begin(), strTmp.end(), strTmp.begin(), tolower);
	return strTmp;
}

bool compareNoCase(const tstring &strA, const tstring &strB)
{
	tstring str1 = strToLower(strA);
	tstring str2 = strToLower(strB);
	return (str1 == str2);
}

wstring ANSIToUnicode(const string& str)
{
	size_t len = 0;
	len = str.length();
	int unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t * pUnicode;
	pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	wstring rt;
	rt = (wchar_t*)pUnicode;
	delete pUnicode;
	return rt;
}

wstring executeCommand(wchar_t* cmd)
{
	char buffer[128];
	string tmp = "";
	wstring result;

	FILE* pipe = _wpopen(cmd, L"r");
	if (!pipe)
	{
		return L"";
	}

	while (!feof(pipe))
	{
		if (fgets(buffer, 128, pipe) != NULL)
			tmp += buffer;
	}
	_pclose(pipe);

	result = ANSIToUnicode(tmp);

	return result;
}

// There is 1 interface on the system:
// 
//     Name                   : Wi-Fi
//     Description            : Qualcomm Atheros AR9485WB-EG Wireless Network Adapter
//     GUID                   : 42dfd47a-2764-43ac-b58e-3df569c447da
//     Physical address       : a4:db:30:d9:3a:9a
//     State                  : connected
//     SSID                   : LUO-PC_Network
//     BSSID                  : d8:15:0d:72:8c:18
//     Network type           : Infrastructure
//     Radio type             : 802.11n
//     Authentication         : WPA2-Personal
//     Cipher                 : CCMP
//     Connection mode        : Auto Connect
//     Channel                : 1
//     Receive rate (Mbps)    : 150
//     Transmit rate (Mbps)   : 150
//     Signal                 : 100%
//     Profile                : LUO-PC_Network
// 
//     Hosted network status  : Not available

void initAdapterList()
{
	size_t iStart = -1;
	size_t iEnd;
	wstring strAdapterName;
	wstring strGUID;

	wstring strOutput = executeCommand(L"netsh wlan show interfaces");
	
	iStart = strOutput.find(L"\n\n", iStart + 1);
	if (iStart == wstring::npos) return;
	while ((iStart = strOutput.find(L": ", iStart + 1)) != wstring::npos)
	{
		iStart += 2;
		iEnd = strOutput.find(L'\n', iStart + 1);
		if (iEnd == wstring::npos) return;
		strAdapterName = strOutput.substr(iStart, iEnd - iStart);
		
		iStart = strOutput.find(L": ", iStart + 1);
		if (iStart == wstring::npos) return;
		iStart = strOutput.find(L": ", iStart + 1);
		if (iStart == wstring::npos) return;
		iStart += 2;
		iEnd = strOutput.find(L'\n', iStart + 1);
		if (iEnd == wstring::npos) return;
		strGUID = strOutput.substr(iStart, iEnd - iStart);

		g_strAdapterNames.push_back(strAdapterName);
		g_strAdapterGUIDs.push_back(strGUID);

		iStart = strOutput.find(L"\n\n", iStart + 1);
		if (iStart == wstring::npos) return;
	}
}

tstring getGuidFromAdapterName(TCHAR *pszAdapterName)
{
	if (g_strAdapterNames.size() == 0)
	{
		initAdapterList();
	}

	for (size_t i = 0; i < g_strAdapterNames.size(); i++)
	{
		if (compareNoCase(g_strAdapterNames[i], pszAdapterName))
		{
			return g_strAdapterGUIDs[i];
		}
	}

	return _T("");
}