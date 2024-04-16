
#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <vector>
#include <filesystem>

#include <ShlObj.h>
#include <Windows.h>
#include <shellapi.h>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>


using namespace std;
using namespace cv;
using namespace nlohmann;

namespace fs = std::filesystem;

class Position {
public:
	int x;
	int y;
};

bool FileExists(const string& filePath) {
	ifstream fs(filePath);
	return fs.good();
}
Position ReadJson(const string& rundown, const int& zone, const int& Ri) {
	Position pos;
	try {
		string jsonPath = "MAPS\\" + rundown.substr(0, 2) + "\\" + rundown.substr(2, 2) + "\\Zone_" + to_string(zone) + ".json";
		ifstream fs(jsonPath);
		json data = json::parse(fs);

		pos.x = data[Ri]["Position"]["x"];
		pos.y = data[Ri]["Position"]["y"];

		fs.close();
		data.clear();
	}
	catch (const exception& e) {
		pos.x = -1;
		pos.y = -1;
	}
	return pos;
}
void MarkOnImage(const string& filePath, const int& zone, const int& Ri, const Position& pos) {
	Mat image = imread(filePath);

	// public
	Point point(pos.x, pos.y);
	

	// circle
	int radius = 40;
	Scalar color1(0, 255, 0);
	circle(image, point, radius, color1, 2, LINE_AA);
	

	// text
	Scalar color2(0, 255, 0);
	int fontFace = FONT_HERSHEY_SIMPLEX;
	Point offset(0, radius / -2 - 20);
	Point textPoint = point + offset;
	double fontScale = 1.0;
	putText(image, to_string(Ri), textPoint, fontFace, fontScale, color2, 2, LINE_AA);

	// save image
	string savePath = "MAP\\Zone_" + to_string(zone) + "_" + to_string(Ri) + ".jpg";
	imwrite(savePath, image);
}
void OpenImage(const string& rundown, const int& zone, const int& Ri) {
	Position pos;
	string filePath = ".\\MAPS\\" + rundown.substr(0, 2) + "\\" + rundown.substr(2, 2) + "\\Zone_" + to_string(zone) + ".jpg";
	string jsonPath = ".\\MAPS\\" + rundown.substr(0, 2) + "\\" + rundown.substr(2, 2) + "\\Zone_" + to_string(zone) + ".json";
	if (FileExists(filePath)) {
		pos = ReadJson(rundown, zone, Ri);
		if (!(pos.x == -1)) {
			MarkOnImage(filePath, zone, Ri, pos);
		}
		else {
			cout << "error while reading json file " << "\"" << jsonPath << "\"" << endl;
		}
	}
	else {
		cout << "map file " << "\"" << filePath << "\"" << " not exists" << endl;
	}
}

void UpdateFilePath(string* filePathAddress) {
	wchar_t* localAppData = nullptr;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppData);

	std::wstring ws(localAppData);
	std::string path(ws.begin(), ws.end());
	CoTaskMemFree(localAppData);
	path += "Low\\10 Chambers Collective\\GTFO";

	vector<string> files;
	size_t pos;

	files.clear();
	for (const auto& entry : fs::directory_iterator(path)) {
		pos = entry.path().filename().string().find("_NICKNAME_NETSTATUS.");
		if (pos != std::string::npos) {
			files.push_back(entry.path().string());
			Sleep(1);
		}
	}
	if (files.size() == 0) {
		*filePathAddress = "";
	}
	else {
		*filePathAddress = files[files.size() - 1];
	}
}
void UpdateLogData(vector<string>* data, string* filePath) {
	ifstream inputFile(*filePath);
	string fline;
	vector<string> flines;

	while (getline(inputFile, fline)) {
		flines.push_back(fline);
	}
	inputFile.close();
	*data = flines;
}
int UpdateRundownData(vector<string>* data, string* rundown, vector<vector<string>>* keyData) {

	size_t pos;

	vector<string> _data = *data;

	int lastDropIndex = 0;

	// get last drop index
	for (int i = _data.size() - 1; i >= 0; i--) {
		pos = _data[i].find("SelectActiveExpedition :");
		if (pos != string::npos) {
			lastDropIndex = i;
			break;
		}
	}
	if (lastDropIndex == 0) {
		return 1;
	}

	// get rundownData
	string line = _data[lastDropIndex];
	line.erase(0, 77).erase(10, line.size() - 1);
	int rundownIndex = stoi(line.substr(0, 2));
	string tier = line.substr(7, 1);
	int tierIndex = stoi(line.substr(line.size() - 1)) + 1;
	switch (rundownIndex) {
	case 32:
		rundownIndex = 1; break;
	case 33:
		rundownIndex = 2; break;
	case 34:
		rundownIndex = 3; break;
	case 37:
		rundownIndex = 4; break;
	case 38:
		rundownIndex = 5; break;
	case 41:
		rundownIndex = 6; break;
	case 31:
		rundownIndex = 7; break;
	case 35:
		rundownIndex = 8; break;
	}
	*rundown = "R" + to_string(rundownIndex) + tier + to_string(tierIndex);

	// get key
	size_t pos1;
	vector<vector<string>> _keyData;
	string keyLine;

	int k = 0;
	for (int i = lastDropIndex; i < _data.size(); i++) {
		pos1 = _data[i].find("Spawning KEYCARD");
		if (pos1 != string::npos) {
			return 0;
		}
		pos = _data[i].find("CreateKeyItemDistribution");
		if (pos != string::npos) {
			_keyData.push_back(vector<string>());
			_keyData[k].push_back(_data[i].erase(0, 77)
				.erase(_data[i].find(" SpawnedItem:"), _data[i].length()));

			for (int j = i; j < _data.size(); j++) {
				pos = _data[j].find("ri: ");
				if (pos != string::npos) {
					keyLine = _data[j].erase(0, 106).erase(_data[j].find(" had weight"), _data[j].size());
					_keyData[k].push_back(keyLine.substr(0, keyLine.find_first_of(' ')));
					_keyData[k].push_back(keyLine.substr(keyLine.find("ri: ") + 4, 2));
					k++;
					break;
				}
			}
			*keyData = _keyData;
		}
	}
	return 1;
}


