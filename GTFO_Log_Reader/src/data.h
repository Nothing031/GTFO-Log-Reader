#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>


#define NPOS std::string::npos

namespace Filters {
	static constexpr const char* Lobby = ">Select";

	static constexpr const char* b_GameStateManager = "GAMESTATEMANAGER";

	static constexpr const char* c_NoLobby = "TO: NoL";
	static constexpr const char* c_Generating = "TO: Ge";
	static constexpr const char* ItemSpawning = "Next Batch: Distribution -";
	static constexpr const char* ItemSpawned = "Next Batch: FunctionMarkerLeft";
	static constexpr const char* c_StopElevatorRide = "TO: StopEle";
	static constexpr const char* c_InLevel = "TO: InLe";
	static constexpr const char* c_ExpeditionDone = "InLevel TO: Expe";

	static constexpr const char* cc_ExpeditionSuccess = "Success";
	static constexpr const char* cc_ExpeditionFail = "Fail";
	static constexpr const char* cc_ExpeditionAbort = "Abort";
}

static constexpr const char* alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

enum COLOR {
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	YELLOW = 6,
	WHITE = 7,
	GRAY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10,
	LIGHT_CYAN = 11,
	LIGHT_RED = 12,
	LIGHT_MAGENTA = 13,
	LIGHT_YELLOW = 14,
	LIGHT_WHITE = 15
};

typedef struct _KEY {
	int ri;
	int zone;
	std::string name;
}KEY;

typedef struct _HSU {
	std::string name;
	std::string zone;
	std::string area;
}HSU;

typedef struct _FACTOR {
	int x;
	int y;
}FACTOR;

enum class eExpeditionProgress {
	NoLobby,
	Lobby,
	Generating,
	ItemSpawning,
	ItemSpawned,
	StopElevatorRide,
	InLevel,
	ExpeditionDone // success, fail, abort
};

enum class eExpeditionEndState {
	ExpeditionSuccess,
	ExpeditionFail,
	ExpeditionAbort
};

typedef struct _CLEARINFO {
	std::string clearTime;
	std::string expedition;
	eExpeditionEndState state;
} CLEARINFO;

typedef struct _EXPEDITION_INDEX {
	int NoLobby = 0;
	int Lobby = 0;
	int Generating = 0;
	int itemSpawnStart = 0;
	int itemSpawnEnd = 0;
	int StopElevatorRide = 0;
	int InLevel = 0;
	int ExpeditionDone = 0;
}EXPEDITION_INDEX;

//typedef struct _LOG_FILE_DATA {
//	bool synced = false;
//	std::string logPath = "";
//	std::ifstream logStream;
//	std::streampos lastStreamPos = 0;
//	std::vector<std::string> logData;
//	int lastCheckedVectorIndex = 0;
//}LOG_FILE_DATA;

typedef struct _EXPEDITION_DATA {
	int seed = 0;
	size_t hash;
	std::string name = "?";
	eExpeditionProgress progress = eExpeditionProgress::NoLobby;
	
	EXPEDITION_INDEX Index;
	
	std::vector<KEY> keys;
	// HSU hsu; // not yet supported

	int expeditionStartTime = 0;
	int expeditoinEndTime = 0;
	int clearTime = 0;
	eExpeditionEndState expeditionEndState = eExpeditionEndState::ExpeditionAbort;

	bool clearDataExported = false;
}EXPEDITION_DATA;








