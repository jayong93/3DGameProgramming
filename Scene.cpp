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
	shaderList.emplace_back(shader);

	D3DXCOLOR cubeColor{ 1.f,0.7f,0.7f,1.f };
	CCubeMesh* mesh{ new CCubeMesh{device, D3D11_FILL_SOLID, cubeColor, 15.0f, 15.0f, 15.0f} };
	CRotatingObject* obj{ new CRotatingObject };
	obj->SetMesh(mesh);
	shader->objList.emplace_back(obj);

	objectList.emplace_back(obj);
	obj->AddRef();
}

void CScene::ReleaseObject()
{
	for (auto& s : shaderList)
	{
		s->ReleaseObjects();
		s->Release();
	}
}

bool CScene::ProcessInput()
{
	return false;
}

void CScene::AnimateObject(float elapsedTime)
{
	for (auto& s : shaderList)
	{
		s->AnimateObjects(elapsedTime);
	}
}

void CScene::Render(ID3D11DeviceContext * deviceContext, CCamera* camera)
{
	for (auto& s : shaderList)
		s->Render(deviceContext);
}
