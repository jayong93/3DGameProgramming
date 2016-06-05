#include "stdafx.h"
#include "GameFramework.h"


CGameFramework::CGameFramework() : d3dDevice{ nullptr }, d3dDeviceContext{ nullptr }, dxgiSwapChain{ nullptr }, d3dRenderTargetView{ nullptr }
{
	_tcscpy_s(captionBuffer, TEXT("DirectX Project "));
}


CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE ghInst, HWND hMainWnd)
{
	hInst = ghInst;
	hWnd = hMainWnd;

	if (!CreateDirect3DDisplay()) return false;

	BuildObject();

	return true;
}

void CGameFramework::OnDestroy()
{
	ReleaseObject();

	if (d3dDeviceContext) d3dDeviceContext->ClearState();
	if (d3dRenderTargetView) d3dRenderTargetView->Release();
	if (dxgiSwapChain) dxgiSwapChain->Release();
	if (d3dDeviceContext) d3dDeviceContext->Release();
	if (d3dDevice) d3dDevice->Release();
}

bool CGameFramework::CreateRenderTargetView()
{
	HRESULT hResult{ S_OK };

	ID3D11Texture2D *d3dBackBuffer;
	if (FAILED(hResult = dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&d3dBackBuffer))) return false;
	if (FAILED(hResult = d3dDevice->CreateRenderTargetView(d3dBackBuffer, nullptr, &d3dRenderTargetView))) return false;
	if (d3dBackBuffer) d3dBackBuffer->Release();

	d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTargetView, nullptr);

	return(true);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	clientWidth = rcClient.right - rcClient.left;
	clientHeight = rcClient.bottom - rcClient.top;

	UINT createDeviceFlags{ 0 };

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = clientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = clientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = true;

	D3D_FEATURE_LEVEL nd3dFeatureLevel;

	HRESULT hResult = S_OK;

	hResult = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &dxgiSwapChain, &d3dDevice, &nd3dFeatureLevel, &d3dDeviceContext);
	if (!dxgiSwapChain || !d3dDevice || !d3dDeviceContext) return false;
	if (nd3dFeatureLevel != D3D_FEATURE_LEVEL_11_0) return false;
	if (!CreateRenderTargetView()) return false;

	return true;
}

void CGameFramework::BuildObject()
{
}

void CGameFramework::ReleaseObject()
{
}

void CGameFramework::ProcessInput()
{
}

void CGameFramework::AnimateObject()
{
}

void CGameFramework::FrameAdvance()
{
	timer.Tick();
	ProcessInput();
	AnimateObject();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	d3dDeviceContext->ClearRenderTargetView(d3dRenderTargetView, fClearColor);
	dxgiSwapChain->Present(1, 0);

	timer.GetFrameRate(captionBuffer + 16, 32);
	SetWindowText(hWnd, captionBuffer);
}

void CGameFramework::OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnKeyEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CGameFramework::OnWndMessage(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_SIZE:
	{
		clientWidth = LOWORD(lParam);
		clientHeight = HIWORD(lParam);

		d3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

		if (d3dRenderTargetView) d3dRenderTargetView->Release();

		dxgiSwapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetView();

		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnMouseEvent(hWnd, iMessage, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnKeyEvent(hWnd, iMessage, wParam, lParam);
		break;
	}

	return 0;
}
