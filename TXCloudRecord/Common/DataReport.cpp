#include "DataReport.h"
#include <time.h>
#include <windows.h>

DataReport::DataReport()
{
}

DataReport::~DataReport()
{
}

DataReport & DataReport::instance()
{
	static DataReport dataReport;
	return dataReport;
}

int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}

uint64_t DataReport::txf_gettickspan(uint64_t lastTick) {
	struct timeval currentWin;
	gettimeofday(&currentWin, NULL);
	unsigned long long tickGet = (unsigned long long)currentWin.tv_sec * 1000 + currentWin.tv_usec / 1000;

	if (tickGet >= lastTick)
	{
		return (tickGet - lastTick);
	}
	else
	{
		return 0;
	}
}

void DataReport::setRecordInfo(ScreenRecordType recordType, std::string str_record_url, std::string str_record_exe, int sliceTime)
{
	m_recordReport.recordType = recordType;
	m_recordReport.str_record_url = str_record_url;
	m_recordReport.str_record_exe = str_record_exe;
	m_recordReport.sliceTime = sliceTime;
}

void DataReport::setResult(std::string result, std::string action, std::string reason)
{
	m_recordReport.str_result = result;
	m_recordReport.str_action = action;
	m_recordReport.str_reason = reason;
}

std::string DataReport::getRecordReport()
{
	std::string jsonUTF8;
	Json::Value root;

	root["type"] = m_recordReport.type;
	root["str_app_name"] = m_recordReport.str_app_name;
	root["str_token"] = m_recordReport.str_token;
	root["int32_app_id"] = m_recordReport.int32_sdkappid;

	root["str_device_type"] = m_recordReport.str_device_type;
	root["str_app_version"] = m_recordReport.str_app_version;
	root["str_sdk_version"] = m_recordReport.str_sdk_version;

	root["recordType"] = m_recordReport.recordType;
	root["str_record_url"] = m_recordReport.str_record_url;
	root["str_record_exe"] = m_recordReport.str_record_exe;
	root["sliceTime"] = m_recordReport.sliceTime;

	root["str_action"] = m_recordReport.str_action;
	root["str_result"] = m_recordReport.str_result;
	root["str_reason"] = m_recordReport.str_reason;

	Json::FastWriter writer;
	jsonUTF8 = writer.write(root);
	return jsonUTF8;
}

uint64_t DataReport::txf_gettickcount() {
	static unsigned long long s_TickDelta = 0;

	unsigned long long tickGet = 0;

	struct timeval currentWin;
	gettimeofday(&currentWin, NULL);
	tickGet = (unsigned long long)currentWin.tv_sec * 1000 + currentWin.tv_usec / 1000;

	if (s_TickDelta == 0) {
		struct timeval current;
		gettimeofday(&current, NULL);
		s_TickDelta = ((unsigned long long)current.tv_sec * 1000 + current.tv_usec / 1000) - tickGet;
	}

	tickGet += s_TickDelta;

	return tickGet;
}