int main(int argc, char* argv[]) {
	int error = 0;

	// Previous data
	vector<vector<string>> prevKeyData;

	// Current data
	string filePath;
	vector<string> data;
	string rundown;
	vector<vector<string>> keyData;

	size_t pos;
	string executable_path = fs::canonical(fs::path(argv[0])).parent_path().string();




	getchar();

	while (1) {
		// update file name
		UpdateFilePath(&filePath);
		if (filePath == "") {
			Sleep(5000);
			continue;
		}

		// update data
		UpdateLogData(&data, &filePath);
		error = UpdateRundownData(&data, &rundown, &keyData);
		if (error == 1) {
			Sleep(1000);
			continue;
		}
		// compare key data
		if (prevKeyData != keyData) {
			prevKeyData = keyData;
		}
		else {
			Sleep(2000);
			continue;
		}

		// print
		cout << endl << endl << endl;
		cout << rundown << endl << endl;

		string line1;
		string line2;
		string line3;

		// print bulkhead key
		cout << "BULKHEAD KEY" << endl;
		for (int i = 0; i < keyData.size(); i++) {
			pos = keyData[i][0].find("BULK");
			if (pos != string::npos) {
				line1 = "    " + keyData[i][0].insert(keyData[i][0].size(), 20 - keyData[i][0].size(), ' ');
				line2 = "Zone : " + keyData[i][1].insert(keyData[i][1].size(), 8 - keyData[i][1].size(), ' ');
				line3 = "Ri : " + keyData[i][2].insert(keyData[i][2].size(), 8 - keyData[i][2].size(), ' ');
				cout << line1 << line2 << line3 << endl;
			}
		}
		// print color key
		cout << endl << "COLOR KEY" << endl;
		for (int i = 0; i < keyData.size(); i++) {
			pos = keyData[i][0].find("BULK");
			if (pos == string::npos) {
				line1 = "    " + keyData[i][0].insert(keyData[i][0].size(), 20 - keyData[i][0].size(), ' ');
				line2 = "Zone : " + keyData[i][1].insert(keyData[i][1].size(), 8 - keyData[i][1].size(), ' ');
				line3 = "Ri : " + keyData[i][2].insert(keyData[i][2].size(), 8 - keyData[i][2].size(), ' ');
				cout << line1 << line2 << line3 << endl;
			}
		}
		cout << endl;
		
		for (const auto& entry : fs::directory_iterator(".\\MAP")) {
			fs::remove(entry.path());
		}
		
		for (int i = 0; i < keyData.size(); i++) {
			int zone = stoi(keyData[i][1]);
			int ri = stoi(keyData[i][2]);
			OpenImage(rundown, zone, ri);
		}
		ShellExecuteA(NULL, "open", ".\\MAP", NULL, NULL, SW_SHOWDEFAULT);

		Sleep(1000);
	}
}