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
	CShader* shader{ new CShader };
	shader->CreateShader(device);

	CCubeMesh* mesh{ new CCubeMesh{device, 15.0f, 15.0f, 15.0f} };
	CRotatingObject* obj{ new CRotatingObject };
	obj->SetMesh(mesh);
	obj->SetShader(shader);

	objectList.emplace_back(obj);
}

void CScene::ReleaseObject()
{
	for (auto& o : objectList)
	{
		o->Release();
	}
}

bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObject(float elapsedTime)
{
	for (auto& o : objectList)
	{
		o->Animate(elapsedTime);
	}
}

void CScene::Render(ID3D11DeviceContext * deviceContext, CCamera* camera)
{
	for (auto& o : objectList)
		o->Render(deviceContext);
}
