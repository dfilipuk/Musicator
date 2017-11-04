#include "Playlist.h"



bool Playlist::IsIndexValid(int index)
{
	return ((index >= 0) || (index < songs->size()));
}

Playlist::Playlist() : currentSongIndex(0)
{
	songs = new std::vector<Song *>();
}


Playlist::~Playlist()
{
	for (auto iterator = songs->begin(); iterator != songs->end(); iterator++) {
		delete (*iterator);
	}
	delete songs;
}

bool Playlist::IsSongAlreadyExists(char * filePath)
{
	for (auto iterator = songs->begin(); iterator != songs->end(); iterator++) {
		if (strcmp((*iterator)->GetFilePath(), filePath) == 0) {
			return true;
		}
	}
	return false;
}

void Playlist::AddSong(Song * newSong)
{
	songs->push_back(newSong);
}

void Playlist::RemoveSongByIndex(int index)
{
	if (IsIndexValid(index)) {
		auto removingItemPosition = songs->begin() + index;
		Song *removingItem = *removingItemPosition;
		songs->erase(removingItemPosition);
		delete removingItem;
		if (index == currentSongIndex) {
			currentSongIndex = 0;
		}
		else if (index < currentSongIndex) {
			currentSongIndex--;
		}
	}
}

Song * Playlist::GetSongByIndex(int index)
{
	if (!IsIndexValid(index)) {
		return NULL;
	}
	return (*songs)[index];
}

int Playlist::GetCurrentSongIndex()
{
	return currentSongIndex;
}

void Playlist::SetCurrentSongIndex(int index)
{
	if (IsIndexValid(index)) {
		currentSongIndex = index;
	}
	else {
		currentSongIndex = 0;
	}
}
