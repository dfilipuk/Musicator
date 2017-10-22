#include "SpectrumVisualiser.h"

#include <math.h>

SpectrumVisualiser::SpectrumVisualiser(HWND hWnd)
{
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
	bitmapInfoHeader->biWidth = SPECTRUM_WIDTH_PX;
	bitmapInfoHeader->biHeight = SPECTRUM_HEIGHT_PX;
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

	if (BASS_ChannelGetData(channel, fft, BASS_DATA_FFT2048) != -1) {
		int leftIntervalBorder = 0;
		int rightIntervalBorder;
		int currentElementInInterval;
		float maxValue;
		int spectrumValue;
		
		memset(spectrumBuffer, SPECTRUM_BACKGROUBD_COLOR_PALETTE_INDEX, SPECTRUM_HEIGHT_PX * SPECTRUM_WIDTH_PX);

		for (int currentBarNumber = 0; currentBarNumber < BARS_AMOUNT; currentBarNumber++) {
			maxValue = 0;
			rightIntervalBorder = (int)pow(2, currentBarNumber *MAX_2_POW / (BARS_AMOUNT - 1));
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

			spectrumValue = (int)(sqrt(maxValue) * SPECTRUM_HORIZONTAL_TRANSFORM_MULTIPLICATION_CONSTANT *
				SPECTRUM_HEIGHT_PX + SPECTRUM_HORIZONTAL_TRANSFORM_ADDITION_CONSTANT);
			if (spectrumValue > SPECTRUM_HEIGHT_PX) {
				spectrumValue = SPECTRUM_HEIGHT_PX;
			}

			while (spectrumValue > 0) {
				memset(spectrumBuffer + (spectrumValue - 1) * SPECTRUM_WIDTH_PX + currentBarNumber *
					(SPECTRUM_WIDTH_PX / BARS_AMOUNT), spectrumValue, SPECTRUM_WIDTH_PX / BARS_AMOUNT - BARS_DISTANCE_PX);
				spectrumValue--;
			}
		}
		BitBlt(hDC, x, y, SPECTRUM_WIDTH_PX, SPECTRUM_HEIGHT_PX, hSpectrumDC, 0, 0, SRCCOPY);
		result = true;
	}

	delete fft;
	ReleaseDC(hWnd, hDC);
	return result;
}

