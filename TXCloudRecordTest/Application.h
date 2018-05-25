#pragma once

#include <Windows.h>
#include <string>

class Application
{
protected:
    Application();

public:
    virtual ~Application();
    static Application& instance();
    int run(int &argc, char **argv);

private:
	std::string m_jsonCmd;
};
