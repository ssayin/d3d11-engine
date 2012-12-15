#include "window.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Window::Window() : _hWnd(NULL), 
		   _nWidth(0), 
		   _nHeight(0),
		   _fCreated(false)
{
	_lpszClassName = L"Engine";
	_hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wcex;
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.hInstance = _hInstance;
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = _lpszClassName;
	wcex.lpszMenuName = 0;
	wcex.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	if(!RegisterClassEx(&wcex))
	{
		LOG_ERROR("Window::Window(): Could not register class.")
	}
}

Window::~Window()
{
	Release();

	if(!UnregisterClass(_lpszClassName, _hInstance))
	{
		LOG_ERROR("Window::~Window(): Could not unregister class.")
	}
}

bool Window::Create(const LPCWSTR lpCaption, int nWidth, int nHeight)
{
	if(_fCreated)
	{
		LOG_WARNING("Window::Create(): Window already created!")
		return false;
	}
	
	_nWidth = nWidth;
	_nHeight = nHeight;

	_hWnd = CreateWindow(_lpszClassName, 
						lpCaption, 
						WS_OVERLAPPEDWINDOW,
						(GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2,
						(GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2,
						nWidth, nHeight, NULL, NULL, _hInstance, NULL);

	if(!_hWnd)
	{
		LOG_ERROR("Window::Window(): Window not created.")
		return false;
	}

	ShowWindow(_hWnd, SW_SHOW);
	SetForegroundWindow(_hWnd);
	SetFocus(_hWnd);
	UpdateWindow(_hWnd);

	_fCreated = true;

	LOG_INFO("Window created.")
	return true;
}

void Window::Release()
{
	_fCreated = false;
	LOG_INFO("Window released.")
}

int Window::GetWidth() const
{
	return _nWidth;
}

int Window::GetHeight() const
{
	return _nHeight;
}

HWND Window::GetHandle() const
{
	return _hWnd;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_ACTIVATE:
		return 0;

	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}