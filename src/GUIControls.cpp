#include "GUIControls.h"

#define BTN_WIDTH 74
#define BTN_HEIGHT 25
#define BTN_Y 300
#define BTN_BACKWARD_X 15
#define BTN_PLAY_X 109
#define BTN_STOP_X 203
#define BTN_PAUSE_X 297
#define BTN_FORWARD_X 391

GUIControls::GUIControls()
{
}


GUIControls::~GUIControls()
{
}

void GUIControls::SetButtonsState(ButtonsState bs)
{
	bool isBtnBackwardEnabled = false;
	bool isBtnPlayEnabled = false;
	bool isBtnStopEnabled = false;
	bool isBtnPauseEnabled = false;
	bool isBtnForwardEnabled = false;

	switch (bs) {
	case bsPlaying:
		isBtnBackwardEnabled = true;
		isBtnForwardEnabled = true;
		isBtnStopEnabled = true;
		isBtnPauseEnabled = true;
		break;
	case bsPaused:
		isBtnPlayEnabled = true;
		break;
	}

	EnableWindow(hWndBtnBackward, isBtnBackwardEnabled);
	EnableWindow(hWndBtnPlay, isBtnPlayEnabled);
	EnableWindow(hWndBtnStop, isBtnStopEnabled);
	EnableWindow(hWndBtnPause, isBtnPauseEnabled);
	EnableWindow(hWndBtnForward, isBtnBackwardEnabled);
}

void GUIControls::CreateControls(HWND hWndParent)
{
	hWndBtnBackward = CreateWindow("BUTTON", "- 10 sec", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		BTN_BACKWARD_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, hWndParent, (HMENU)BTN_BACKWARD_ID,
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE), NULL);
	hWndBtnPlay = CreateWindow("BUTTON", "Play", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		BTN_PLAY_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, hWndParent, (HMENU)BTN_PLAY_ID,
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE), NULL);
	hWndBtnStop = CreateWindow("BUTTON", "Stop", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		BTN_STOP_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, hWndParent, (HMENU)BTN_STOP_ID,
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE), NULL);
	hWndBtnPause = CreateWindow("BUTTON", "Pause", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		BTN_PAUSE_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, hWndParent, (HMENU)BTN_PAUSE_ID,
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE), NULL);
	hWndBtnForward = CreateWindow("BUTTON", "+ 10 sec", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		BTN_FORWARD_X, BTN_Y, BTN_WIDTH, BTN_HEIGHT, hWndParent, (HMENU)BTN_FORWARD_ID,
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE), NULL);
	SetButtonsState(bsStopped);
}

