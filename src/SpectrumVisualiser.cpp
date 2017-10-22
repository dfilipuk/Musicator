#include "SpectrumVisualiser.h"

#include <math.h>
#include <memory.h>

SpectrumVisualiser::SpectrumVisualiser(HWND hWnd, int height, int barWidth, int barsAmo, int barsDist)
{
	barWidthPx = barWidth;
	barsAmount = barsAmo;
	barsDistancePx = barsDist;
	spectrumWidthPx = barsAmount * barWidth;
	spectrumHeightPx = height;
	
	scaleCoefficient = spectrumHeightPx / LOGICAL_SPECTRUM_HEIGHT;
	unusedHeight = spectrumHeightPx % LOGICAL_SPECTRUM_HEIGHT;
	
	bitmapInfoBuffer = new BYTE[sizeof(BITMAPINFOHEADER) + COLORS_AMOUNT_IN_PALETTE * sizeof(RGBQUAD)];
	bitmapInfoHeader = (BITMAPINFOHEADER *)bitmapInfoBuffer;
	palette = (RGBQUAD *)(bitmapInfoBuffer + sizeof(BITMAPINFOHEADER));
	CreateBitmap(hWnd);
}

SpectrumVisualiser::~SpectrumVisualiser()
{
	delete bitmapInfoBuffer;
	SelectObject(hSpectrumDC, hSpectrumInitialBitmap);
	DeleteDC(hSpectrumDC);
	DeleteObject(hSpectrumBitmap);
}

void SpectrumVisualiser::CreateBitmap(HWND hWnd)
{
	HDC hDC;

	InitializeBitmapInfoHeader();
	InitializePalette();

	hSpectrumBitmap = CreateDIBSection(0, (BITMAPINFO *)bitmapInfoBuffer, DIB_RGB_COLORS, (void **)&spectrumBuffer, NULL, 0);
	hDC = GetDC(hWnd);
	hSpectrumDC = CreateCompatibleDC(hDC);
	hSpectrumInitialBitmap = SelectObject(hSpectrumDC, hSpectrumBitmap);
	ReleaseDC(hWnd, hDC);
}

void SpectrumVisualiser::InitializeBitmapInfoHeader()
{
	bitmapInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader->biWidth = spectrumWidthPx;
	bitmapInfoHeader->biHeight = spectrumHeightPx;
	bitmapInfoHeader->biPlanes = 1;
	bitmapInfoHeader->biBitCount = BITMAP_BITS_PER_PIXEL;
	bitmapInfoHeader->biCompression = BI_RGB;
	bitmapInfoHeader->biSizeImage = 0;
	bitmapInfoHeader->biClrUsed = COLORS_AMOUNT_IN_PALETTE;
	bitmapInfoHeader->biClrImportant = COLORS_AMOUNT_IN_PALETTE;
	bitmapInfoHeader->biXPelsPerMeter = 0;
	bitmapInfoHeader->biYPelsPerMeter = 0;
}

void SpectrumVisualiser::InitializePalette()
{
	palette[SPECTRUM_BACKGROUBD_COLOR_PALETTE_INDEX].rgbBlue = 0;
	palette[SPECTRUM_BACKGROUBD_COLOR_PALETTE_INDEX].rgbGreen = 0;
	palette[SPECTRUM_BACKGROUBD_COLOR_PALETTE_INDEX].rgbRed = 0;
	for (int i = 1; i < COLORS_AMOUNT_IN_PALETTE; i++) {
		palette[i].rgbGreen = 256 - i;
		palette[i].rgbRed = i;
		palette[i].rgbBlue = 0;
	}
}

bool SpectrumVisualiser::DrawSpectrum(HWND hWnd, int x, int y, DWORD channel)
{
	HDC hDC = GetDC(hWnd);
	float *fft = new float[FFT_ELEMENTS_AMOUNT];
	bool result = false;

	memset(spectrumBuffer, SPECTRUM_BACKGROUBD_COLOR_PALETTE_INDEX, spectrumHeightPx * spectrumWidthPx);

	if (BASS_ChannelGetData(channel, fft, BASS_DATA_FFT2048) != -1) {
		int leftIntervalBorder = 0;
		int rightIntervalBorder;
		int currentElementInInterval;
		float maxValue;
		int spectrumColorValue;
		int spectrumHeightValue;
		int currentColorUsageCounter;
		
		for (int currentBarNumber = 0; currentBarNumber < barsAmount; currentBarNumber++) {
			maxValue = 0;
			rightIntervalBorder = (int)pow(2, currentBarNumber * MAX_2_POW / (barsAmount - 1));
			if (rightIntervalBorder <= leftIntervalBorder) {
				rightIntervalBorder = leftIntervalBorder + 1;
			}
			if (rightIntervalBorder > (FFT_ELEMENTS_AMOUNT - 1)) {
				rightIntervalBorder = FFT_ELEMENTS_AMOUNT - 1;
			}
			currentElementInInterval = leftIntervalBorder;
			while (currentElementInInterval < rightIntervalBorder) {
				if (maxValue < fft[currentElementInInterval]) {
					maxValue = fft[currentElementInInterval];
				}
				currentElementInInterval++;
			}
			leftIntervalBorder = currentElementInInterval;

			spectrumColorValue = (int)(sqrt(maxValue) * SPECTRUM_HORIZONTAL_TRANSFORM_MULTIPLICATION_CONSTANT *
				LOGICAL_SPECTRUM_HEIGHT + SPECTRUM_HORIZONTAL_TRANSFORM_ADDITION_CONSTANT);
			if (spectrumColorValue > LOGICAL_SPECTRUM_HEIGHT) {
				spectrumColorValue = LOGICAL_SPECTRUM_HEIGHT;
			}
			if (spectrumColorValue < SPECTRUM_FIRST_COLOR_INDEX) {
				spectrumColorValue = SPECTRUM_FIRST_COLOR_INDEX;
			}

			spectrumHeightValue = spectrumColorValue * scaleCoefficient + unusedHeight;
			currentColorUsageCounter = 0;
			while (spectrumHeightValue > 0) {
				if ((spectrumColorValue > SPECTRUM_FIRST_COLOR_INDEX) && (currentColorUsageCounter == scaleCoefficient)) {
					currentColorUsageCounter = 0;
					spectrumColorValue--;
				}

				memset(spectrumBuffer + (spectrumHeightValue - 1) * spectrumWidthPx + currentBarNumber * (barWidthPx), 
					spectrumColorValue, barWidthPx - barsDistancePx);

				spectrumHeightValue--;
				currentColorUsageCounter++;
			}
		}
		result = true;
	}
	else {
		int zeroSpectrumHeight;
		for (int i = 0; i < barsAmount; i++) {
			zeroSpectrumHeight = unusedHeight;
			while (zeroSpectrumHeight > 0) {
				memset(spectrumBuffer + (zeroSpectrumHeight - 1) * spectrumWidthPx + i * (barWidthPx),
					SPECTRUM_FIRST_COLOR_INDEX, barWidthPx - barsDistancePx);
				zeroSpectrumHeight--;
			}
		}
	}

	BitBlt(hDC, x, y, spectrumWidthPx, spectrumHeightPx, hSpectrumDC, 0, 0, SRCCOPY);

	delete fft;
	ReleaseDC(hWnd, hDC);
	return result;
}

