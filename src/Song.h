#pragma once
class Song
{
private:
	char *filePath, *fileName, *formattedTime;

	char* GetFileNameFromPath(char *filePath);
	char* GetFormattedTime(int length);
public:
	Song(char *filePath, int length);
	virtual ~Song();
	char* GetFilePath();
	char* GetFileName();
	char* GetTime();
};

