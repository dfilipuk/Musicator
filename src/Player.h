#pragma once

#include "stdafx.h"

#define RUN_STEP_SEC 10

class Player
{
private:
	const int DEVICE = -1;
	const DWORD FREQUENCY = 44100;

	int channelLengthBytes, runStepSec, runStepBytes;
	bool isSongLoaded;
	bool isSongPlaying;
	HSTREAM hCurrentStream;

	void FreeSong();
public:
	Player(int runStepSec);
	virtual ~Player();
	bool IsSongLoaded();
	bool IsSongPlaying();
	DWORD GetCurrentStreamHandler();
	bool InitializeDevice(HWND hWnd);
	bool LoadSong(char *file);
	bool PlaySong();
	void PauseSong();
	void StopSong();
	void RunForward();
	void RunBackward();
};

