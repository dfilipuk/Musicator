#pragma once

#include "stdafx.h"

class OpenFileDialog
{
public:
	OpenFileDialog();
	virtual ~OpenFileDialog();
	char* GetFilename(HWND hWnd);
};

