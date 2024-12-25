#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <ShlObj.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <regex>
#include <functional>
#include <stack>

#include "utils.hpp"
#include "data.h"


class LogReader
{
public:
    bool initSuccess;
    bool synced; 

	std::string logPath;
    std::ifstream logStream;
	std::streampos lastStreamPos;
    std::vector<std::string> logData;
    int lastCheckedIndex;

    EXPEDITION_DATA expedition;

    std::stack<CLEARINFO>* pClearInfoStack;

    LogReader();
    ~LogReader();

    bool Init(std::stack<CLEARINFO>* pStack);

    std::string getFilePath();
    bool LogSynchronousCheck();

    EXPEDITION_DATA NewExpedition(const std::string& line);

    void ReadFile();
    void ParseLog();
    void FindKey();
};