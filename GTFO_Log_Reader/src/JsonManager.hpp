#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>
#include <fstream>


#include "data.h"

using namespace nlohmann;

class JsonManager {
private:
	typedef struct PosStruct {
		int index;
		Position position;
	}POSSTRUCT;

	typedef struct ExpeditionDoorStruct {
		int index;
		int zone;
		bool do_not_show = false;
		bool active_custom_name = false;
		std::string custom_name = "ZONE_?";
	}EXPDOOR;

	static std::string GetPosJsonPath(std::string expedition, int zone) {
		return "MAPS\\" + expedition.substr(0, 2) + "\\" + expedition.substr(2, 2) + "\\Zone_" + std::to_string(zone) + ".json";
	}

public:
	static Position GetPosition(std::string expedition, int zone, int index) {
		Position pos = { -1, -1 };

		std::string filepath = GetPosJsonPath(expedition, zone);
		if (!std::filesystem::exists(filepath))
			return pos;

		std::ifstream i(filepath);
		json data = json::parse(i);
		i.close();
		for (int eIndex = 0; eIndex < data.size(); eIndex++) {
			if (data[eIndex]["index"] == index) {
				pos.x = data[eIndex]["position"]["x"];
				pos.y = data[eIndex]["position"]["y"];
				return pos;
			}
		}
		return pos;
	}
};



