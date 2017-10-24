#pragma comment(lib, "bass.lib")

#include "stdafx.h"
#include "SpectrumVisualiser.h"
#include "OpenFileDialog.h"
#include "Player.h"

#define SPECTRUM_HEIGHT_PX 275
#define SPECTRUM_BARS_AMOUNT 30
#define SPECTRUM_BAR_WIDTH_PX 16
#define SPECTRUM_BARS_DISTANCE_PX 2

#define CLIENT_AREA_HEIGHT_PX 275
#define CLIENT_AREA_WIDTH_PX 490
#define SPECTRUM_X 0
#define SPECTRUM_Y 0
#define TIMER_INTERVAL 25

void CALLBACK UpdateSpectrum(PVOID lpParametr, BOOLEAN TimerOrWaitFired);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND hMainWnd;
SpectrumVisualiser *spectrumVizualizer;
OpenFileDialog *openFileDialog;
Player *player;
HANDLE TIMER;
DWORD chan;

void ShowError(HWND hWnd, const char* errorMessage) 
{
	MessageBox(hWnd, errorMessage, "Error", MB_OK);
}

bool PlayFile(HWND hWnd)
{
	char *file = openFileDialog->GetFilename(hWnd);
	if (file == NULL) {
		return false;
	}

	if (!player->LoadSong(file)) {
		delete file;
		return false;
	}
	delete file;
	if (!player->PlaySong()) {
		delete file;
		return false;
	}
	return true;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	MSG msg;

	if (HIWORD(BASS_GetVersion()) != BASSVERSION) {
		ShowError(0, "An incorrect version of BASS.DLL was loaded!");
		return 0;
	}

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "Musicator";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);
	hMainWnd = CreateWindow("Musicator", "Musicator", WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE | WS_MINIMIZEBOX,
			CW_USEDEFAULT, 0, CLIENT_AREA_WIDTH_PX + 2 * GetSystemMetrics(SM_CXDLGFRAME), 
			CLIENT_AREA_HEIGHT_PX + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYDLGFRAME), 
			NULL, NULL, hInstance, NULL);

	spectrumVizualizer = new SpectrumVisualiser(hMainWnd, SPECTRUM_HEIGHT_PX, SPECTRUM_BAR_WIDTH_PX,
			SPECTRUM_BARS_AMOUNT, SPECTRUM_BARS_DISTANCE_PX);
	openFileDialog = new OpenFileDialog();

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete openFileDialog;
	delete spectrumVizualizer;

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
			spectrumVizualizer->DrawSpectrum(hMainWnd, SPECTRUM_X, SPECTRUM_Y, chan);
			EndPaint(hWnd, &p);
		}
		break;
	case WM_CREATE:
		player = new Player();
		if (!(player->InitializeDevice(hWnd))) {
			ShowError(hWnd, "Can't initialize device!");
			return -1;
		}
		if (!PlayFile(hWnd)) {
			return -1;
		}
		CreateTimerQueueTimer(&TIMER, NULL, (WAITORTIMERCALLBACK)&UpdateSpectrum, NULL, TIMER_INTERVAL, TIMER_INTERVAL, 0);
		break;
	case WM_DESTROY:
		DeleteTimerQueueTimer(NULL, TIMER, NULL);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CALLBACK UpdateSpectrum(PVOID lpParametr, BOOLEAN TimerOrWaitFired)
{
	if (player->IsSongPlaying()) {
		spectrumVizualizer->DrawSpectrum(hMainWnd, SPECTRUM_X, SPECTRUM_Y, player->GetCurrentStreamHandler());
	}
	else {
		spectrumVizualizer->DrawZeroSpectrum(hMainWnd, SPECTRUM_X, SPECTRUM_Y);
	}
}

