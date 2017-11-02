#include "Player.h"



Player::Player(int runStepSec)
{
	isSongLoaded = false;
	isSongPlaying = false;
	hCurrentStream = 0;
	this->runStepSec = runStepSec > 0 ? runStepSec : 0;
	runStepBytes = 0;
}


Player::~Player()
{
	StopSong();
	FreeSong();
	BASS_Free();
}

bool Player::IsSongLoaded()
{
	return isSongLoaded;
}

bool Player::IsSongPlaying()
{
	return isSongPlaying;
}

DWORD Player::GetCurrentStreamHandler()
{
	if (isSongLoaded) {
		return hCurrentStream;
	}
	else {
		return 0;
	}
}

bool Player::InitializeDevice(HWND hWnd)
{
	return BASS_Init(DEVICE, FREQUENCY, 0, hWnd, NULL);
}

bool Player::LoadSong(char * file)
{
	hCurrentStream = BASS_StreamCreateFile(FALSE, file, 0, 0, BASS_MUSIC_AUTOFREE);
	if (hCurrentStream == 0) {
		return false;
	}
	isSongLoaded = true;
	channelLengthBytes = BASS_ChannelGetLength(hCurrentStream, BASS_POS_BYTE);
	runStepBytes = BASS_ChannelSeconds2Bytes(hCurrentStream, runStepSec);

	return true;
}

bool Player::PlaySong()
{
	if (!isSongLoaded) {
		return false;
	}
	if (BASS_ChannelPlay(hCurrentStream, FALSE) == TRUE) {
		isSongPlaying = true;
		return true;
	}
	else {
		return false;
	}
}

void Player::PauseSong()
{
	if (isSongPlaying) {
		BASS_ChannelPause(hCurrentStream);
		isSongPlaying = false;
	}
}

void Player::StopSong()
{
	if (isSongPlaying) {
		BASS_ChannelStop(hCurrentStream);
		isSongPlaying = false;
	}
	FreeSong();
}

void Player::RunForward()
{
	int currentPositionBytes;
	if ((!isSongLoaded) || (runStepBytes <= 0) || (channelLengthBytes <= 0)) {
		return;
	}
	currentPositionBytes = BASS_ChannelGetPosition(hCurrentStream, BASS_POS_BYTE);
	currentPositionBytes = currentPositionBytes + runStepBytes > channelLengthBytes
		? channelLengthBytes - 1
		: currentPositionBytes + runStepBytes;
	BASS_ChannelSetPosition(hCurrentStream, currentPositionBytes, BASS_POS_BYTE);
}

void Player::RunBackward()
{
	int currentPositionBytes;
	if ((!isSongLoaded) || (runStepBytes <= 0) || (channelLengthBytes <= 0)) {
		return;
	}
	currentPositionBytes = BASS_ChannelGetPosition(hCurrentStream, BASS_POS_BYTE);
	currentPositionBytes = currentPositionBytes - runStepBytes <= 0
		? 1
		: currentPositionBytes - runStepBytes;
	BASS_ChannelSetPosition(hCurrentStream, currentPositionBytes, BASS_POS_BYTE);
}

void Player::FreeSong()
{
	if (!isSongPlaying && isSongLoaded) {
		BASS_StreamFree(hCurrentStream);
		isSongLoaded = false;
	}
}

