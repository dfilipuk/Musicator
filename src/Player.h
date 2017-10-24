#pragma once

#include "stdafx.h"

class Player
{
private:
	const int DEVICE = -1;
	const DWORD FREQUENCY = 44100;

	bool isSongLoaded;
	bool isSongPlaying;
	HSTREAM hCurrentStream;
public:
	Player();
	virtual ~Player();
	bool IsSongLoaded();
	bool IsSongPlaying();
	DWORD GetCurrentStreamHandler();
	bool InitializeDevice(HWND hWnd);
	bool LoadSong(char *file);
	bool PlaySong();
	void StopSong();
	void FreeSong();
};

