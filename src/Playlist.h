#pragma once

#include <vector>
#include "Song.h"

class Playlist
{
private:
	std::vector<Song *> *songs;
	int currentSongIndex;

	bool IsIndexValid(int index);
public:
	Playlist();
	virtual ~Playlist();
	bool IsSongAlreadyExists(char *filePath);
	void AddSong(Song *newSong);
	void RemoveSongByIndex(int index);
	Song* GetSongByIndex(int index);
	int GetCurrentSongIndex();
	void SetCurrentSongIndex(int index);
	Song* GetNextSong();
};

