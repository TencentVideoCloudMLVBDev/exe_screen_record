#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include "json.h"
#include "log.h"

enum ScreenRecordType
{
	RecordScreenNone = 0,
	RecordScreenToServer = 1,
	RecordScreenToClient = 2,
	RecordScreenToBoth = 3,
};

enum ScreenRecordCmd
{
	ScreenRecordStart = 1001,
	ScreenRecordStop = 1002,
	ScreenRecordExit = 1003,
};

class TXCloudRecordCmd
{
protected:
	TXCloudRecordCmd();
public:
	virtual ~TXCloudRecordCmd();
	static TXCloudRecordCmd& instance();

	int runAndRecord(ScreenRecordType recordType, std::string recordUrl, std::string recordExe, int winID = -1);
	void exit();
	void start();
	void stop();

private:
	HWND m_recordHwnd = nullptr;
};
