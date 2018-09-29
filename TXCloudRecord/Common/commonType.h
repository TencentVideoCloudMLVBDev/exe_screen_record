#pragma once
#include <string>


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

struct RecordData {
    ScreenRecordType recordType;
    char  recordUrl[512];
    char  recordPath[512];
    char  recordExe[64];
    int sliceTime;
    int winID;

    RecordData()
        : recordType(RecordScreenNone)
        , sliceTime(60)
        , winID(-1)
    {
        ::memset(recordUrl, '\0', sizeof(recordUrl));
        ::memset(recordPath, '\0', sizeof(recordPath));
        ::memset(recordExe, '\0', sizeof(recordExe));
    }
};

struct RecordDataReport
{
	std::string type;
	std::string bussiness;
	std::string platform;
	std::string str_app_name;
	std::string str_token;
	uint32_t int32_sdkappid;

	std::string str_device_type;
	std::string str_app_version;
	std::string str_sdk_version;

	ScreenRecordType recordType;
	std::string str_record_url;
	std::string str_record_exe;
	uint32_t int32_sliceTime;

	std::string str_action;
	std::string str_result;
	std::string str_reason;

	RecordDataReport()
	{
		this->int32_sdkappid = 0;
		this->type = "event";
		this->bussiness = "webexe";
		this->platform = "pc";
		this->str_app_name = "TXCloudRecord";
	}
};