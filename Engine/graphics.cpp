#include "graphics.h"

Graphics::Graphics() : Singleton(),
		       _pstSwapChain(0),
		       _pstDevice(0),
		       _pstDeviceContext(0),
		       _pstRenderTargetView(0),
		       _pstDepthStencilBuffer(0),
		       _pstDepthStencilState(0),
	               _pstDepthStencilView(0),
		       _pstRasterState(0)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(Window* pcWindow, float fpScreenDepth, float fpScreenNear, bool bVSync)
{
	HRESULT result;
	IDXGIFactory* pstFactory;
	IDXGIAdapter* pstAdapter;
	IDXGIOutput* pstOutput;

	int nScreenWidth = pcWindow->GetWidth();
	int nScreenHeight = pcWindow->GetHeight();

	unsigned int nModes, nNumerator, nDenominator, nStringLength;

	DXGI_MODE_DESC* rgDisplayModes;
	DXGI_ADAPTER_DESC stAdapterDesc;
	
	DXGI_SWAP_CHAIN_DESC stSwapChainDesc;
	D3D_FEATURE_LEVEL eFeatureLevel;

	ID3D11Texture2D* pstBackBuffer;
	D3D11_TEXTURE2D_DESC stDepthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC stDepthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC stDepthStencilViewDesc;

	D3D11_RASTERIZER_DESC stRasterDesc;
	D3D11_VIEWPORT stViewport;

	float fpFieldOfView, fpScreenAspect;

	_bVSync = bVSync;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pstFactory));
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): CreateDXGIFactory")
		return false;
	}

	result = pstFactory->EnumAdapters(0, &pstAdapter);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): EnumAdapters")
		return false;
	}

	result = pstAdapter->EnumOutputs(0, &pstOutput);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): EnumOutputs")
		return false;
	}

	result = pstOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
										  DXGI_ENUM_MODES_INTERLACED, 
										  &nModes, 
										  NULL);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): GetDisplayModeList")
		return false;
	}

	rgDisplayModes = new DXGI_MODE_DESC[nModes];

	result = pstOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 
										  DXGI_ENUM_MODES_INTERLACED, 
										  &nModes, 
										  rgDisplayModes);

	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): GetDisplayModeList 2")
		return false;
	}

	for(unsigned int i = 0; i < nModes; i++)
	{
		if(rgDisplayModes[i].Width == static_cast<unsigned int>(nScreenWidth))
		{
			if(rgDisplayModes[i].Height == static_cast<unsigned int>(nScreenHeight))
			{
				nNumerator = rgDisplayModes[i].RefreshRate.Numerator;
				nDenominator = rgDisplayModes[i].RefreshRate.Denominator;
			}
		}
	}

	result = pstAdapter->GetDesc(&stAdapterDesc);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): pstAdapter->GetDesc(&stAdapterDesc)")
		return false;
	}

	_nVideoCardMemory = static_cast<int>(stAdapterDesc.DedicatedVideoMemory / 1024 / 1024);
	wcstombs_s(&nStringLength, _szVideoCardName, 128, stAdapterDesc.Description, 128);

	_DELETE(rgDisplayModes);
	_RELEASE(pstOutput)
	_RELEASE(pstFactory)

	memset(&stSwapChainDesc, 0, sizeof(stSwapChainDesc));
	stSwapChainDesc.BufferCount = 1;
	stSwapChainDesc.BufferDesc.Width = nScreenWidth;
	stSwapChainDesc.BufferDesc.Height = nScreenHeight;
	stSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if(_bVSync)
	{
		stSwapChainDesc.BufferDesc.RefreshRate.Numerator = nNumerator;
		stSwapChainDesc.BufferDesc.RefreshRate.Denominator = nDenominator;
	}
	else
	{
		stSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		stSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	stSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	stSwapChainDesc.OutputWindow = pcWindow->GetHandle();

	stSwapChainDesc.SampleDesc.Count = 1;
	stSwapChainDesc.SampleDesc.Quality = 0;
	stSwapChainDesc.Windowed = true;
	stSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	stSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	stSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	stSwapChainDesc.Flags = 0;

	eFeatureLevel = D3D_FEATURE_LEVEL_10_0;

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 
											NULL, 0, &eFeatureLevel, 1, 
											D3D11_SDK_VERSION, &stSwapChainDesc, 
											&_pstSwapChain, &_pstDevice, NULL, 
											&_pstDeviceContext);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): CreateDeviceAndSwapChain")
		return false;
	}

	result = _pstSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pstBackBuffer));
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): _pstSwapChain->GetBuffer(...)")
		return false;
	}

	result = _pstDevice->CreateRenderTargetView(pstBackBuffer, NULL, &_pstRenderTargetView);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): _pstDevice->CreateRenderTargetView(...)")
		return false;
	}

	_RELEASE(pstBackBuffer)

	memset(&stDepthBufferDesc, 0, sizeof(stDepthBufferDesc));
	stDepthBufferDesc.Width = nScreenWidth;
	stDepthBufferDesc.Height = nScreenHeight;
	stDepthBufferDesc.MipLevels = 1;
	stDepthBufferDesc.ArraySize = 1;
	stDepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stDepthBufferDesc.SampleDesc.Count = 1;
	stDepthBufferDesc.SampleDesc.Quality = 0;
	stDepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	stDepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	stDepthBufferDesc.CPUAccessFlags = 0;
	stDepthBufferDesc.MiscFlags = 0;

	result = _pstDevice->CreateTexture2D(&stDepthBufferDesc, NULL, &_pstDepthStencilBuffer);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): _pstDevice->CreateTexture2D(...)")
		return false;
	}

	memset(&stDepthStencilDesc, 0, sizeof(stDepthStencilDesc));
	stDepthStencilDesc.DepthEnable = true;
	stDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	stDepthStencilDesc.StencilEnable = true;
	stDepthStencilDesc.StencilReadMask = 0xFF;
	stDepthStencilDesc.StencilWriteMask = 0xFF;

	stDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	stDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	stDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	stDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = _pstDevice->CreateDepthStencilState(&stDepthStencilDesc, &_pstDepthStencilState);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): _pstDevice->CreateDepthStencilState(...)")
		return false;
	}

	_pstDeviceContext->OMSetDepthStencilState(_pstDepthStencilState, 1);

	memset(&stDepthStencilViewDesc, 0, sizeof(stDepthStencilViewDesc));

	stDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	stDepthStencilViewDesc.Texture2D.MipSlice = 0;

	result = _pstDevice->CreateDepthStencilView(_pstDepthStencilBuffer, &stDepthStencilViewDesc, &_pstDepthStencilView);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): _pstDevice->CreateDepthStencilView(...)")
		return false;
	}

	_pstDeviceContext->OMSetRenderTargets(1, &_pstRenderTargetView, _pstDepthStencilView);

	stRasterDesc.AntialiasedLineEnable = false;
	stRasterDesc.CullMode = D3D11_CULL_BACK;
	stRasterDesc.DepthBias = 0;
	stRasterDesc.DepthBiasClamp = 0.0f;
	stRasterDesc.DepthClipEnable = true;
	stRasterDesc.FillMode = D3D11_FILL_SOLID;
	stRasterDesc.FrontCounterClockwise = false;
	stRasterDesc.MultisampleEnable = false;
	stRasterDesc.ScissorEnable = false;
	stRasterDesc.SlopeScaledDepthBias = 0.0f;

	result = _pstDevice->CreateRasterizerState(&stRasterDesc, &_pstRasterState);
	if(FAILED(result))
	{
		LOG_ERROR("Graphics::Initialize(): _pstDevice->CreateRasterizerState(...)")
		return false;
	}

	_pstDeviceContext->RSSetState(_pstRasterState);

	stViewport.Width = static_cast<float>(nScreenWidth);
	stViewport.Height = static_cast<float>(nScreenHeight);
	stViewport.MinDepth = 0.0f;
	stViewport.MaxDepth = 1.0f;
	stViewport.TopLeftX = 0.0f;
	stViewport.TopLeftY = 0.0f;

	_pstDeviceContext->RSSetViewports(1, &stViewport);
	fpFieldOfView = XM_PI / 4.0f;
	fpScreenAspect = static_cast<float>(nScreenWidth) / static_cast<float>(nScreenHeight);

	XMMATRIX stProjectionMatrix = XMMatrixPerspectiveFovLH(fpFieldOfView, fpScreenAspect, fpScreenNear, fpScreenDepth);
	XMStoreFloat4x4(&_stProjectionMatrix, stProjectionMatrix);

	XMMATRIX stWorldMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&_stWorldMatrix, stWorldMatrix);

	XMMATRIX stOrthoMatrix = XMMatrixOrthographicLH(static_cast<float>(nScreenWidth), static_cast<float>(nScreenHeight), fpScreenNear, fpScreenDepth);
	XMStoreFloat4x4(&_stOrthoMatrix, stOrthoMatrix);

	LOG_INFO("Graphics initialized.")
	return true;
}

