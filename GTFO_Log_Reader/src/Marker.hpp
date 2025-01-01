#pragma once

#include <string>
#include <filesystem>


#include <opencv2/opencv.hpp>
#include "JsonManager.hpp"



class Marker {
private:
	static std::string GetImagePath(std::string expedition, int zone) {
		return "MAPS\\" + expedition.substr(0, 2) + "\\" + expedition.substr(2, 2) + "\\Zone_" + std::to_string(zone) + ".jpg";
	}



public:
	static bool MarkItem(std::string expedition, int zone, int index) {
		std::string filePath = GetImagePath(expedition, zone);
		if (!std::filesystem::exists(filePath))
			return false;
		






	}
};








