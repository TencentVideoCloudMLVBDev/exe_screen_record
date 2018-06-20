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
	ScreenRecordUpdate = 1001,
	ScreenRecordStart = 1002,
	ScreenRecordStop = 1003,
	ScreenRecordExit = 1004,
};

struct RecordData{
	ScreenRecordType recordType;
	char  recordUrl[512];
	char  recordPath[512];
	char  recordExe[64];
	int sliceTime;
	int winID;
	RecordData()
	{
		this->winID = -1;
		this->sliceTime = 60;
	}
};

class TXCloudRecordCmd
{
protected:
	TXCloudRecordCmd();
public:
	virtual ~TXCloudRecordCmd();
	static TXCloudRecordCmd& instance();

	bool runAndRecord(RecordData recordData);
	void update(RecordData recordData);
	void start();
	void stop();
	void exit();

private:
	HWND m_recordHwnd = nullptr;
};
