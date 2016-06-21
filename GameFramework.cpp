#include "stdafx.h"
#include "GameFramework.h"


CGameFramework::CGameFramework() : d3dDevice{ nullptr }, d3dDeviceContext{ nullptr }, dxgiSwapChain{ nullptr }, d3dRenderTargetView{ nullptr }, player{ nullptr }
{
	srand(time(nullptr));
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

	D3D11_TEXTURE2D_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Width = clientWidth;
	bd.Height = clientHeight;
	bd.MipLevels = 1;
	bd.ArraySize = 1;
	bd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bd.SampleDesc.Count = 1;
	bd.SampleDesc.Quality = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(hResult = d3dDevice->CreateTexture2D(&bd, nullptr, &depthStencilBuffer))) return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC vd;
	ZeroMemory(&vd, sizeof(vd));
	vd.Format = bd.Format;
	vd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	vd.Texture2D.MipSlice = 0;
	if (FAILED(hResult = d3dDevice->CreateDepthStencilView(depthStencilBuffer, &vd, &depthStencilView))) return false;

	d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTargetView, depthStencilView);

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
	scene = new CScene;

	player = new CPlayer;

	CShader::CreateShaderVariables(d3dDevice);

	playerShader = new CShader;
	XMVECTOR cubeColor = XMVectorSet(0.4f, 0.4f, 1.f, 1.f);
	CMesh* mesh = new CCubeMesh{ d3dDevice,D3D11_FILL_SOLID, cubeColor, 0.5f,0.5f,0.5f };
	playerShader->CreateShader(d3dDevice);
	playerShader->objList.emplace_back(player);
	player->SetMesh(mesh);
	player->SetPosition({ 0.f, 1.25f, 0.f });

	CCamera* cam = new ThirdCam;
	cam->CreateShaderVariable(d3dDevice);
	cam->SetViewport(d3dDeviceContext, 0, 0, clientWidth, clientHeight);

	cam->CreateProjectionMatrix(1.0, 500.0f, clientWidth / (float)clientHeight, 90.0f);
	cam->SetPlayer(player);
	cam->CreateViewMatrix();

	player->SetCamera(cam);
	player->CreateShaderVariables(d3dDevice);

	if (scene) scene->BuildObject(d3dDevice, player);

	XMFLOAT4A color{ 1.0f,0.0f,0.0f,1.0f };
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(D3DXCOLOR);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &color;
	d3dDevice->CreateBuffer(&bd, &data, &cbColor);
	d3dDeviceContext->PSSetConstantBuffers(PS_SLOT_COLOR, 1, &cbColor);
}

void CGameFramework::ReleaseObject()
{
	if (scene)
	{
		scene->ReleaseObject();
		delete scene;
	}

	if (player) delete player;
	if (cbColor) cbColor->Release();
}

void CGameFramework::ProcessInput()
{
	bool processedByScene{ false };
	if (scene) processedByScene = scene->ProcessInput();
	if (!processedByScene)
	{
		static UCHAR keyBuffer[256];
		DWORD direction{ 0 };
		if (GetKeyboardState(keyBuffer))
		{
			if (keyBuffer['W'] & 0xf0) direction |= FORWARD;
			if (keyBuffer['S'] & 0xf0) direction |= BACKWARD;
			if (keyBuffer['A'] & 0xf0) direction |= LEFT;
			if (keyBuffer['D'] & 0xf0) direction |= RIGHT;
		}

		float cx = 0.f, cy = 0.f;
		POINT cursorPos;

		if (GetCapture() == hWnd)
		{
			SetCursor(nullptr);

			GetCursorPos(&cursorPos);

			cx = (float)(cursorPos.x - oldCursorPos.x) / 3.f;
			cy = (float)(cursorPos.y - oldCursorPos.y) / 3.f;
			SetCursorPos(oldCursorPos.x, oldCursorPos.y);
		}

		if (direction != 0 || cx != 0.f || cy != 0.f)
		{
			if (cx || cy)
			{
				if (keyBuffer[VK_RBUTTON] & 0xf0)
					player->GetCamera()->Rotate(cy, cx, 0.f);
			}

			if (direction) player->Move(direction, 5.f*timer.GetTimeElapsed());
		}
	}

	player->Update(timer.GetTimeElapsed());
}

void CGameFramework::AnimateObject()
{
	scene->AnimateObject(timer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	timer.Tick(0);
	ProcessInput();
	AnimateObject();

	float fClearColor[4] = { 1.f, 1.f, 1.0f, 1.0f };
	if (d3dRenderTargetView)
		d3dDeviceContext->ClearRenderTargetView(d3dRenderTargetView, fClearColor);
	if (depthStencilView)
		d3dDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	if (player) player->UpdateShaderVariables(d3dDeviceContext);
	CCamera* cam = player ? player->GetCamera() : nullptr;
	if (scene)scene->Render(d3dDeviceContext, cam);
	if (playerShader) playerShader->Render(d3dDeviceContext);

	dxgiSwapChain->Present(1, 0);

	int textLen = lstrlen(TEXT("DirectX Project "));
	timer.GetFrameRate(captionBuffer + textLen, 49 - textLen);
	SetWindowText(hWnd, captionBuffer);
}

void CGameFramework::OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		GetCursorPos(&oldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
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
		case VK_F1:
		case VK_F2:
		case VK_F3:
		{

			break;
		}
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
		if (depthStencilBuffer) depthStencilBuffer->Release();
		if (depthStencilView) depthStencilView->Release();

		dxgiSwapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetView();
		CCamera* cam = player->GetCamera();
		if (cam) cam->SetViewport(d3dDeviceContext, 0, 0, clientWidth, clientHeight);
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
