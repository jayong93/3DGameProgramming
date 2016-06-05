#pragma once
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
};

