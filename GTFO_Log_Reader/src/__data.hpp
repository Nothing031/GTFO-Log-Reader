#pragma once
#include <string>

struct Keywords {
	static constexpr const char* DropPoint_Keyword = "SelectActiveExpedition :";
	static constexpr const char* KeyDist_Keyword = "CreateKeyItemDistribution";
	static constexpr const char* KeyName_Keyword = "CreateKeyItemDistribution";
	static constexpr const char* KeyRi_Keyword = "ri: ";
	static constexpr const char* KeyZone_Keyword = "zone: ZONE";
	static constexpr const char* KeySpawn_Keyword = "Spawning KEYCARD";
	static constexpr const char* ObjectiveStarted = "OBJECTIVE STARTED ";

	static constexpr int Ri_Begin_Offset = 4;
	static constexpr const char* Ri_End_Keyword = " had";

	static constexpr int Zone_Begin_Offset = 10;
	static constexpr const char* Zone_End_Keyword = " function:";

	static constexpr int Name_begin_Offset = 48;
	static constexpr const char* Name_End_Keyword = " SpawnedItem";

	static constexpr const char* GTFO_CAPTION = "GTFO";
	static constexpr const char* GTFO_CLASS = "UnityWndClass";
	
};

typedef struct __KEY_STRUCT__ {
	std::string ri;
	std::string zone;
	std::string name;
}key_t;


enum eLogState {
	Dropping,
	KeyFound,
	NoAnyKey,
	NoAnyLog,
};


enum eCurrentProgress {
	LoadingFiles,
	NoAnyLogFound,
	ReadingFile,
	ParsingData,
	Done
};

static constexpr const char* sCurrentProgress[5] = {
	"Loading Files",
	"No Any Log Found",
	"Reading File",
	"Parsing Data",
	"Done"
};

