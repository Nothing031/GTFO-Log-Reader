#include <iostream>
#include <ctime>
#include <time.h>
#include <chrono>
#include <fstream>
#include <filesystem>

#include <Windows.h>
#include <conio.h>

#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace nlohmann;

typedef struct _Position {
	int x;
	int y;
}Position;
typedef struct _MapIndex {
	int index;
	Position position;
}MapIndex;


HANDLE hConsoleOutput;
HANDLE hConsoleInput;
HWND hwnd;

int displayWidth;
int displayHeight;
double displayRatio;

vector<MapIndex> mapIndex;
int clickCount = 0;


inline void gotoxy(int x, int y) {
	COORD xy = { x, y };
	SetConsoleCursorPosition(hConsoleOutput, xy);
}
void HideCursor() {
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hConsoleOutput, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hConsoleOutput, &cci);
}
void Init() {
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	hwnd = GetConsoleWindow();

	// always on top
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// set console size
	RECT rect;
	GetWindowRect(hwnd, &rect);
	MoveWindow(hwnd, rect.left, rect.top, 800, 300, TRUE);

	// disable quick edit mode
	DWORD mode;
	if (GetConsoleMode(hConsoleInput, &mode)) {
		mode &= ~ENABLE_QUICK_EDIT_MODE;
		mode |= ENABLE_EXTENDED_FLAGS;
		SetConsoleMode(hConsoleInput, mode);
	}

	// get fully accessable window size
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int titlebarHeight = GetSystemMetrics(SM_CYCAPTION);
	RECT workArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
	int workAreaHeight = workArea.bottom - workArea.top;
	int taskbarHeight = screenHeight - workAreaHeight;

	
	displayWidth = screenWidth;
	displayHeight = workAreaHeight - titlebarHeight;
	displayRatio = (double)displayWidth / displayHeight;
}
string GetFilePath() {
	gotoxy(0, 0);
	cout << "drag and drop a map image              " << endl;
	cout << "currently supporting extension : .jpg  " << endl;
	FlushConsoleInputBuffer(hConsoleInput);
	std::string filePath = "";
	while (true) {
		char ch = _getch();
		if (ch == '\r')
			break;
		filePath.push_back(ch);

		if (filePath.find(".jpg") != string::npos)
			break;
	}

	size_t pos = 0;
	while ((pos = filePath.find('\\', pos)) != string::npos) {
		filePath.replace(pos, 1, "/");
		++pos;
	}
	pos = 0;
	while ((pos = filePath.find('\"', pos)) != string::npos) {
		filePath.replace(pos, 1, "");
		++pos;
	}
	return filePath;
	
}

void onMouseClick(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		Position pos;
		pos.x = x;
		pos.y = y;
		MapIndex _mapindex;
		_mapindex.position = pos;
		_mapindex.index = clickCount;
		mapIndex.push_back(_mapindex);
		clickCount++;
		gotoxy(0, 1);
		cout << clickCount << endl;
	}
}
bool CreateIndex(string filePath) {
	MoveWindow(hwnd, 0, 0, 500, 100, TRUE);
	HideCursor();

	Mat image = imread(filePath);

	// set image size
	int imageWidth = image.cols;
	int imageHeight = image.rows;
	double imageRatio = (double)imageWidth / imageHeight;
	int resizedImageWidth = 0;
	int resizedImageHeight = 0;
	if (displayRatio < imageRatio) { // if width is longer
		resizedImageWidth = displayWidth;
		resizedImageHeight = displayWidth * imageRatio;
	}
	else {
		resizedImageWidth = displayHeight * imageRatio;
		resizedImageHeight = displayHeight;
	}

	// create window
	string fileName = filesystem::path(filePath).stem().string();
	namedWindow(fileName, WINDOW_NORMAL);
	resizeWindow(fileName, resizedImageWidth, resizedImageHeight);

	imshow(fileName, image);
	system("cls");
	SetForegroundWindow(FindWindowA(NULL, fileName.c_str()));
	setMouseCallback(fileName, onMouseClick);
	while (true) {
		int key = pollKey();
		gotoxy(0, 0);
		cout << "\"+=\": skip | \"backspace\": remove | \"enter\": done" << endl;
		if (clickCount == 0) 
			cout << "NextIndex : " + to_string(clickCount) + "                        ";
		else {
			cout << "Index Saved : " + to_string(clickCount - 1) + " | NextIndex : " + to_string(clickCount);
		}
		if (key == 61){
			MapIndex mi;
			mi.position.x = -1;
			mi.position.y = -1;
			mi.index = -1;
			clickCount++;
			mapIndex.push_back(mi);
		}
		
		if (key == 8) {// backspace
			if (clickCount != 0) {
				clickCount--;
				mapIndex.pop_back();
			}
		}
		else if (key == 13)//13 return
			break;
		this_thread::sleep_for(chrono::milliseconds(10));
	}

	// show marked image
	for (int i = 0; i < mapIndex.size(); i++) {
		Point point(mapIndex[i].position.x, mapIndex[i].position.y);
		putText(image, std::to_string(mapIndex[i].index), point, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2, LINE_AA);
	}
	imshow(fileName, image);

	while (true) {
		int key = pollKey();
		gotoxy(0, 0);
		cout << "press return to confirm                           " << endl;
		cout << "press backspace to discard                        ";
		if (key == 13) {//13 return
			destroyAllWindows();
			return true;
		}
		else if (key == 8)
			return false;
		this_thread::sleep_for(chrono::milliseconds(10));
	}
	return true;
}
void CreateJson(string filePath) {
	ofstream file(filePath, ios::trunc);
	json json = json::array();

	for (int i = 0; i < mapIndex.size(); i++) {
		json.push_back({
			{"index", mapIndex[i].index},
			{"position", {
				{"x", mapIndex[i].position.x},
				{"y", mapIndex[i].position.y}
				}
			}
		});
	}
	
	file << json.dump(2);
	file.close();
}

// spaghetti code.
// but don't worry, we are not living in 20th century!!!

int main()
{
	Init();
	while(true) {
		RECT rect;
		GetWindowRect(hwnd, &rect);
		MoveWindow(hwnd, rect.left, rect.top, 800, 300, TRUE);
		HideCursor();

		string filePath;
		do {
			filePath = GetFilePath();
			if (!filesystem::exists(filePath)) {
				cout << "file not exists : " << filePath << endl;
			}
		} while (!filesystem::exists(filePath));


		bool createIndexPassed;
		do {
			clickCount = 0;
			mapIndex.clear();
			createIndexPassed = CreateIndex(filePath);
		} while (!createIndexPassed);

		filesystem::path dir = filesystem::path(filePath).parent_path();
		filesystem::path fileName = filesystem::path(filePath).filename();
		filesystem::path jsonName = fileName.stem();
		string jsonPath = (dir / jsonName).string() + ".json";
		CreateJson(jsonPath);
	}
}