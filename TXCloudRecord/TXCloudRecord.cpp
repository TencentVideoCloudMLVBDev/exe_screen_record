#include "TXCloudRecord.h"
#include "Base.h"
#include "TXLiveCommon.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h> 
#include <ShObjIdl.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

#define WINDOW_CLASS_NAME TEXT("TXCloudRecord")
#define WINDOW_CAPTION_NAME TEXT("TXCloudRecordCaption")

TXLivePusher m_pusher;
HWND m_hChooseHwnd = nullptr;
std::string m_recordUrl;
std::string m_recordPath;
ScreenRecordType m_recordType = RecordScreenNone;

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

void startRecord()
{
	RECT captureRect = { 0 };
	RECT renderRect = { 0 };
	m_pusher.setVideoQualityParamPreset(TXE_VIDEO_QUALITY_STILLIMAGE_DEFINITION);
	m_pusher.setScreenCaptureParam(m_hChooseHwnd, captureRect, false);
	m_pusher.startPreview(TXE_VIDEO_SRC_SDK_SCREEN, nullptr, renderRect);
	//sdk内部默认是镜像模式（针对摄像头），但是录屏出来的源数据本来就是镜像模式。
	m_pusher.setRenderYMirror(false);
	m_pusher.setOutputYMirror(false);
	m_pusher.openSystemVoiceInput();
	m_pusher.startAudioCapture();

	if (m_recordType == RecordScreenToServer)
	{
		size_t index = m_recordUrl.find("record");  // 忽略大小写
		if (std::string::npos == index)
		{
			m_recordUrl.append("&record=mp4&record_interval=7200");
		}

		m_pusher.startPush(m_recordUrl.c_str());
	}
	else if (m_recordType == RecordScreenToClient)
	{
		m_pusher.startLocalRecord(m_recordPath.c_str(), 60 * 60);
	}
	else if (m_recordType == RecordScreenToBoth)
	{
		size_t index = m_recordUrl.find("record");  // 忽略大小写
		if (std::string::npos == index)
		{
			m_recordUrl.append("&record=mp4&record_interval=7200");
		}

		m_recordUrl = m_recordUrl;
		m_pusher.startPush(m_recordUrl.c_str());
		m_pusher.startLocalRecord(m_recordPath.c_str(), 60 * 60);
	}
}

void stopRecord()
{
	m_pusher.stopAudioCapture();
	m_pusher.stopPreview();

	if (m_recordType == RecordScreenToServer)
	{
		m_pusher.stopPush();
	}
	else if (m_recordType == RecordScreenToClient)
	{
		m_pusher.stopLocalRecord();
	}
	else if (m_recordType == RecordScreenToBoth)
	{
		m_pusher.stopPush();
		m_pusher.stopLocalRecord();
	}
}

TXCloudRecord::TXCloudRecord()
{
}

TXCloudRecord::~TXCloudRecord()
{
}

TXCloudRecord& TXCloudRecord::instance()
{
	static TXCloudRecord uniqueInstance;
	return uniqueInstance;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COPYDATA)
	{
		COPYDATASTRUCT* copy_data = reinterpret_cast<COPYDATASTRUCT*>(lParam);
		const char* message = reinterpret_cast<const char*>(copy_data->lpData);
		LINFO(L"recv message:%s", message);

		switch (wParam)
		{
		case ScreenRecordStart:
		{
			if (!strcmp(message, "RecordStart"))
			{
				if (InSendMessage())
					ReplyMessage(0);

				startRecord();
			}
		}
		break;
		case ScreenRecordStop:
		{
			if (!strcmp(message, "RecordStop"))
			{
				if (InSendMessage())
					ReplyMessage(0);

				stopRecord();
			}
		}
		break;
		case ScreenRecordExit:
		{
			if (!strcmp(message, "RecordExit"))
			{
				if (InSendMessage())
					ReplyMessage(0);

				stopRecord();
				exit(0);
			}
		}
		break;
		default:
			break;
		}
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL CreateRecordWindow()
{
	WNDCLASSEX wc = { 0 };
	ATOM ret;
	HWND hwnd;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WINDOW_CLASS_NAME;
	ret = RegisterClassEx(&wc);
	if (ret == NULL && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
	{
		LINFO(L"RegisterClassEx Fail:%d\r\n", GetLastError());
		return FALSE;
	}

	hwnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_CAPTION_NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, GetModuleHandle(NULL), 0);

	LINFO(L"hwnd:%x\r\n", hwnd);

	if (!::IsWindow(hwnd))
		return FALSE;
	::UpdateWindow(hwnd);
	return TRUE;
}

typedef struct _tagCAPTUREWINDOWINFOEX
{
	HWND   hWnd;			// window handle
	std::string  strWndName;    // window name
}CAPTUREWINDOWINFOEX;

static BOOL CALLBACK EnumTaskbarWnds(HWND hwnd, LPARAM lParam)
{
	std::vector<CAPTUREWINDOWINFOEX>* pWndList = (std::vector<CAPTUREWINDOWINFOEX> *)lParam;
	if (!GetWindow(hwnd, GW_OWNER) && IsWindowVisible(hwnd) && !IsIconic(hwnd))//没有父窗口、窗口可见、窗口没有最小化
	{
		char szClassName[MAX_PATH] = { 0 };
		GetClassNameA(hwnd, szClassName, MAX_PATH);

		if (strcmp(szClassName, "Shell_TrayWnd") != 0				//滤掉任务栏本身  
			&& strcmp(szClassName, "Progman") != 0)					//滤掉桌面
		{
			DWORD dwId = 0;
			GetWindowThreadProcessId(hwnd, &dwId);
			HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				0, dwId);

			if (handle)
			{
				char path_buffer[_MAX_PATH] = { 0 };
				char drive[_MAX_DRIVE] = { 0 };
				char dir[_MAX_DIR] = { 0 };
				char fname[_MAX_FNAME] = { 0 };
				char ext[_MAX_EXT] = { 0 };

				GetProcessImageFileNameA(handle, path_buffer, MAX_PATH);
				_splitpath_s(path_buffer, drive, dir, fname, ext);
				strcat_s(fname, ext);

				CAPTUREWINDOWINFOEX item;
				item.hWnd = hwnd;
				item.strWndName = fname;
				if (pWndList)
					pWndList->push_back(item);
			}

		}
	}
	return TRUE;
}

