#pragma comment(lib, "bass.lib")
#pragma comment(lib, "ComCtl32.Lib")

#include "stdafx.h"
#include "resource.h"
#include "SpectrumVisualiser.h"
#include "OpenFileDialog.h"
#include "Player.h"
#include "GUIControls.h"
#include "Song.h"
#include "Playlist.h"

#define SPECTRUM_HEIGHT_PX 275
#define SPECTRUM_BARS_AMOUNT 30
#define SPECTRUM_BAR_WIDTH_PX 16
#define SPECTRUM_BARS_DISTANCE_PX 2
#define SPECTRUM_X 0
#define SPECTRUM_Y 0
#define SPECTRUM_TIMER_INTERVAL 25

#define CLIENT_AREA_HEIGHT_PX 500
#define CLIENT_AREA_WIDTH_PX 490

void CALLBACK UpdateSpectrum(PVOID lpParametr, BOOLEAN TimerOrWaitFired);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND hMainWnd;
HANDLE hSpectrumUpdateTimer;
SpectrumVisualiser *spectrumVizualizer;
OpenFileDialog *openFileDialog;
Player *player;
Playlist *playlist;
GUIControls *controls;
bool isSpectrumTimerWorking = false;

void ShowError(HWND hWnd, const char* errorMessage) 
{
	MessageBox(hWnd, errorMessage, "Error", MB_OK);
}

void StartSpectrumTimer()
{
	if (!isSpectrumTimerWorking) {
		CreateTimerQueueTimer(&hSpectrumUpdateTimer, NULL, (WAITORTIMERCALLBACK)&UpdateSpectrum, NULL,
			SPECTRUM_TIMER_INTERVAL, SPECTRUM_TIMER_INTERVAL, 0);
		isSpectrumTimerWorking = true;
	}
}

void StopSpectrumTimer()
{
	if (isSpectrumTimerWorking) {
		if (DeleteTimerQueueTimer(NULL, hSpectrumUpdateTimer, NULL) != 0) {
			isSpectrumTimerWorking = false;
		}
		else if (GetLastError() == ERROR_IO_PENDING) {
			isSpectrumTimerWorking = false;
		}
	}
}

void StopPlayingSong(Player *player)
{
	StopSpectrumTimer();
	player->StopSong();
}

bool StartPlayingNewSong(char *file, Player *player)
{
	StopPlayingSong(player);
	if (!player->LoadSong(file)) {
		return false;
	}
	if (!player->PlaySong()) {
		return false;
	}
	StartSpectrumTimer();
	return true;
}

void AddSongAction(HWND hWnd) 
{
	Song *newSong;
	char *file = openFileDialog->GetFilename(hWnd);
	if (file == NULL) {
		return;
	}

	if (playlist->IsSongAlreadyExists(file)) {
		delete file;
		return;
	}

	int songLengthInSeconds = player->GetSongLengthInSeconds(file);
	if (songLengthInSeconds < 0) {
		ShowError(hWnd, "Unable to open file!");
		delete file;
		return;
	}

	newSong = new Song(file, songLengthInSeconds);
	playlist->AddSong(newSong);
	controls->AddElementToListView(newSong->GetFileName(), newSong->GetTime());
}

void StopSongAction(HWND hWnd)
{
	StopPlayingSong(player);
	controls->SetButtonsState(bsStopped);
	spectrumVizualizer->DrawZeroSpectrum(hWnd, SPECTRUM_X, SPECTRUM_Y);
}

