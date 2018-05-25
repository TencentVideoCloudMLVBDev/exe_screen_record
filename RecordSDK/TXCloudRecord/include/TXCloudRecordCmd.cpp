#include "TXCloudRecordCmd.h"
#include <Psapi.h>
#include <Shellapi.h>
#include <assert.h>  
#pragma comment (lib,"Psapi.lib")

#define WINDOW_CLASS_NAME TEXT("TXCloudRecord")
#define WINDOW_CAPTION_NAME TEXT("TXCloudRecordCaption")
#define RecordExe "TXCloudRecord.exe"

static unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

static unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}

static std::string URLEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

static std::string URLDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

TXCloudRecordCmd::TXCloudRecordCmd()
{
}

TXCloudRecordCmd::~TXCloudRecordCmd()
{
	stop();
}

TXCloudRecordCmd& TXCloudRecordCmd::instance()
{
	static TXCloudRecordCmd uniqueInstance;
	return uniqueInstance;
}

int TXCloudRecordCmd::runAndRecord(ScreenRecordType recordType, std::string recordUrl, std::string recordExe, int winID)
{
	LOGGER;

	do
	{
		Json::Value root;
		root["recordUrl"] = recordUrl;
		root["recordExe"] = recordExe; //需要录制的exe名称。启动record之前就需要运行
										 //root["winID"] = (int)GetDesktopWindow();  //需要录制的窗口句柄。winID和recordExe必须传一个，都传会录制recordExe内容
		if (recordExe.empty() && winID != -1)
		{
			root["winID"] = winID;
		}
		root["recordType"] = recordType;

		Json::FastWriter writer;
		std::string jsonUTF8 = writer.write(root);
		std::string encodeJson = URLEncode(jsonUTF8);

		std::string cmd = "\"txcloudrecord://liteav/params?json=";
		cmd.append(encodeJson);
		cmd.append("\\");

		char szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
		(strrchr(szFilePath, '\\'))[0] = 0;
		std::string path = szFilePath;
		path.append("\\");
		path.append(RecordExe);

		
		SHELLEXECUTEINFOA sei = { 0 };
		sei.cbSize = sizeof(SHELLEXECUTEINFOA);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.hwnd = NULL;
		sei.lpVerb = "open";
		sei.lpFile = path.c_str();
		sei.lpParameters = cmd.c_str();
		sei.lpDirectory = NULL;
		sei.nShow = SW_HIDE;
		sei.hInstApp = NULL;
		sei.lpIDList = NULL;
		sei.lpClass = NULL;
		sei.hkeyClass = NULL;
		sei.dwHotKey = NULL;
		sei.hIcon = NULL;
		sei.hProcess = NULL;

		BOOL ret = ::ShellExecuteExA(&sei);
		if (FALSE == ret)
		{
			LERROR(L"ShellExecuteExW failed: %lu", ::GetLastError());
			return false;
		}

	} while (0);
	return 0;
}

void TXCloudRecordCmd::exit()
{
	LINFO(L"txcloudrecord exit");
	if (!m_recordHwnd)
	{
		m_recordHwnd = FindWindowExA(HWND_MESSAGE, NULL, "TXCloudRecord", "TXCloudRecordCaption");
	}

	if (m_recordHwnd)
	{
		std::string message = "RecordExit";
		COPYDATASTRUCT copy_data = { ScreenRecordExit, message.length() + 1,(LPVOID)message.c_str() };
		::SendMessage(m_recordHwnd, WM_COPYDATA, ScreenRecordExit, reinterpret_cast<LPARAM>(&copy_data));
	}
}

void TXCloudRecordCmd::start()
{
	LINFO(L"txcloudrecord start");
	if (!m_recordHwnd)
	{
		m_recordHwnd = FindWindowExA(HWND_MESSAGE, NULL, "TXCloudRecord", "TXCloudRecordCaption");
	}

	if (m_recordHwnd)
	{
		std::string message = "RecordStart";
		COPYDATASTRUCT copy_data = { ScreenRecordStart, message.length() + 1,(LPVOID)message.c_str() };
		::SendMessage(m_recordHwnd, WM_COPYDATA, ScreenRecordStart, reinterpret_cast<LPARAM>(&copy_data));
	}
}

void TXCloudRecordCmd::stop()
{
	LINFO(L"txcloudrecord stop");
	if (!m_recordHwnd)
	{
		m_recordHwnd = FindWindowExA(HWND_MESSAGE, NULL, "TXCloudRecord", "TXCloudRecordCaption");
	}

	if (m_recordHwnd)
	{
		std::string message = "RecordStop";
		COPYDATASTRUCT copy_data = { ScreenRecordStop, message.length() + 1,(LPVOID)message.c_str() };
		::SendMessage(m_recordHwnd, WM_COPYDATA, ScreenRecordStop, reinterpret_cast<LPARAM>(&copy_data));
	}
}
