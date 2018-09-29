#include "TXCloudRecord.h"
#include "Base.h"
#include "TXLiveCommon.h"
#include "DataReport.h"
#include "HttpReportRequest.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h> 
#include <ShObjIdl.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

#define WINDOW_CLASS_NAME TEXT("TXCloudRecord")
#define WINDOW_CAPTION_NAME TEXT("TXCloudRecordCaption")

TXLivePusher m_pusher, *m_pLivePusher = nullptr;
HWND m_hChooseHwnd = nullptr;
HWND m_mainHwnd = nullptr;
UINT_PTR m_timerID = 0x1;
std::string m_recordUrl;
std::string m_recordPath;
std::string m_recordExe;
ScreenRecordType m_recordType = RecordScreenNone;
HANDLE m_parentProcess = nullptr;
int m_sliceTime = 60;

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
	if (m_pLivePusher == nullptr) 
		return;

	RECT captureRect = { 0 };
	RECT renderRect = { 0 };
	m_pLivePusher->setVideoQualityParamPreset(TXE_VIDEO_QUALITY_STILLIMAGE_DEFINITION);
	m_pLivePusher->setScreenCaptureParam(m_hChooseHwnd, captureRect, false, true);
	//m_pusher.setVideoResolution(TXE_VIDEO_RESOLUTION_1280x720);
	m_pLivePusher->startPreview(TXE_VIDEO_SRC_SDK_SCREEN, nullptr, renderRect);
	//sdk内部默认是镜像模式（针对摄像头），但是录屏出来的源数据本来就是镜像模式。
	m_pLivePusher->setRenderYMirror(false);
	m_pLivePusher->setOutputYMirror(false);
	m_pLivePusher->openSystemVoiceInput();
	m_pLivePusher->startAudioCapture();

	if (m_recordType == RecordScreenToServer)
	{
		size_t index = m_recordUrl.find("record");  // 忽略大小写
		if (std::string::npos == index)
		{
			m_recordUrl.append("&record=mp4&record_interval=7200");
		}

		m_pLivePusher->startPush(m_recordUrl.c_str());
	}
	else if (m_recordType == RecordScreenToClient)
	{
		m_pLivePusher->startLocalRecord(m_recordPath.c_str(), m_sliceTime * 60);
	}
	else if (m_recordType == RecordScreenToBoth)
	{
		size_t index = m_recordUrl.find("record");  // 忽略大小写
		if (std::string::npos == index)
		{
			m_recordUrl.append("&record=mp4&record_interval=7200");
		}

		m_recordUrl = m_recordUrl;
		m_pLivePusher->startPush(m_recordUrl.c_str());
		m_pLivePusher->startLocalRecord(m_recordPath.c_str(), m_sliceTime * 60);
	}

	DataReport::instance().setResult("success", "start", "");
	DataReport::instance().setRecordInfo(m_recordType, m_recordUrl, m_recordExe, m_sliceTime);
	HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
}

void stopRecord()
{
	if (m_pLivePusher == nullptr) return;
	m_pLivePusher->stopAudioCapture();
	m_pLivePusher->stopPreview();

	if (m_recordType == RecordScreenToServer)
	{
		m_pLivePusher->stopPush();
	}
	else if (m_recordType == RecordScreenToClient)
	{
		m_pLivePusher->stopLocalRecord();
	}
	else if (m_recordType == RecordScreenToBoth)
	{
		m_pLivePusher->stopPush();
		m_pLivePusher->stopLocalRecord();
	}
	DataReport::instance().setResult("success", "stop", "");
	DataReport::instance().setRecordInfo(m_recordType, m_recordUrl, m_recordExe, m_sliceTime);
	HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
}

TXCloudRecord::TXCloudRecord()
    : m_jsonCmd("")
{
	m_pLivePusher = new TXLivePusher();
}

TXCloudRecord::~TXCloudRecord()
{
	if (m_pLivePusher)
	{
		delete m_pLivePusher;
	}

	m_pLivePusher = nullptr;
}

TXCloudRecord& TXCloudRecord::instance()
{
	static TXCloudRecord uniqueInstance;
	return uniqueInstance;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COPYDATA:
    {
        COPYDATASTRUCT* copy_data = reinterpret_cast<COPYDATASTRUCT*>(lParam);
        if (!copy_data)
        {
            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }

        switch (copy_data->dwData)
        {
        case ScreenRecordUpdate:
        {
            RecordData data = *reinterpret_cast<RecordData*>(copy_data->lpData);
            LINFO(L"recv message: update");

            if (InSendMessage())
                ReplyMessage(0);

            if (data.recordType != RecordScreenNone)
            {
                stopRecord();

                m_recordPath = data.recordPath;
                m_recordUrl = data.recordUrl;
                m_sliceTime = data.sliceTime;
                m_recordType = data.recordType;

                startRecord();
            }
        }
        break;
        case ScreenRecordStart:
        {
            if (InSendMessage())
                ReplyMessage(0);

            startRecord();
        }
        break;
        case ScreenRecordStop:
        {
            if (InSendMessage())
                ReplyMessage(0);

            stopRecord();
        }
        break;
        case ScreenRecordExit:
        {
            if (InSendMessage())
                ReplyMessage(0);

            stopRecord();

			DataReport::instance().setResult("success", "exit", "");
			DataReport::instance().setRecordInfo(m_recordType, m_recordUrl, m_recordExe, m_sliceTime);
			HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
            if (m_pLivePusher)
            {
                delete m_pLivePusher;
                m_pLivePusher = nullptr;
            }

            TXCloudRecord::instance().quit();
        }
        break;
        default:
            break;
        }
    }
    break;
    case WM_TIMER:
    {
        DWORD ret = ::WaitForSingleObject(m_parentProcess, 0);
        if (WAIT_TIMEOUT != ret)
        {
            stopRecord();

            //DataReport::instance().setResult("success", "exit", "");
            //DataReport::instance().setRecordInfo(m_recordType, m_recordUrl, m_recordExe, m_sliceTime);
            //HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());

            if (m_pLivePusher)
            {
                delete m_pLivePusher;
                m_pLivePusher = nullptr;
            }

            TXCloudRecord::instance().quit();
        }
    }
    break;
    case WM_DESTROY:
    {
        ::PostQuitMessage(0);
    }
    break;
    default:
        break;
    }

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

