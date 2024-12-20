#pragma once
#include <string>
#include <filesystem>


using namespace std;

inline bool FileExists(const string& filePath) {
	ifstream ifs(filePath);
	return ifs.good();
}




