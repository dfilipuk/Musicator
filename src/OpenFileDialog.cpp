#include "OpenFileDialog.h"



OpenFileDialog::OpenFileDialog()
{
}


OpenFileDialog::~OpenFileDialog()
{
}

char * OpenFileDialog::GetFilename(HWND hWnd)
{
	char *file = new char[MAX_PATH];
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFile = file;
	ofn.lpstrFile[0] = '\0';
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrTitle = "Select a file to play";
	ofn.lpstrFilter = "Playable files\0*.mp3;*.mp2;*.mp1;*.ogg;*.wav;*.aif\0";
	
	if (GetOpenFileName(&ofn)) {
		return file;
	}
	else {
		delete [] file;
		return NULL;
	}
}

