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
	std::string url = "xxx";

	TXCloudRecordCmd::instance().runAndRecord(RecordScreenToServer, url, "chrome.exe");
	system("pause");

	TXCloudRecordCmd::instance().stop();
	system("pause");

	TXCloudRecordCmd::instance().start();
	system("pause");

	TXCloudRecordCmd::instance().exit();
	system("pause");
    return 0;
}