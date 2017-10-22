#pragma comment(lib, "bass.lib")

#include "stdafx.h"
#include "SpectrumVisualiser.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SPECTRUM_HEIGHT_PX 275
#define SPECTRUM_BARS_AMOUNT 30
#define SPECTRUM_BAR_WIDTH_PX 19
#define SPECTRUM_BARS_DISTANCE_PX 2

#define CLIENT_CONTROLS_HEIGHT_PX 0
#define SPECTRUM_X 0
#define SPECTRUM_Y 0
#define TIMER_INTERVAL 25

void CALLBACK UpdateSpectrum(PVOID lpParametr, BOOLEAN TimerOrWaitFired);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND hMainWnd;
SpectrumVisualiser *spectrumVizualizer;
HANDLE TIMER;
DWORD chan;

void ShowError(HWND hWnd, const char* errorMessage) 
{
	MessageBox(hWnd, errorMessage, "Error", MB_OK);
}

BOOL PlayFile(HWND hWnd)
{
	char file[MAX_PATH] = "";
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = file;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
	ofn.lpstrTitle = "Select a file to play";
	ofn.lpstrFilter = "playable files\0*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp3;*.mp2;*.mp1;*.ogg;*.wav;*.aif\0All files\0*.*\0\0";
	if (!GetOpenFileName(&ofn)) return FALSE;

	if (!(chan = BASS_StreamCreateFile(FALSE, file, 0, 0, BASS_MUSIC_AUTOFREE))
		&& !(chan = BASS_MusicLoad(FALSE, file, 0, 0, BASS_MUSIC_RAMP | BASS_MUSIC_AUTOFREE | BASS_MUSIC_PRESCAN, 1))) {
		ShowError(hWnd, "Can't play file");
		return FALSE;
	}

	QWORD length = BASS_ChannelGetLength(chan, BASS_POS_BYTE);
	double time = BASS_ChannelBytes2Seconds(chan, length);
	QWORD pos = BASS_ChannelSeconds2Bytes(chan, time - 5);
	BASS_ChannelSetPosition(chan, pos, BASS_POS_BYTE);

	BASS_ChannelPlay(chan, FALSE);

	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	MSG msg;
	int spectrumWidth = SPECTRUM_BARS_AMOUNT * SPECTRUM_BAR_WIDTH_PX + (SPECTRUM_BARS_AMOUNT) * SPECTRUM_BARS_DISTANCE_PX;

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
			CW_USEDEFAULT, 0, spectrumWidth + 50 + 2 * GetSystemMetrics(SM_CXDLGFRAME), 
			SPECTRUM_HEIGHT_PX + CLIENT_CONTROLS_HEIGHT_PX + GetSystemMetrics(SM_CYCAPTION) + 2 * GetSystemMetrics(SM_CYDLGFRAME), 
			NULL, NULL, hInstance, NULL);

	spectrumVizualizer = new SpectrumVisualiser(hMainWnd, SPECTRUM_HEIGHT_PX, SPECTRUM_BAR_WIDTH_PX,
			SPECTRUM_BARS_AMOUNT, SPECTRUM_BARS_DISTANCE_PX);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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
		if (!BASS_Init(-1, 44100, 0, hWnd, NULL)) {
			ShowError(hWnd, "Can't initialize device");
			return -1;
		}
		if (!PlayFile(hWnd)) {
			BASS_Free();
			return -1;
		}
		CreateTimerQueueTimer(&TIMER, NULL, (WAITORTIMERCALLBACK)&UpdateSpectrum, NULL, TIMER_INTERVAL, TIMER_INTERVAL, 0);
		break;
	case WM_DESTROY:
		DeleteTimerQueueTimer(NULL, TIMER, NULL);
		BASS_Free();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CALLBACK UpdateSpectrum(PVOID lpParametr, BOOLEAN TimerOrWaitFired)
{
	spectrumVizualizer->DrawSpectrum(hMainWnd, SPECTRUM_X, SPECTRUM_Y, chan);
}

