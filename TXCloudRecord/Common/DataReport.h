#pragma once
#include "json.h"
#include "commonType.h"

class DataReport
{
private:
	DataReport();

public:
	virtual ~DataReport();

	/**
	* \brief：获取DataReport单例，通过单例调用DataReport的接口
	*/
	static DataReport& instance();

	uint64_t txf_gettickcount();
	uint64_t txf_gettickspan(uint64_t lastTick);


	void setRecordInfo(ScreenRecordType recordType, std::string str_record_url, std::string str_record_exe, uint32_t sliceTime);
	void setResult(std::string result, std::string action, std::string reason = "");

	//getreport
	std::string getRecordReport();

private:
	RecordDataReport m_recordReport;
};
