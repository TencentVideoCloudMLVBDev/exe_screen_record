#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include "json.h"
#include "log.h"
#include "TXLivePusher.h"
#include "commonType.h"

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
