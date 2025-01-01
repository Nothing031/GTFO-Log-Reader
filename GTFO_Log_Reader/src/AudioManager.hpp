#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include "resource.h"

#pragma comment(lib, "winmm.lib")

namespace AudioManager {
	enum eAUDIO {
		Intro,
		Ping,
		Type
	};

	static bool initSuccess;
	static LPVOID pIntroWav;
	static LPVOID pPingWav;
	static LPVOID pTypeWav;

	static void Init() {
		initSuccess = false;
		HRSRC hRes;
		HGLOBAL hGlobal;

		hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_WAVE1), L"WAVE");
		hGlobal = LoadResource(NULL, hRes);
		pIntroWav = LockResource(hGlobal);

		hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_WAVE2), L"WAVE");
		hGlobal = LoadResource(NULL, hRes);
		pPingWav = LockResource(hGlobal);

		hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_WAVE3), L"WAVE");
		hGlobal = LoadResource(NULL, hRes);
		pTypeWav = LockResource(hGlobal);

		initSuccess = true;
	}
	static void Play(eAUDIO audio) {

		switch (audio) {
		case eAUDIO::Intro:
			PlaySoundA((LPCSTR)pIntroWav, NULL, SND_MEMORY | SND_ASYNC);
			break;

		case eAUDIO::Ping:
			PlaySoundA((LPCSTR)pPingWav, NULL, SND_MEMORY | SND_ASYNC);
			break;

		case eAUDIO::Type:
			PlaySoundA((LPCSTR)pTypeWav, NULL, SND_MEMORY | SND_ASYNC);
			break;
		}
	}
};