void Graphics::Release()
{
	_RELEASE_IF(_pstSwapChain)
	_RELEASE_IF(_pstRasterState)
	_RELEASE_IF(_pstDepthStencilView)
	_RELEASE_IF(_pstDepthStencilState)
	_RELEASE_IF(_pstDepthStencilBuffer)
	_RELEASE_IF(_pstRenderTargetView)
	_RELEASE_IF(_pstDeviceContext)
	_RELEASE_IF(_pstDevice)
	_RELEASE_IF(_pstSwapChain)

	LOG_INFO("Graphics released.")
}

void Graphics::BeginScene(float fpRed, float fpGreen, float fpBlue, float fpAlpha)
{
	float rgfpColor[] = 
	{
		fpRed,
		fpGreen,
		fpBlue,
		fpAlpha
	};

	_pstDeviceContext->ClearRenderTargetView(_pstRenderTargetView, rgfpColor);
	_pstDeviceContext->ClearDepthStencilView(_pstDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::EndScene()
{
	if(_bVSync)
	{
		_pstSwapChain->Present(1, 0);
	}
	else
	{
		_pstSwapChain->Present(0, 0);
	}
}

void Graphics::GetVideoCardInfo(char* szCardName, int& nVideoCardMemory)
{
	strcpy_s(szCardName, 128, _szVideoCardName);
	nVideoCardMemory = _nVideoCardMemory;
}

ID3D11Device* Graphics::GetDevice()
{
	return _pstDevice;
}

ID3D11DeviceContext* Graphics::GetDeviceContext()
{
	return _pstDeviceContext;
}

XMMATRIX Graphics::GetProjectionMatrix()
{
	XMMATRIX stProjectionMatrix = XMLoadFloat4x4(&_stProjectionMatrix);
	return stProjectionMatrix;
}

XMMATRIX Graphics::GetWorldMatrix()
{
	XMMATRIX stWorldMatrix = XMLoadFloat4x4(&_stWorldMatrix);
	return stWorldMatrix;
}

XMMATRIX Graphics::GetOrthoMatrix()
{
	XMMATRIX stOrthoMatrix = XMLoadFloat4x4(&_stOrthoMatrix);
	return stOrthoMatrix;
}