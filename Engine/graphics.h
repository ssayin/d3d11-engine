#pragma once

#define  _XM_NO_INTRINSICS_

#include <d3d11.h>
#include <xnamath.h>
#include "window.h"
#include "enginebase.h"
#include "singleton.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

class Graphics : public Singleton<Graphics>
{
public:
	Graphics();
	virtual ~Graphics();

	bool Initialize(Window* pcWindow, float fpScreenDepth, float fpScreenNear, bool bVSync);
	void Release();

	void BeginScene(float fpRed, float fpGreen, float fpBlue, float fpAlpha);
	void EndScene();

	void GetVideoCardInfo(char* szCardName, int& nVideoCardMemory);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	XMMATRIX GetProjectionMatrix();
	XMMATRIX GetWorldMatrix();
	XMMATRIX GetOrthoMatrix();

private:
	IDXGISwapChain* _pstSwapChain;
	ID3D11Device* _pstDevice;
	ID3D11DeviceContext* _pstDeviceContext;
	ID3D11RenderTargetView* _pstRenderTargetView;
	ID3D11Texture2D* _pstDepthStencilBuffer;
	ID3D11DepthStencilState* _pstDepthStencilState;
	ID3D11DepthStencilView* _pstDepthStencilView;
	ID3D11RasterizerState* _pstRasterState;
	
	XMFLOAT4X4 _stProjectionMatrix;
	XMFLOAT4X4 _stWorldMatrix;
	XMFLOAT4X4 _stOrthoMatrix;

	bool _bVSync;
	int _nVideoCardMemory;
	char _szVideoCardName[128];
};