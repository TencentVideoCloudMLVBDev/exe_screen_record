#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include "json.h"
#include "log.h"
#include "TXLivePusher.h"

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

class TXCloudRecord
{
protected:
	TXCloudRecord();

public:
	virtual ~TXCloudRecord();
	static TXCloudRecord& instance();
	int run(int &argc, char **argv);

private:
	std::string m_jsonCmd;
};
