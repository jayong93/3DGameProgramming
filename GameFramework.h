#pragma once
#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE ghInst, HWND hMainWnd);
	void OnDestroy();

	bool CreateRenderTargetView();
	bool CreateDirect3DDisplay();

	void BuildObject();
	void ReleaseObject();

	void ProcessInput();
	void AnimateObject();
	void FrameAdvance();

	void OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	void OnKeyEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnWndMessage(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE hInst;
	HWND hWnd;

	int clientWidth, clientHeight;

	ID3D11Device* d3dDevice;
	IDXGISwapChain* dxgiSwapChain;
	ID3D11DeviceContext* d3dDeviceContext;
	ID3D11RenderTargetView* d3dRenderTargetView;
	ID3D11Texture2D* depthStencilBuffer{ nullptr };
	ID3D11DepthStencilView* depthStencilView{ nullptr };

	CTimer timer;
	TCHAR captionBuffer[50];
	CScene* scene;

	CShader* playerShader{ nullptr };
	CPlayer* player;
	ID3D11Buffer* cbColor;

	POINT oldCursorPos;
};

