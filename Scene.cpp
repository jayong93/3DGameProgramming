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

void CScene::BuildObject(ID3D11Device * device, CPlayer* player)
{
	CShader* shader{ new CShader };
	shader->CreateShader(device);
	shaderList.emplace_back(shader);

	XMVECTOR cubeColor = XMVectorSet( 1.f,0.4f,0.4f,1.f );
	CCubeMesh* mesh{ new CCubeMesh{device, D3D11_FILL_SOLID, cubeColor, 0.5f, 0.5f, 0.5f} };
	for (int i = 0; i < 80; i++)
	{
		ChasingObject* obj{ new ChasingObject{player} };
		obj->SetMesh(mesh);
		obj->SetPosition({ RandomRangeFloat(-49.f,49.f),1.25f,RandomRangeFloat(-49.f,49.f) });
		shader->objList.emplace_back(obj);
		objectList.emplace_back(obj);
		obj->AddRef();
	}

	XMVECTOR floorColor = XMVectorSet( 0.5f,0.5f,0.5f,1.f );
	mesh = new CCubeMesh{ device, D3D11_FILL_SOLID, floorColor, 100.f,2.f,100.f };
	CGameObject* floor{ new CGameObject };
	floor->SetMesh(mesh);
	shader->objList.emplace_back(floor);

	objectList.emplace_back(floor);
	floor->AddRef();
}

void CScene::ReleaseObject()
{
	for (auto& s : shaderList)
	{
		s->ReleaseObjects();
		s->Release();
	}
	for (auto& o : objectList)
		o->Release();
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
