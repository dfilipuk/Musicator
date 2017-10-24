#pragma once

#include "stdafx.h"

class SpectrumVisualiser
{
private:
	const int LOGICAL_SPECTRUM_HEIGHT = 255;
	const int COLORS_AMOUNT_IN_PALETTE = 256;
	const int SPECTRUM_BACKGROUBD_COLOR_PALETTE_INDEX = 0;
	const int SPECTRUM_FIRST_COLOR_INDEX = 1;
	const int BITMAP_BITS_PER_PIXEL = 8;
	const int FFT_ELEMENTS_AMOUNT = 1024;
	const int SPECTRUM_HORIZONTAL_TRANSFORM_ADDITION_CONSTANT = -10;
	const float SPECTRUM_HORIZONTAL_TRANSFORM_MULTIPLICATION_CONSTANT = 2.5;
	const float MAX_2_POW = 10.0;

	unsigned int spectrumWidthPx;
	unsigned int spectrumHeightPx;
	unsigned int barsAmount;
	unsigned int barsDistancePx;
	unsigned int barWidthPx;
	unsigned int scaleCoefficient;
	unsigned int unusedHeight;

	BYTE *bitmapInfoBuffer;
	BYTE *spectrumBuffer;
	HBITMAP hSpectrumBitmap;
	HDC hSpectrumDC;
	HGDIOBJ hSpectrumInitialBitmap;
	BITMAPINFOHEADER *bitmapInfoHeader;
	RGBQUAD *palette;

	void CreateBitmap(HWND hWnd);
	void InitializeBitmapInfoHeader();
	void InitializePalette();

public:
	SpectrumVisualiser(HWND hWnd, int height, int barWidth, int barsAmo, int barsDist);
	virtual ~SpectrumVisualiser();
	bool DrawSpectrum(HWND hWnd, int x, int y, DWORD channel);
	void DrawZeroSpectrum(HWND hWnd, int x, int y);
};

