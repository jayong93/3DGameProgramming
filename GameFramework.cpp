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
	scene = new CScene;

	CShader* shader{ new CShader };
	shader->CreateShader(d3dDevice);

	player = new CCubePlayer{ d3dDevice };
	CMesh* cubeMesh = new CCubeMesh{ d3dDevice,D3D11_FILL_WIREFRAME,{1.f,0.f,0.f,1.f} };
	player->SetMesh(cubeMesh);
	player->SetShader(shader);

	CCamera* cam{ nullptr };

	player->CreateShaderVariables(d3dDevice);
	player->ChangeCamera(d3dDevice, CameraMode::SPACESHIP, timer.GetTimeElapsed());

	cam = player->GetCamera();
	cam->SetViewport(d3dDeviceContext, 0, 0, clientWidth, clientHeight);
	cam->CreateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	cam->CreateViewMatrix();

	player->SetPosition(D3DXVECTOR3{ 0.f,0.f,100.f });

	if (scene) scene->BuildObject(d3dDevice);

	D3DXCOLOR color{ 1.0f,0.0f,0.0f,1.0f };
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
		DWORD direction = 0;
		if (GetKeyboardState(keyBuffer))
		{
			if (keyBuffer[VK_UP] & 0xf0) direction |= FORWARD;
			if (keyBuffer[VK_DOWN] & 0xf0) direction |= BACKWARD;
			if (keyBuffer[VK_LEFT] & 0xf0) direction |= LEFT;
			if (keyBuffer[VK_RIGHT] & 0xf0) direction |= RIGHT;
			if (keyBuffer[VK_PRIOR] & 0xf0) direction |= UP;
			if (keyBuffer[VK_NEXT] & 0xf0) direction |= DOWN;
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
					player->Rotate(cy, cx, 0.f);
				//else
				//	player->Rotate(cy, cx, 0.f);
			}

			if (direction) player->Move(direction, 50.f*timer.GetTimeElapsed(), true);
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

	float fClearColor[4] = { 0.75f, 0.75f, 1.0f, 1.0f };
	d3dDeviceContext->ClearRenderTargetView(d3dRenderTargetView, fClearColor);

	if (player) player->UpdateShaderVariables(d3dDeviceContext);
	CCamera* cam = player ? player->GetCamera() : nullptr;
	if (scene)scene->Render(d3dDeviceContext, cam);

	if (player) player->Render(d3dDeviceContext);

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
			if (player) player->ChangeCamera(d3dDevice, (CameraMode)(wParam - VK_F1 + 1), timer.GetTimeElapsed());
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
