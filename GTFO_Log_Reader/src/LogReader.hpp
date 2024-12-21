#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

#include <Windows.h>
#include <ShlObj.h>

#include "__data.hpp"

using namespace std;

namespace fs = filesystem;


class LogReader
{
public:
	LogReader() {
		// get dirPath
		wchar_t* localAppData = nullptr;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localAppData);

		wstring ws(localAppData);
		string path(ws.begin(), ws.end());
		CoTaskMemFree(localAppData);
		this->dirPath = path + "Low\\10 Chambers Collective\\GTFO";


		this->streamPointer = 0;
		this->prevStreamPointer = 0;
		this->state = eLogState::NoAnyKey;
	}

	eLogState GetKeys(vector<key_t>* Keys, eCurrentProgress* currentProgress) {
		Keys->clear();
		// file check
		if (!this->TryGetFilePath()) {
			*currentProgress = eCurrentProgress::NoAnyLogFound;
			return eLogState::NoAnyKey;
		}


		// read file
		*currentProgress = eCurrentProgress::ReadingFile;
		this->ReadFile();

		// Parse data
		*currentProgress = eCurrentProgress::ParsingData;
		if (!this->TryParseData(Keys)) {
			return this->state;
		}
		
		*currentProgress = eCurrentProgress::Done;
		return eLogState::KeyFound;
	}
private:
	// file
	string dirPath;
	string filePath;
	streampos streamPointer;
	streampos prevStreamPointer;
	
	// log
	string rundown;
	string rundownRawData;
	vector<string> data;

	// state
	eLogState state;

	bool TryGetFilePath() {
		vector<string> files;
		for (const auto& entry : filesystem::directory_iterator(this->dirPath)) {
			if (entry.path().filename().string().find("_NICKNAME_NETSTATUS.") != string::npos) {
				files.push_back(entry.path().string());
			}
		}
		if (files.size() == 0) {
			this->filePath = "";
			return false;
		}
		else {
			this->filePath = files[files.size() - 1];
			return true;
		}
	}
	void ReadFile() {
		data.clear();
		string line;
		size_t pos;
		ifstream file(this->filePath);
		file.seekg(this->streamPointer);
		
		while (getline(file, line)) {
			pos = line.find(Keywords::DropPoint_Keyword);
			if (pos != string::npos) {
				data.clear();
				this->rundownRawData = line;
				this->streamPointer = file.tellg();
			}
			else {
				data.push_back(line);
			}
		}
	}
	string GetRundownData() {
		try {
			string line = this->rundownRawData;
			line.erase(0, 77).erase(10, line.size() - 1);
			string tier = line.substr(7, 1);
			int rundownIndex = stoi(line.substr(0, 2));
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
			default:
				rundownIndex = 0; break;
			}
			this->rundown = "R" + to_string(rundownIndex) + tier + to_string(tierIndex);
		}
		catch (exception e){
			this->rundown = "UNKNOWN RUNDOWN";
		}
	}// endfunc
	bool TryParseData(vector<key_t>* Keys) {
		bool KeyLoaded = false;
		bool Droped = false;

		// pos
		size_t pos_ri     = -1;
		size_t pos_dist   = -1;
		size_t pos_zone   = -1;
		int keySpawnPoint = -1;

		// get KeySpawnPoint
		for (int i = this->data.size() - 1; i >= 0; i--) {
			if (this->data[i].find(Keywords::KeySpawn_Keyword) != string::npos) {
				KeyLoaded = true;
				keySpawnPoint = i;
				break;
			}
			if (this->data[i].find(Keywords::ObjectiveStarted) != string::npos) {
				Droped = true;
			}
		}
		if (!KeyLoaded && Droped) {
			this->state = eLogState::NoAnyKey;
			return false;
		}
		else if (!KeyLoaded && !Droped) {
			this->state = eLogState::Dropping;
			return false;
		}
		this->state = eLogState::KeyFound;

		// find key
		for (int i = keySpawnPoint; i >= 0; i--) { //find key dist loop
			pos_ri = this->data[i].find(Keywords::KeyDist_Keyword); // find start of key via dist keyword
			if (pos_ri != string::npos) {
				key_t key;
				size_t targetStart, targetEnd;
				pos_zone = this->data[i].find(Keywords::KeyZone_Keyword);

				// 06:15:11.431 - <color=#C84800>TryGetExistingGenericFunctionDistributionForSession, foundDist in zone: ZONE167 function: ResourceContainerWeak available: 36 randomValue: 0.5273821 ri: 29 had weight: 101</color>
				
				// ri
				targetStart = pos_ri + Keywords::Ri_Begin_Offset;
				targetEnd = this->data[i].find(Keywords::Ri_End_Keyword);
				key.ri = this->data[i].substr(targetStart, targetEnd - targetStart);

				// zone
				targetStart = pos_zone + Keywords::Zone_Begin_Offset;
				targetEnd = this->data[i].find(Keywords::Zone_End_Keyword);
				key.zone = this->data[i].substr(targetStart, targetEnd - targetStart);

				// name
				// rewind to get name
				for (int j = i; j >= 0; j--) {
					pos_dist = this->data[j].find(Keywords::KeyName_Keyword);
					if (pos_dist != string::npos) {
						targetStart = pos_dist + Keywords::Name_begin_Offset;
						targetEnd = this->data[j].find(Keywords::Name_End_Keyword);
						key.name = this->data[j].substr(targetStart, targetEnd - targetStart);
						break;
					}
				}
				Keys->push_back(key);
			}
		}//end find key dist loop
		return true;
	}
};