HWND CreateRecordWindow()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = WINDOW_CLASS_NAME;
    ATOM ret = RegisterClassEx(&wc);
	if (ret == NULL && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
	{
		LINFO(L"RegisterClassEx Fail:%d\r\n", GetLastError());
		return FALSE;
	}

	HWND hwnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_CAPTION_NAME, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, GetModuleHandle(NULL), 0);

	LINFO(L"hwnd:%x\r\n", hwnd);

	if (!::IsWindow(hwnd))
		return NULL;

	::UpdateWindow(hwnd);

	return hwnd;
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

std::string& replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
{
	while (true) {
		std::string::size_type   pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

int TXCloudRecord::run(int &argc, char **argv)
{
    LOGGER;

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

    curl_global_init(CURL_GLOBAL_ALL);

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
			DataReport::instance().setResult("fail", "error", "error cmd");
			HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
			break;
		}

		std::string json = m_jsonCmd.substr(index + prefix.size());
		std::string jsonDecode = URLDecode(json);

		Json::Value root;
		Json::Reader reader;
		if (false == reader.parse(jsonDecode, root))
		{
			LINFO(L"error json");
			DataReport::instance().setResult("fail", "error", "error json");
			HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
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
			DataReport::instance().setResult("fail", "error", "record type none");
			HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
			break;
		}

		if (root.isMember("recordUrl"))
		{
			m_recordUrl = root["recordUrl"].asString();
		}

		if (root.isMember("recordPath"))
		{
			m_recordPath = root["recordPath"].asString();
			if (!m_recordPath.empty() && m_recordPath.find("mp4") == std::string::npos)
			{
				replace_all(m_recordPath, "/", "\\");

				if (m_recordPath.rfind("\\") != m_recordPath.length() - 1)
				{
					m_recordPath.append("\\");
				}

				time_t lnTime;
				struct tm *stTime;
				char strdate[32];

				time(&lnTime);
				stTime = localtime(&lnTime);

				strftime(strdate, 32, "%Y_%m_%d_%H_%M_%S", stTime);
				m_recordPath.append(strdate);
				m_recordPath.append(".mp4");
			}
		}

		if (root.isMember("sliceTime"))
		{
			m_sliceTime = root["sliceTime"].asInt();
		}

		if (m_recordUrl.empty() && m_recordPath.empty())
		{
			LINFO(L"empty recordurl and recordpath");
			break;
		}

		if (root.isMember("recordExe"))
		{
			m_recordExe = root["recordExe"].asString();
		}

		int winID = -1;
		if (root.isMember("winID"))
		{
			winID = root["winID"].asInt();
		}

		if (m_recordExe.empty() && winID == -1)
		{
			LINFO(L"empty recordExe name && winID");
			DataReport::instance().setResult("fail", "error", "empty recordExe name && winID");
			HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
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

		if (!m_recordExe.empty())
		{
			std::vector<CAPTUREWINDOWINFOEX> windowVec;
			BOOL bRet = EnumWindows(EnumTaskbarWnds, (LPARAM)&windowVec);
			for (int i = 0; i < windowVec.size(); i++)
			{
				if (!strcmp(windowVec[i].strWndName.c_str(), m_recordExe.c_str()))
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

			DataReport::instance().setResult("fail", "error", "error hwnd");
			HttpReportRequest::instance().reportELK(DataReport::instance().getRecordReport());
			break;
		}

        m_mainHwnd = CreateRecordWindow();
        if (!m_mainHwnd)
        {
            LINFO(L"CreateRecordWindow failed: %lu", ::GetLastError());
        }

        DWORD parentPid = 0;
        if (root.isMember("parentPid"))
        {
            parentPid = root["parentPid"].asUInt64();
        }

        if (0 != parentPid)
        {
            m_parentProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, parentPid);
            if (!m_parentProcess)
            {
                LINFO(L"CreateRecordWindow failed: %lu", ::GetLastError());
                break;
            }
        }

        ::SetTimer(m_mainHwnd, m_timerID, 1000, NULL);  // 每次检测一次父进程是否存活

		startRecord();

        MSG msg = { 0 };
		while (::GetMessage(&msg, NULL, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	} while (0);

    LINFO(L"HttpReportRequest close");

    HttpReportRequest::instance().close();

    LINFO(L"curl_global_cleanup");

    curl_global_cleanup();

	return 0;
}

void TXCloudRecord::quit()
{
    if (m_mainHwnd && ::IsWindow(m_mainHwnd))
    {
        ::KillTimer(m_mainHwnd, m_timerID);
        ::DestroyWindow(m_mainHwnd);
    }
    else
    {
        ::PostQuitMessage(0);
    }
}
