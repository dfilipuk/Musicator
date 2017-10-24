#include "Player.h"



Player::Player()
{
	isSongLoaded = false;
	isSongPlaying = false;
	hCurrentStream = 0;
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

void Player::StopSong()
{
	if (isSongPlaying) {
		BASS_ChannelStop(hCurrentStream);
		isSongPlaying = false;
	}
}

void Player::FreeSong()
{
	if (!isSongPlaying && isSongLoaded) {
		BASS_StreamFree(hCurrentStream);
		isSongLoaded = false;
	}
}

