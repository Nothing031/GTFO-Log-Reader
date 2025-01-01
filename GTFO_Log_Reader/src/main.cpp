#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>
#include <chrono>
#include <algorithm>

#include <Windows.h>
#include <conio.h>

#include <nlohmann/json.hpp>

#include "AudioManager.hpp"
#include "data.h"
#include "LogReader.h"
#include "Marker.hpp"
#include "JsonManager.hpp"

using namespace std;
using namespace nlohmann;

#define CONSOLE_CELL_X 80
#define CONSOLE_CELL_Y 20
#define CLEAR_INFO_MAX 16

// GLOBAL VARIABLE
bool NOIMAGE = true;

HWND consoleHwnd;
HWND hGTFO;
HANDLE hConsoleOutput;
HANDLE hConsoleInput;

std::stack<CLEARINFO> clearInfoStack;
///////////////////

#define SLEEP(duration) std::this_thread::sleep_for(std::chrono::milliseconds(duration))
#define SET_COLOR(color) SetConsoleTextAttribute(hConsoleOutput, color);

inline void gotoxy(int x, int y) {
	COORD xy = { x, y };
	SetConsoleCursorPosition(hConsoleOutput, xy);
}
inline void __IntroTextFailed() {
	gotoxy(17, 2);
	SET_COLOR(WHITE);
	cout << "SUBJECT ";
	SET_COLOR(RED);
	cout << "NOT ";
	SET_COLOR(WHITE);
	cout << "READY FOR CORTEX INTERFACE INJECTION";
}
inline void __IntroText() {
	gotoxy(19, 2);
	SET_COLOR(WHITE);
	cout << "SUBJECT READY FOR CORTEX INTERFACE INJECTION";
}
void DisplayIntro() {
	if (hGTFO == NULL) {
		SLEEP(800);
		AudioManager::Play(AudioManager::Intro);
		SLEEP(150);
		__IntroTextFailed();
		SLEEP(20);
		cout << "\r                                                                                 ";
		SLEEP(20);
		__IntroTextFailed();
		SLEEP(20);
		cout << "\r                                                                                 ";
		SLEEP(28);
		__IntroTextFailed();
		SLEEP(40);
		cout << "\r                                                                                 ";
		SLEEP(40);
		__IntroTextFailed();
		SLEEP(40);
		cout << "\r                                                                                 ";
		SLEEP(40);
		__IntroTextFailed();

		while (hGTFO == NULL) {
#ifdef _DEBUG
			if (_kbhit()) {
				if (_getch() == 0x20) {
					return;
				}
			}
#endif // _DEBUG
			SLEEP(100);
			hGTFO = FindWindowA("UnityWndClass", "GTFO");
		}
	}	
	SLEEP(800);
	AudioManager::Play(AudioManager::Intro);
	SLEEP(150);
	__IntroText();
	SLEEP(20);
	cout << "\r                                                                                 ";
	SLEEP(20);
	__IntroText();
	SLEEP(20);
	cout << "\r                                                                                 ";
	SLEEP(28);
	__IntroText();
	SLEEP(40);
	cout << "\r                                                                                 ";
	SLEEP(40);
	__IntroText();
	SLEEP(40);
	cout << "\r                                                                                 ";
	SLEEP(40);
	__IntroText();
	SLEEP(700);
}

void ShowClearInfo() {
	if (clearInfoStack.empty()) return;

	int y = 3;
	int infoShown = 0;
	stack<CLEARINFO> cpyClearInfos = clearInfoStack;
	while (!cpyClearInfos.empty()) {
		gotoxy(4, y);
		cout << cpyClearInfos.top().expedition << "    ";
		int color = (cpyClearInfos.top().state == eExpeditionEndState::ExpeditionSuccess ? LIGHT_GREEN : RED);
		SET_COLOR(color);
		cout << cpyClearInfos.top().clearTime;
		switch (cpyClearInfos.top().state) {
		case eExpeditionEndState::ExpeditionSuccess:
			cout << "    SURVIVED";
			break;
		case eExpeditionEndState::ExpeditionFail:
			cout << "    FAILED  ";
			break;
		case eExpeditionEndState::ExpeditionAbort:
			cout << "    ABORTED ";
			break;
		}
		SET_COLOR(WHITE);
		cpyClearInfos.pop();
		y++;
		infoShown++;
		if (infoShown == CLEAR_INFO_MAX)
			break;
	}
}
void HideKeys() {
	// clear key
	int x = 41;
	int y = 1;

	for (y = 0; y < CONSOLE_CELL_Y - 1; y++) {
		gotoxy(x, y);
		cout << string(38, ' ');
	}
}
void ShowKeys(vector<KEY>& keys) {
	HideKeys();
	AudioManager::Play(AudioManager::Ping);
	// key classification
	sort(keys.begin(), keys.end(), [](const KEY& key1, const KEY& key2) {return key1.zone < key2.zone; });
	vector<KEY> colorKeys;
	vector<KEY> bulkheadKeys;

	for (int i = 0; i < keys.size(); i++) {
		if (keys[i].name.find("BULKHEAD") != std::string::npos)
			bulkheadKeys.push_back(keys[i]);
		else
			colorKeys.push_back(keys[i]);
	}

	// display key;
	int x = 41;
	int y = 1;
	// bulkhead Key
	if (bulkheadKeys.size() != 0) {
		SET_COLOR(WHITE);
		gotoxy(x, y);
		cout << "             BULKHEAD KEY";
		for (int i = 0; i < bulkheadKeys.size(); i++) {
			string name = AddPadding(bulkheadKeys[i].name, 22, ' ');
			string zone = AddPadding(to_string(bulkheadKeys[i].zone), 7, ' ');
			string ri = AddPadding(to_string(bulkheadKeys[i].index), 2, ' ');
			// gotoxy;
			gotoxy(x, ++y);
			cout << name << "Zone: ";
			SET_COLOR(YELLOW);
			cout << zone;
			SET_COLOR(WHITE);
			cout << "" << ri;
		}
		y++;
	}

	// color key
	if (colorKeys.size() != 0) {
		gotoxy(x, ++y);
		cout << "             COLOR KEY";
		for (int i = 0; i < colorKeys.size(); i++) {
			string name = AddPadding(colorKeys[i].name, 22, ' ');
			string zone = AddPadding(to_string(colorKeys[i].zone), 7, ' ');
			string ri = AddPadding(to_string(colorKeys[i].index), 2, ' ');
			gotoxy(x, ++y);
			cout << name << "Zone: ";
			SET_COLOR(YELLOW);
			cout << zone;
			SET_COLOR(WHITE);
			cout << "" << ri;
		}
	}
}
void ShowExpedition(const string& rundown) {
	gotoxy(17, 1);
	cout << rundown;
}

