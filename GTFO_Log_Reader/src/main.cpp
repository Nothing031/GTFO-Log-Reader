#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>
#include <chrono>
#include <mutex>

#include <Windows.h>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "__data.hpp"
#include "LogReader.hpp"

using namespace std;
using namespace cv;
using namespace nlohmann;

namespace fs = filesystem;

mutex printMutex;
HWND hwnd;
HWND hGTFO;
HANDLE outputHandle;
HANDLE inputHandle;

bool FORCEINJECT;

void gotoxy(int x, int y) {
	COORD xy = { x, y };
	SetConsoleCursorPosition(outputHandle, xy);
}
void PrintTyping(const string& _str, const bool& _endl, const int& delay) {
	printMutex.lock();
	for (int i = 0; i < _str.size(); i++) {
		cout << _str[i];
		this_thread::sleep_for(chrono::milliseconds(delay));
	}
	if (_endl) cout << endl;
	printMutex.unlock();
}
void PrintLoading() {
	const int maxValue = 100;
	const double initialDelay = 8;
	const double factor = 1.12;

	double delay = initialDelay;

	for (int i = 0; i <= maxValue; ++i) {
		std::cout << "\r" << i << "%";
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay)));
		if (i >= 70)
			delay *= factor; // 딜레이를 점점 증가
	}
}

inline bool FileExists(const string& filePath) {
	ifstream ifs(filePath);
	return ifs.good();
}


void AutoPause() {
	while (true) {
		if (GetForegroundWindow() != hGTFO) break;
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	hGTFO = FindWindowA(Keywords::GTFO_CLASS, Keywords::GTFO_CAPTION);
}

void DisplayKeys(vector<key_t>& keys) {
	string line;
	vector<key_t> Bulkhead;
	vector<key_t> Color;

	// classification
	for (int i = 0; i < keys.size(); i++) {
		if (keys[i].name.find("BULK") != string::npos)
			Bulkhead.push_back(keys[i]);
		else
			Color.push_back(keys[i]);
	}

	// bulkhead

	cout << "BULKHEAD KEY" << endl;
	for (int i = 0; i < Bulkhead.size(); i++) {
		line = "    " + Bulkhead[i].name.insert(Bulkhead[i].name.size(), 20 - Bulkhead[i].name.size(), ' ');
		line += "Zone : " + Bulkhead[i].zone.insert(Bulkhead[i].zone.size(), 8 - Bulkhead[i].zone.size(), ' ');
		line += "Ri : " + Bulkhead[i].ri.insert(Bulkhead[i].ri.size(), 8 - Bulkhead[i].ri.size(), ' ');
		cout << line << endl;
	}
	cout << endl;

	cout << "COLOR KEY" << endl;
	for (int i = 0; i < Color.size(); i++) {
		line = "    " + Color[i].name.insert(Color[i].name.size(), 20 - Color[i].name.size(), ' ');
		line += "Zone : " + Color[i].zone.insert(Color[i].zone.size(), 8 - Color[i].zone.size(), ' ');
		line += "Ri : " + Color[i].ri.insert(Color[i].ri.size(), 8 - Color[i].ri.size(), ' ');
		cout << line << endl;
	}
	cout << endl;

}

void Init() {
	// init variable
	FORCEINJECT = false;

	// get handles
	hwnd = GetConsoleWindow();
	outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	inputHandle = GetStdHandle(STD_INPUT_HANDLE);

	// set window
	RECT rect;
	GetWindowRect(hwnd, &rect);
	MoveWindow(hwnd, rect.left, rect.top, 800, 400, false);

	// hide cursor
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(outputHandle, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(outputHandle, &cursorInfo);

	// disable quick edit mode
	DWORD mode;
	if (GetConsoleMode(inputHandle, &mode)) {
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_EXTENDED_FLAGS;
		SetConsoleMode(inputHandle, mode);
	}

	// set font
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(outputHandle, FALSE, &fontInfo);
	fontInfo.dwFontSize.Y = 15;
	SetCurrentConsoleFontEx(outputHandle, FALSE, &fontInfo);
	

}

int main() {
	// Init
	Init();



	// intergrity check
	if (!fs::exists(".\\MAP"))
		fs::create_directory(".\\MAP");
	//if (!fs::exists("opencv_world490.dll")) {
	//	cout << "\"opencv_world490.dll\" does not exists" << endl;
	//	getchar();
	//	return 1;
	//}

	// variables
	eCurrentProgress currentProgress;
	vector<key_t> keys;
	eLogState logState;
	LogReader logReader;

	// intro
	hGTFO = FindWindowA(Keywords::GTFO_CLASS, Keywords::GTFO_CAPTION);
	if (hGTFO == NULL) {
		PrintTyping("SUBJECT NOT READY FOR CORTEX INTERFACE INJECTION", false, 15);
		int _count = 0;
		while (hGTFO == NULL) {
			if (GetAsyncKeyState(0x20) & 0x8000) {
				FORCEINJECT = true;
				break;
			}
			this_thread::sleep_for(chrono::milliseconds(10));
			hGTFO = FindWindowA(Keywords::GTFO_CLASS, Keywords::GTFO_CAPTION);
		}
	}
	if (!FORCEINJECT) {
		cout << "\r                                                                                        \r";
		PrintTyping("SUBJECT READY FOR CORTEX INTERFACE INJECTION", true, 15);
		this_thread::sleep_for(chrono::milliseconds(500));
		PrintTyping("INJECTING", true, 5);
		PrintLoading();
		this_thread::sleep_for(chrono::milliseconds(500));
	}
	else {
		cout << endl;
		PrintTyping("OVERRIDE : ", false, 15);
		this_thread::sleep_for(chrono::milliseconds(500));
		PrintTyping("FORCE INJECT", true, 60);
		this_thread::sleep_for(chrono::milliseconds(500));
		PrintTyping("INJECTING", true, 5);
		PrintLoading();
		this_thread::sleep_for(chrono::milliseconds(500));
	}

	






	
	return 0;




	while (true) {
		logState = logReader.GetKeys(&keys, &currentProgress);

		switch (logState) {
		case eLogState::NoAnyLog:
			this_thread::sleep_for(chrono::seconds(5));
			break;
		case eLogState::Dropping:
			this_thread::sleep_for(chrono::milliseconds(500));
			break;
		case eLogState::NoAnyKey:
			// display 


			// pause
			this_thread::sleep_for(chrono::milliseconds(500));
			AutoPause();
			break;
		case eLogState::KeyFound:
			// display


			// pause
			this_thread::sleep_for(chrono::milliseconds(500));
			AutoPause();
			break;
		}





	}







}



