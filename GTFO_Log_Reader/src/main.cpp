#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>
#include <chrono>
#include <mutex>

#include <Windows.h>
#include <conio.h>
#include <mmsystem.h>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

#include "__data.hpp"
#include "LogReader.hpp"
#include "resource.h"

#pragma comment(lib, "winmm.lib")

using namespace std;
using namespace cv;
using namespace nlohmann;

namespace fs = filesystem;

// GLOBAL VARIABLE
mutex printMutex;
HWND hwnd;
HWND hGTFO;
HANDLE hConsoleOutput;
HANDLE hConsoleInput;

bool FORCEINJECT;
//////////////////


void gotoxy(int x, int y) {
	COORD xy = { x, y };
	SetConsoleCursorPosition(hConsoleOutput, xy);
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
void PrintTyping(const string& _str, const bool& _endl, const int& delay, const bool& reverse, const int& x, const int& y) {
	printMutex.lock();
	for (int i = _str.size(); i >= 0; i--) {
		gotoxy(x+i, y);
		cout << _str[i];
		this_thread::sleep_for(chrono::milliseconds(delay));
	}
	if (_endl) cout << endl;
	printMutex.unlock();
}


void PrintLoading(short x, short y) {
	const int maxValue = 100;
	const double initialDelay = 8;
	const double factor = 1.12;

	double delay = initialDelay;

	for (int i = 0; i <= maxValue; ++i) {
		gotoxy(x, y);
		cout << i << "%";
		this_thread::sleep_for(chrono::milliseconds(static_cast<int>(delay)));
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
void __IntroText() {
	gotoxy(22, 2);
	SetConsoleTextAttribute(hConsoleOutput, 15);
	cout << "SUBJECT ";
	SetConsoleTextAttribute(hConsoleOutput, 4);
	cout << "NOT ";
	SetConsoleTextAttribute(hConsoleOutput, 15);
	cout << "READY FOR CORTEX INTERFACE INJECTION";
}
void __Sleep(int millisecond) {
	this_thread::sleep_for(chrono::milliseconds(millisecond));
}
void DisplayIntro() {
	if (hGTFO == NULL) {
		__Sleep(800);
		PlaySound(MAKEINTRESOURCE(IDR_WAVE13), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		__Sleep(200);
		__IntroText();
		__Sleep(20);
		cout << "\r                                                                                 ";
		__Sleep(20);
		__IntroText();
		__Sleep(20);
		cout << "\r                                                                                 ";
		__Sleep(48);
		__IntroText();
		__Sleep(50);
		cout << "\r                                                                                 ";
		__Sleep(50);
		__IntroText();
		__Sleep(50);
		cout << "\r                                                                                 ";
		__Sleep(50);
		__IntroText();


		int _count = 0;
		while (hGTFO == NULL) {
			if (_kbhit()) {
				if (_getch() == 0x20) {
					FORCEINJECT = true;
					break;
				}
			}
			this_thread::sleep_for(chrono::milliseconds(10));
			hGTFO = FindWindowA(Keywords::GTFO_CLASS, Keywords::GTFO_CAPTION);
		}
	}
	if (!FORCEINJECT) {
		PrintTyping("                                                ", false, 7, true, 22, 2);
		this_thread::sleep_for(chrono::milliseconds(700));
		gotoxy(22, 2);
		PrintTyping("SUBJECT READY FOR CORTEX INTERFACE INJECTION", true, 15);
		this_thread::sleep_for(chrono::milliseconds(700));
		gotoxy(22, 3);
		PrintTyping("INJECTING", true, 5);
		PrintLoading(32, 3);
		this_thread::sleep_for(chrono::milliseconds(500));
		system("cls");
	}
	else {
		gotoxy(22, 3);
		SetConsoleTextAttribute(hConsoleOutput, 4);
		PrintTyping("OVERRIDE : ", false, 15);
		this_thread::sleep_for(chrono::milliseconds(500));
		PrintTyping("   ", true, 500, true, 30, 2);
		cout << "   ";
		this_thread::sleep_for(chrono::milliseconds(500));
		gotoxy(33, 3);
		PrintTyping("FORCE INJECT", true, 60);
		this_thread::sleep_for(chrono::milliseconds(700));
		SetConsoleTextAttribute(hConsoleOutput, 15);
		gotoxy(22, 4);
		PrintTyping("INJECTING ", false, 5);
		PrintLoading(32, 4);
		this_thread::sleep_for(chrono::milliseconds(500));
		system("cls");
	}

}



void Init() {
	// init variable
	FORCEINJECT = false;

	// get handles
	hwnd = GetConsoleWindow();
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	hGTFO = FindWindowA(Keywords::GTFO_CLASS, Keywords::GTFO_CAPTION);

	// set window
	RECT rect;
	GetWindowRect(hwnd, &rect);
	MoveWindow(hwnd, rect.left, rect.top, 800, 400, false);

	// hide cursor
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hConsoleOutput, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hConsoleOutput, &cci);

	// disable quick edit mode
	DWORD mode;
	if (GetConsoleMode(hConsoleInput, &mode)) {
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_EXTENDED_FLAGS;
		SetConsoleMode(hConsoleInput, mode);
	}

	// hide scroll
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
	SMALL_RECT windowSize = csbi.srWindow;
	COORD bufferSize = {
		static_cast<SHORT>(windowSize.Right - windowSize.Left + 1),
		static_cast<SHORT>(windowSize.Bottom - windowSize.Top + 1)
	};
	SetConsoleScreenBufferSize(hConsoleOutput, bufferSize);

	// set clock resolution
	timeBeginPeriod(1);
}



int main() {
	// Init
	Init();


	//
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
	DisplayIntro();

	






	
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



