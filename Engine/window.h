#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "logger.h"

class Window
{
public:
	Window();
	~Window();
	bool Create(const LPCWSTR lpCaption, int nWidth, int nHeight);
	void Release();

	int GetWidth() const;
	int GetHeight() const;
	HWND GetHandle() const;

private:
	HWND _hWnd;
	LPCWSTR _lpszClassName;
	HINSTANCE _hInstance;
	int _nWidth, _nHeight;
	bool _fCreated;
};