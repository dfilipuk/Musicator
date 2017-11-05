#include "Song.h"

#include <stdio.h>
#include <string.h>

#define BACKSLASH '\\'
#define SECONDS_IN_MINUTE 60
#define MINUTES_IN_HOUR 60
#define FORMATTED_TIME_STRING_LENGTH 8

char * Song::GetFileName(char * filePath)
{
	int pathLength = strlen(filePath);
	int backSlashIndex = pathLength;
	bool isFound = false;
	while ((backSlashIndex >= 0) && (!isFound)) {
		if (filePath[backSlashIndex] == BACKSLASH) {
			isFound = true;
		}
		else {
			backSlashIndex--;
		}
	}
	if (backSlashIndex >= 0) {
		int filenameStartIndex = backSlashIndex + 1;
		int filenameLength = pathLength - filenameStartIndex;
		char *filename = new char[filenameLength + 1];
		strcpy_s(filename, filenameLength + 1, &filePath[filenameStartIndex]);
		return filename;
	}
	else {
		return NULL;
	}
}

char * Song::GetFormattedTime(int length)
{
	length = length < 0 ? 0 : length;
	int seconds = length % SECONDS_IN_MINUTE;
	int minutes = (length / SECONDS_IN_MINUTE) % MINUTES_IN_HOUR;
	int hours = (length / SECONDS_IN_MINUTE) / MINUTES_IN_HOUR;
	char *buffer = new char[FORMATTED_TIME_STRING_LENGTH];
	sprintf_s(buffer, FORMATTED_TIME_STRING_LENGTH, "%1d:%02d:%02d", hours, minutes, seconds);
	return buffer;
}

Song::Song(char *filePath, int length)
{
	this->filePath = filePath;
	fileName = GetFileName(filePath);
	formattedTime = GetFormattedTime(length);
}


Song::~Song()
{
	delete [] filePath;
	delete [] formattedTime;
	if (fileName != NULL) {
		delete [] fileName;
	}
}

char * Song::GetFilePath()
{
	return filePath;
}

char * Song::GetFileName()
{
	return fileName;
}

char * Song::GetTime()
{
	return formattedTime;
}