std::string GetLogPath() {
	wchar_t* localAppData = nullptr;
	SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &localAppData);
	std::wstring ws(localAppData);
	std::string dirPath(ws.begin(), ws.end());
	dirPath += "\\10 Chambers Collective\\GTFO";

	vector<string> files;
	size_t pos;

	for (const auto& file : filesystem::directory_iterator(dirPath)) {
		if (file.path().filename().string().find("_NICKNAME_NETSTATUS.") != string::npos) {
			files.push_back(file.path().string());
		}
	}
	if (files.size() == 0) {
		return "";
	}
	else {
		return files[files.size() - 1];
	}
}

void Init() {
	setlocale(LC_ALL, "");
	SetConsoleTitleA("GTFO LOG READER");

	// get handles
	consoleHwnd = GetConsoleWindow();
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	hGTFO = FindWindowA("UnityWndClass", "GTFO");

#ifdef _DEBUG // always on top
	SetWindowPos(consoleHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	ShowWindow(consoleHwnd, SW_NORMAL);
#endif 

	// set console size
	int x = 33 + 8 * CONSOLE_CELL_X;
	int y = 39 + 16 * CONSOLE_CELL_Y;
	RECT rect;
	GetWindowRect(consoleHwnd, &rect);
	MoveWindow(consoleHwnd, rect.left, rect.top, x, y, false);

	// set console unresizeable
	LONG style = GetWindowLong(consoleHwnd, GWL_STYLE);
	style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
	SetWindowLong(consoleHwnd, GWL_STYLE, style);

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
}

int main() {
	// Init
	Init();
	AudioManager::Init();

	// intro
	DisplayIntro();

	// init LogReader
	LogReader logReader;
	do {
		logReader.Init(&clearInfoStack);
		SLEEP(500);
	} while (!logReader.initSuccess);
	system("cls");


	//flags
	bool keyDisplayed = false;
	bool clearInfoDisplayed = false;
	size_t prevExpeHash = 0;

	while (true) {
		logReader.ReadFile();
		logReader.ParseLog();
		switch (logReader.expedition.progress) {
			case eExpeditionProgress::NoLobby :{
					ShowExpedition("    ");
					break;
				}
			case eExpeditionProgress::Lobby: {
				// show rundown info
				if (prevExpeHash != logReader.expedition.hash) {
					ShowExpedition(logReader.expedition.name);
				}
				// show clear info
				if (!clearInfoDisplayed) {
					ShowClearInfo();
					HideKeys();
					clearInfoDisplayed = true;
				}
				
				keyDisplayed = false;
				break;
				}
			case eExpeditionProgress::ItemSpawned: {
				// show key info
				if (!keyDisplayed) {
					logReader.FindKey();
					if (logReader.expedition.keys.empty()) 
						keyDisplayed = true; // no any key exists
					else {
						ShowKeys(logReader.expedition.keys);
						if(Marker::MarkKeys(logReader.expedition.name, logReader.expedition.keys))
							ShellExecuteA(NULL, "open", ".\\MAP", NULL, NULL, SW_SHOWDEFAULT);
					}
				}

				keyDisplayed = true;
				break;
			}
			case eExpeditionProgress::InLevel: {
				if (!keyDisplayed) {
					logReader.FindKey();
					if (logReader.expedition.keys.empty()) 
						keyDisplayed = true; // no any key exists
					else {
						ShowKeys(logReader.expedition.keys);
						if (Marker::MarkKeys(logReader.expedition.name, logReader.expedition.keys))
							ShellExecuteA(NULL, "open", ".\\MAP", NULL, NULL, SW_SHOWDEFAULT);
					}
				}

				keyDisplayed = true;
				clearInfoDisplayed = false;
				break;
			}
			case eExpeditionProgress::ExpeditionDone: {
				// TODO show clear time
				if (!clearInfoDisplayed) {
					ShowClearInfo();
					clearInfoDisplayed = true;
				}
				break;
			}
		}
		SLEEP(200);
	}
}