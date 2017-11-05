#pragma once

#include "stdafx.h"

#define BTN_BACKWARD_ID 0
#define BTN_PLAY_ID 1
#define BTN_STOP_ID 2
#define BTN_PAUSE_ID 3
#define BTN_FORWARD_ID 4
#define LV_PLAYLIST_ID 5

enum ButtonsState { bsStopped, bsPlaying, bsPaused };

class GUIControls
{
private:
	HWND hWndBtnBackward, hWndBtnPlay, hWndBtnStop, hWndBtnPause, hWndBtnForward, hWndListView;
	HFONT font;
	int lvWidth, lvHeight;
	char *currentSongName;

	HFONT GetFont();
public:
	void SetButtonsState(ButtonsState bs);
	void CreateControls(HWND hWndParent);
	void AddElementToListView(char *song, char *length);
	void DeleteElementFromListView(int index);
	int GetSelectedListViewItemInd();
	void SetCurrentSongName(char *songName);
	void DrawSongName(HDC hDc);
	GUIControls();
	virtual ~GUIControls();
};

