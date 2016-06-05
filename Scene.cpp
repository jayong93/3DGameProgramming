#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
}


CScene::~CScene()
{
}

bool CScene::OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnKeyEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObject(ID3D11Device * device)
{
}

void CScene::ReleaseObject()
{
}

bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObject(float elapsedTime)
{
}

void CScene::Render(ID3D11DeviceContext * deviceContext)
{
}
