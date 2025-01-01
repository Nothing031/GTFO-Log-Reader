#include "LogReader.h"

LogReader::LogReader()
{
	initSuccess = false;
}
LogReader::~LogReader() {
	logStream.close();
}

bool LogReader::Init(std::stack<CLEARINFO>* pStack)
{
	initSuccess = false;
	logPath = GetFilePath();
	if (logPath.empty()) return false;

	logStream = std::ifstream(this->logPath);
	if (!logStream.good())return false;

	synced = LogSynchronousCheck();
	if (!synced) return false;

	this->pClearInfoStack = pStack;

	lastStreamPos = 0;
	logData.clear();

	initSuccess = true;
	return true;
}
std::string LogReader::GetFilePath()
{
	wchar_t* localAppData = nullptr;
	SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, NULL, &localAppData);
	std::wstring ws(localAppData);
	std::string dirPath(ws.begin(), ws.end());
	dirPath += "\\10 Chambers Collective\\GTFO";

	std::vector<std::string> files;
	size_t pos;

	for (const auto& file : std::filesystem::directory_iterator(dirPath)) {
		if (file.path().filename().string().find("NICKNAME_NETSTATUS.") != std::string::npos) {
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
bool LogReader::LogSynchronousCheck()
{
	std::regex timeRegex("^\\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{3}.*$");

	std::ifstream file(logPath);
	
	std::string line;
	std::vector<std::string> lines;

	std::streampos lastPos = 0;
	std::streampos tempPos;
	
	file.clear();
	file.seekg(lastPos);
	getline(file, line);
	tempPos = file.tellg();
	if (tempPos != -1) lastPos = tempPos;
	while (getline(file, line)) {
		tempPos = file.tellg();
		if (tempPos != -1) {
			lastPos = tempPos;
		}
		lines.push_back(line);
	}

	// synchronous check;
	for (int i = lines.size() - 1; i >= 0; i--) {
		if (std::regex_match(lines[i], timeRegex)) {
			int logTime = gtfoTime_to_msTime(lines[i]);
			int currentTime = get_current_UTC_ms_time();
			int diff = std::abs(currentTime - logTime);
			if (diff < 10000) { // under 10sec
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

EXPEDITION_DATA LogReader::NewExpedition(const std::string& line)
{
	EXPEDITION_DATA newExpedition;

	int rundownId = std::stoi(line.substr(77, 2));
	switch (rundownId) {
	case 32: rundownId = 1; break;
	case 33: rundownId = 2; break;
	case 34: rundownId = 3; break;
	case 37: rundownId = 4; break;
	case 38: rundownId = 5; break;
	case 41: rundownId = 6; break;
	case 31: rundownId = 7; break;
	case 35: rundownId = 8; break;
	default: rundownId = 0; break;
	}
	if (rundownId != 0) {
		std::string tier = line.substr(84, 1);
		int expeditionId = std::stoi(line.substr(86, 1)) + 1;
		newExpedition.name = "R" + std::to_string(rundownId) + tier + std::to_string(expeditionId);
	}
	else {
		newExpedition.name = "?";
	}

	std::string strIncludingSeed = SubByKey(line, 88, "sessionGUID");
	std::istringstream seedStream(strIncludingSeed);
	std::string seedStr;
	seedStream >> seedStr >> seedStr;
	int seed = std::stoi(seedStr);
	newExpedition.seed = seed;

	std::string baseHashStr = line.substr(0, 12) + std::to_string(seed);
	std::hash<std::string> hashFn;
	newExpedition.hash = hashFn(baseHashStr);
	return newExpedition;
}

void LogReader::ReadFile()
{
	std::string line;
	std::streampos tempPos;

	logStream.clear();
	logStream.seekg(lastStreamPos);
	std::getline(logStream, line);
	tempPos = logStream.tellg();
	if (tempPos != -1)
		lastStreamPos = tempPos;
	
	while (std::getline(logStream, line)) {
		tempPos = logStream.tellg();
		if (tempPos != -1)
			lastStreamPos = tempPos;
		logData.push_back(line);
	}
}
void LogReader::ParseLog()
{
	if (lastCheckedIndex == logData.size()) return;

	// NoLobby
	// Lobby
	// Generating
	// ItemSpawning
	// ItemSpawned
	// StopElevatorRide
	// InLevel
	// ExpeditionDone

	for (; lastCheckedIndex < logData.size(); lastCheckedIndex++) {
		if (logData[lastCheckedIndex].find(Filters::Lobby) != NPOS) {
			expedition.Index.Lobby = lastCheckedIndex;
			expedition = NewExpedition(logData[lastCheckedIndex]);
			expedition.progress = eExpeditionProgress::Lobby;
		}
		else if (logData[lastCheckedIndex].find(Filters::ItemSpawning) != NPOS) {
			expedition.Index.itemSpawnStart = lastCheckedIndex;
			expedition.progress = eExpeditionProgress::ItemSpawning;
		}
		else if (logData[lastCheckedIndex].find(Filters::ItemSpawned) != NPOS) {
			expedition.Index.itemSpawnEnd = lastCheckedIndex;
			expedition.progress = eExpeditionProgress::ItemSpawned;
		}
		else if (logData[lastCheckedIndex].find(Filters::b_GameStateManager) != NPOS) {
			if (logData[lastCheckedIndex].find(Filters::c_Generating) != NPOS) {

			}
			else if (logData[lastCheckedIndex].find(Filters::c_NoLobby) != NPOS) {
				expedition.Index.NoLobby = lastCheckedIndex;
				expedition.progress = eExpeditionProgress::NoLobby;
			}
			else if (logData[lastCheckedIndex].find(Filters::c_StopElevatorRide) != NPOS) {
				expedition.Index.StopElevatorRide = lastCheckedIndex;
				expedition.progress = eExpeditionProgress::StopElevatorRide;
			}
			else if (logData[lastCheckedIndex].find(Filters::c_InLevel) != NPOS) {
				expedition.Index.InLevel = lastCheckedIndex;
				expedition.progress = eExpeditionProgress::InLevel;
				std::string gtfoTime = logData[expedition.Index.InLevel].substr(0, 12);
				expedition.expeditionStartTime = gtfoTime_to_msTime(gtfoTime);
			}
			else if (logData[lastCheckedIndex].find(Filters::c_ExpeditionDone) != NPOS) {
				expedition.Index.ExpeditionDone = lastCheckedIndex;
				expedition.progress = eExpeditionProgress::ExpeditionDone;
				std::string gtfoTime = logData[expedition.Index.ExpeditionDone].substr(0, 12);
				expedition.expeditoinEndTime = gtfoTime_to_msTime(gtfoTime);

				// export clear info
				CLEARINFO clearInfo;
				clearInfo.clearTime = msTime_to_gtfoTime(expedition.expeditoinEndTime - expedition.expeditionStartTime);
				clearInfo.expedition = expedition.name;
				if (logData[expedition.Index.ExpeditionDone].find(Filters::cc_ExpeditionSuccess) != NPOS)
					clearInfo.state = eExpeditionEndState::ExpeditionSuccess;
				else if (logData[expedition.Index.ExpeditionDone].find(Filters::cc_ExpeditionFail) != NPOS)
					clearInfo.state = eExpeditionEndState::ExpeditionFail;
				else
					clearInfo.state = eExpeditionEndState::ExpeditionAbort;
				
				pClearInfoStack->push(clearInfo);
			}
		}
	}
}
void LogReader::FindKey()
{
	std::vector<KEY> Keys;
	
	for (int i = expedition.Index.itemSpawnStart; i < expedition.Index.itemSpawnEnd; i++) {
		if (logData[i].find("CreateKeyItem") != NPOS) {
			KEY key;
			key.name = SubByKey(logData[i], 77, " SpawnedItem");
			i += 2;
			for (; logData[i].find("AREA RESULT") != NPOS; i++);
			key.zone = stoi(SubByKey(logData[i], "zone: ZONE", " function:"));
			key.index = stoi(SubByKey(logData[i], "ri: ", " had"));

			Keys.push_back(key);
		}
	}
	expedition.keys = Keys;
}