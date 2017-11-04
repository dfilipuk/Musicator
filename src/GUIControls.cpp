#include "GUIControls.h"

#define BTN_WIDTH 74
#define BTN_HEIGHT 25
#define BTN_Y 300
#define BTN_BACKWARD_X 15
#define BTN_PLAY_X 109
#define BTN_STOP_X 203
#define BTN_PAUSE_X 297
#define BTN_FORWARD_X 391
#define LV_PLAYLIST_X 0
#define LV_PLAYLIST_Y 330
#define LV_EXTRA_EMPTY_SPACE_WIDTH 20
#define LVC_SONGLENGTH_WIDTH 100
#define LVC_SONGLENGTH_TEXTMAXLENGTH 10
#define LVC_SONG_TEXTMAXLENGTH 50
#define LVC_SONGLENGTH_TITLE "Length"
#define LVC_SONG_TITLE "Song"
#define LVC_SONG_IND 0
#define LVC_SONGLENGTH_IND 1

GUIControls::GUIControls() : lvHeight(0), lvWidth(0)
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
	INITCOMMONCONTROLSEX icex;
	LVCOLUMN lvcSong, lvcSongLength;
	RECT clientRect;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);
	GetClientRect(hWndParent, &clientRect);
	lvWidth = clientRect.right - clientRect.left;
	lvHeight = clientRect.bottom - LV_PLAYLIST_Y;
	
	hWndListView = CreateWindow(WC_LISTVIEW, "Playlist", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY | LVS_REPORT | WS_CLIPSIBLINGS | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS,
		LV_PLAYLIST_X, LV_PLAYLIST_Y, lvWidth, lvHeight, hWndParent, (HMENU)LV_PLAYLIST_ID, 
		(HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE), NULL);
	ListView_SetExtendedListViewStyleEx(hWndListView, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	lvcSong.mask = LVCF_TEXT | LVCF_WIDTH;
	lvcSongLength.mask = LVCF_TEXT | LVCF_WIDTH;
	lvcSong.cx = lvWidth - LVC_SONGLENGTH_WIDTH - LV_EXTRA_EMPTY_SPACE_WIDTH;
	lvcSongLength.cx = LVC_SONGLENGTH_WIDTH;
	lvcSong.cchTextMax = LVC_SONG_TEXTMAXLENGTH;
	lvcSongLength.cchTextMax = LVC_SONGLENGTH_TEXTMAXLENGTH;
	lvcSong.pszText = LVC_SONG_TITLE;
	lvcSongLength.pszText = LVC_SONGLENGTH_TITLE;
	ListView_InsertColumn(hWndListView, LVC_SONG_IND, &lvcSong);
	ListView_InsertColumn(hWndListView, LVC_SONGLENGTH_IND, &lvcSongLength);

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

void GUIControls::AddElementToListView(char * song, char * length)
{
	LVITEM lvi;
	int newElementIndex = ListView_GetItemCount(hWndListView);
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = LVC_SONG_TEXTMAXLENGTH;
	lvi.iItem = newElementIndex;
	lvi.pszText = song;
	lvi.iSubItem = LVC_SONG_IND;
	ListView_InsertItem(hWndListView, &lvi);
	ListView_SetItemText(hWndListView, newElementIndex, LVC_SONGLENGTH_IND, length);
}

void GUIControls::DeleteElementFromListView(int index)
{
	int itemsAmount = ListView_GetItemCount(hWndListView);
	if (index < itemsAmount) {
		ListView_DeleteItem(hWndListView, index);
	}
}

int GUIControls::GetSelectedListViewItemInd()
{
	return ListView_GetNextItem(hWndListView, -1, LVNI_ALL | LVNI_SELECTED);
}

