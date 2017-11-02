#pragma once

#include "stdafx.h"

#define BTN_BACKWARD_ID 0
#define BTN_PLAY_ID 1
#define BTN_STOP_ID 2
#define BTN_PAUSE_ID 3
#define BTN_FORWARD_ID 4

enum ButtonsState { bsStopped, bsPlaying, bsPaused };

class GUIControls
{
private:
	HWND hWndBtnBackward, hWndBtnPlay, hWndBtnStop, hWndBtnPause, hWndBtnForward;
public:
	void SetButtonsState(ButtonsState bs);
	void CreateControls(HWND hWndParent);
	GUIControls();
	virtual ~GUIControls();
};

