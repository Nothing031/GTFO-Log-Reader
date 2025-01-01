#pragma once
#include <iostream>
#include <string>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <conio.h>

static std::string msTime_to_gtfoTime(int msTime){
	std::string hour = std::to_string(msTime / 3600000);
	msTime %= 3600000;
	std::string minute = std::to_string(msTime / 60000);
	msTime %= 60000;
	std::string second = std::to_string(msTime / 1000);
	msTime %= 1000;
	std::string millisecond = std::to_string(msTime);
	std::ostringstream gtfoTime;
	gtfoTime << std::setw(2) << std::setfill('0') << hour << ":"
		<< std::setw(2) << std::setfill('0') << minute << ":"
		<< std::setw(2) << std::setfill('0') << second << "."
		<< std::setw(3) << std::setfill('0') << millisecond;
	return gtfoTime.str();
}
static int gtfoTime_to_msTime(const std::string& gtfoTime) {
	if (gtfoTime.size() < 12) return -1;
	int hour = std::stoi(gtfoTime.substr(0, 2));
	int minute = std::stoi(gtfoTime.substr(3, 2));
	int second = std::stoi(gtfoTime.substr(6, 2));
	int millisecond = std::stoi(gtfoTime.substr(9, 3));
	int msTime = millisecond
		+ second * 1000
		+ minute * 60000
		+ hour * 3600000;
	return msTime;
}
static int get_current_UTC_ms_time() {
	std::chrono::time_point<std::chrono::system_clock> chrono_now = std::chrono::system_clock::now();
	std::chrono::milliseconds chrono_ms = std::chrono::duration_cast<std::chrono::milliseconds>(chrono_now.time_since_epoch()) % 1000;
	std::time_t now = std::chrono::system_clock::to_time_t(chrono_now);
	std::tm utc_time;
	gmtime_s(&utc_time, &now);

	int currentMsTime = chrono_ms.count()
		+ utc_time.tm_sec * 1000
		+ utc_time.tm_min * 60000
		+ utc_time.tm_hour * 3600000;
	return currentMsTime;
}

static std::string AddPadding(const std::string& str, int Size, char Separator) {
	std::ostringstream oss;
	oss  << std::left << std::setw(Size) << std::setfill(Separator) << str;
	return oss.str();
}
static std::string SubByKey(std::string baseStr, std::string beginFilter, std::string endFilter)
{
	size_t beginPos = baseStr.find(beginFilter);
	size_t endPos = baseStr.find(endFilter);
	if (beginPos == std::string::npos || endPos == std::string::npos)
		return "";

	int start = beginPos + beginFilter.size();
	int count = endPos - start;
	return baseStr.substr(start, count);
}
static std::string SubByKey(std::string baseStr, int beginOffset, std::string endFilter)
{
	size_t endPos = baseStr.find(endFilter);
	if (endPos == std::string::npos)
		return "";
	int count = endPos - beginOffset;
	return baseStr.substr(beginOffset, count);
}