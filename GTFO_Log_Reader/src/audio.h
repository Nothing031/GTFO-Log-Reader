
#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include "resource.h"


#pragma comment(lib, "winmm.lib")

class audio {
public:
	enum eAUDIO {
		Intro,
		Ping,
		Type
	};

	static bool initSuccess;

	static LPVOID pIntroWav;
	static LPVOID pPingWav;
	static LPVOID pTypeWav;

	static void Init();
	static void Play(eAUDIO audio);
};










