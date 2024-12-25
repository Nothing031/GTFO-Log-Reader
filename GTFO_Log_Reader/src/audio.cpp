#include "audio.h"
#include <iostream>

bool audio::initSuccess = false;

LPVOID audio::pIntroWav = nullptr;
LPVOID audio::pPingWav = nullptr;
LPVOID audio::pTypeWav = nullptr;

void audio::Init() {
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

void audio::Play(eAUDIO audio) {
	
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