int TXCloudRecord::run(int &argc, char **argv)
{
	//任务栏隐藏图标
	ITaskbarList *pTaskList = NULL;
	HRESULT initRet = CoInitialize(NULL);
	HRESULT createRet = CoCreateInstance(CLSID_TaskbarList,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskbarList,
		(LPVOID*)&pTaskList);

	if (createRet == S_OK)
	{

		pTaskList->DeleteTab(GetConsoleWindow());

		pTaskList->Release();
	}

	CoUninitialize();

	//隐藏控制台窗口
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	//::MessageBoxW(NULL, NULL, NULL, MB_OK);   // 方便附加调试
	LOGGER;
	do
	{
		if (argc <= 1)
		{
			LINFO(L"no cmd");
			break;
		}
		m_jsonCmd = ::GetCommandLineA();  // 注意，因argv会忽略命令行中的引号，导致json字符串不合法，估使用GetCommandLine函数

		std::string prefix = "txcloudrecord://liteav/params?json=";
		size_t index = m_jsonCmd.find(prefix);  // 忽略大小写
		if (std::string::npos == index)
		{
			LINFO(L"error cmd");
			break;
		}

		std::string json = m_jsonCmd.substr(index + prefix.size());
		std::string jsonDecode = URLDecode(json);

		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(jsonDecode, root))
		{
			LINFO(L"error json");
			break;;
		}

		m_recordType = RecordScreenNone;
		if (root.isMember("recordType"))
		{
			m_recordType = (ScreenRecordType)root["recordType"].asInt();
		}

		if (m_recordType == RecordScreenNone)
		{
			LINFO(L"record type none");
			break;
		}

		if (root.isMember("recordUrl"))
		{
			m_recordUrl = root["recordUrl"].asString();
		}

		if (root.isMember("recordPath"))
		{
			m_recordPath = root["recordPath"].asString();
		}

		if (m_recordUrl.empty() && m_recordPath.empty())
		{
			LINFO(L"empty recordurl and recordpath");
			break;
		}

		std::string recordExe;
		if (root.isMember("recordExe"))
		{
			recordExe = root["recordExe"].asString();
		}

		int winID = -1;
		if (root.isMember("winID"))
		{
			winID = root["winID"].asInt();
		}

		if (recordExe.empty() && winID == -1)
		{
			LINFO(L"empty recordExe name && winID");
			break;
		}

		std::string ip;
		int proxyPort = 0;
		if (root.isMember("proxy"))
		{
			Json::Value proxy = root["proxy"];

			if (proxy.isMember("ip"))
			{
				ip = proxy["ip"].asString();
			}

			if (proxy.isMember("port"))
			{
				proxyPort = proxy["port"].asInt();
			}
		}
		if (!ip.empty())
		{
			TXLiveCommon::getInstance()->setProxy(ip.c_str(), proxyPort);
		}

		m_hChooseHwnd = nullptr;

		if (!recordExe.empty())
		{
			std::vector<CAPTUREWINDOWINFOEX> windowVec;
			BOOL bRet = EnumWindows(EnumTaskbarWnds, (LPARAM)&windowVec);
			for (int i = 0; i < windowVec.size(); i++)
			{
				if (!strcmp(windowVec[i].strWndName.c_str(), recordExe.c_str()))
				{
					m_hChooseHwnd = windowVec[i].hWnd;
					break;
				}
			}
		}
		else
			m_hChooseHwnd = (HWND)winID;

		if (!m_hChooseHwnd)
		{
			LINFO(L"error exe name or the exe doesn't run, or error winid");
			break;
		}

		startRecord();
		CreateRecordWindow();

		MSG msg;
		while (::GetMessage(&msg, NULL, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

	} while (0);

	return 0;
}