void RemoveSongAction(HWND hWnd)
{
	int selectedSongIndex = controls->GetSelectedListViewItemInd();
	while (selectedSongIndex != -1) {
		controls->DeleteElementFromListView(selectedSongIndex);
		if (selectedSongIndex == playlist->GetCurrentSongIndex()) {
			StopSongAction(hWnd);
		}
		playlist->RemoveSongByIndex(selectedSongIndex);
		selectedSongIndex = controls->GetSelectedListViewItemInd();
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	MSG msg;
	HACCEL hAccelTable;

	if (!Player::IsMusicLibVersionCorrect()) {
		ShowError(0, "An incorrect version of BASS.DLL was loaded!");
		return 0;
	}

	controls = new GUIControls();
	player = new Player(RUN_STEP_SEC);
	playlist = new Playlist();
	openFileDialog = new OpenFileDialog();

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = "Musicator";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);
	hMainWnd = CreateWindow("Musicator", "Musicator", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE | WS_MINIMIZEBOX,
			CW_USEDEFAULT, 0, CLIENT_AREA_WIDTH_PX + 2 * GetSystemMetrics(SM_CXDLGFRAME), 
			CLIENT_AREA_HEIGHT_PX + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYDLGFRAME) + GetSystemMetrics(SM_CYMENU),
			NULL, NULL, hInstance, NULL);

	spectrumVizualizer = new SpectrumVisualiser(hMainWnd, SPECTRUM_HEIGHT_PX, SPECTRUM_BAR_WIDTH_PX,
			SPECTRUM_BARS_AMOUNT, SPECTRUM_BARS_DISTANCE_PX);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	delete player;
	delete controls;
	delete openFileDialog;
	delete spectrumVizualizer;
	delete playlist;

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PAINT:
		if (GetUpdateRect(hWnd, 0, 0)) {
			PAINTSTRUCT p;
			HDC dc;
			dc = BeginPaint(hWnd, &p);
			UpdateSpectrum(NULL, false);
			EndPaint(hWnd, &p);
		}
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_FILE_ADDSONG:
			AddSongAction(hWnd);
			break;
		case ID_FILE_REMOVESONG:
			RemoveSongAction(hWnd);
			break;
		case BTN_BACKWARD_ID:
			player->RunBackward();
			SetFocus(hWnd);
			break;
		case BTN_PLAY_ID:
			player->PlaySong();
			controls->SetButtonsState(bsPlaying);
			StartSpectrumTimer();
			break;
		case BTN_STOP_ID:
			StopSongAction(hWnd);
			break;
		case BTN_PAUSE_ID:
			player->PauseSong();
			controls->SetButtonsState(bsPaused);
			StopSpectrumTimer();
			InvalidateRect(hWnd, NULL, true);
			break;
		case BTN_FORWARD_ID:
			player->RunForward();
			SetFocus(hWnd);
			break;
		}
		break;
	}
	case WM_NOTIFY:
		LPNMHDR lpnmHdr;
		lpnmHdr = (LPNMHDR)lParam;
		if ((lpnmHdr->idFrom == LV_PLAYLIST_ID) && (lpnmHdr->code == NM_DBLCLK))
		{
			int selectedSongIndex = controls->GetSelectedListViewItemInd();
			Song *selectedSong = playlist->GetSongByIndex(selectedSongIndex);
			if (selectedSong != NULL) {
				playlist->SetCurrentSongIndex(selectedSongIndex);
				StartPlayingNewSong(selectedSong->GetFilePath(), player);
				controls->SetButtonsState(bsPlaying);
			}
		}
		SetFocus(hWnd);
		break;
	case WM_CREATE:
		if (!(player->InitializeDevice(hWnd))) {
			ShowError(hWnd, "Can't initialize device!");
			return -1;
		}
		controls->CreateControls(hWnd);
		break;
	case WM_DESTROY:
		StopPlayingSong(player);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CALLBACK UpdateSpectrum(PVOID lpParametr, BOOLEAN TimerOrWaitFired)
{
	if (player->IsSongLoaded()) {
		bool isStopped = !spectrumVizualizer->DrawSpectrum(hMainWnd, SPECTRUM_X, SPECTRUM_Y, player->GetCurrentStreamHandler());
		if (isStopped) {
			StopPlayingSong(player);
			controls->SetButtonsState(bsStopped);
			Song *nextSong = playlist->GetNextSong();
			if (nextSong != NULL) {
				StartPlayingNewSong(nextSong->GetFilePath(), player);
				controls->SetButtonsState(bsPlaying);
			}
		}
	}
	else {
		spectrumVizualizer->DrawZeroSpectrum(hMainWnd, SPECTRUM_X, SPECTRUM_Y);
	}
}

