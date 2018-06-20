#include "Application.h"
#include "log.h"
#include "TXCloudRecordCmd.h"

Application::Application()
{
}

Application::~Application()
{
}

Application& Application::instance()
{
    static Application uniqueInstance;
    return uniqueInstance;
}

int Application::run(int &argc, char **argv)
{
    LOGGER;
	RecordData recordData;

	strcpy_s(recordData.recordExe, "xxx"); //待录制应用程序名称 例如chrome.exe
	strcpy_s(recordData.recordUrl, "xxx"); //需要改为推流地址
	strcpy_s(recordData.recordPath, "F:\\"); //保存路径，若没有以mp4结尾，会自动保存为路径+yyyy_mm_dd_hh_mm_ss.mp4格式
	recordData.recordType = RecordScreenToBoth;
	recordData.sliceTime = 60; //本地文件分片时长，以分钟为单位
	recordData.winID = -1;     //待录制窗口句柄

	TXCloudRecordCmd::instance().runAndRecord(recordData);
	system("pause");

	TXCloudRecordCmd::instance().stop();
	system("pause");
	
	recordData.recordType = RecordScreenToClient;
	TXCloudRecordCmd::instance().update(recordData);
	system("pause");

	TXCloudRecordCmd::instance().start();
	system("pause");

	TXCloudRecordCmd::instance().exit();
	system("pause");
    return 0;
}