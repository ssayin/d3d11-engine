#include "window.h"
#include "graphics.h"
#include "enginebase.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	Window* pcWindow = new Window;
	Graphics* pcGraphics = new Graphics;

	pcWindow->Create(L"Engine", 800, 600);
	pcGraphics->Initialize(pcWindow, 1000.0f, 1.0f, true);

	MSG msg = {0};

	do
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			pcGraphics->BeginScene(0.5f, 0.5f, 1.0f, 1.0f);

			pcGraphics->EndScene();
		}

	}while(msg.message != WM_QUIT);

	_RELEASE_AND_DELETE(pcGraphics)
	_RELEASE_AND_DELETE(pcWindow)

	return 0;
}