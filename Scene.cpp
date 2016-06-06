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

	CWireCubeMesh* wMesh{ new CWireCubeMesh{device, 45.0f, 45.0f, 45.0f} };
	CGameObject* obj{ new CGameObject };
	obj->SetMesh(wMesh);
	obj->SetShader(shader);

	objectList.emplace_back(obj);

	CCubeMesh* cMesh{ new CCubeMesh{device} };
	CRotatingObject* rObj;
	for (int i = 0; i < 50; i++)
	{
		rObj = new CRotatingObject{ RandomRangeFloat(0.0f,90.0f) };
		rObj->SetPos(D3DXVECTOR3{ RandomRangeFloat(-20,20),RandomRangeFloat(-20,20),RandomRangeFloat(-20,20) });
		D3DXVECTOR3 vel{ RandomRangeFloat(-1.0f,1.0f), RandomRangeFloat(-1.0f,1.0f), RandomRangeFloat(-1.0f,1.0f) };
		D3DXVec3Normalize(&vel, &vel);
		vel *= 5.0f;
		rObj->SetVelocity(vel.x, vel.y, vel.z);
		rObj->SetMesh(cMesh);
		rObj->SetShader(shader);
		objectList.emplace_back(rObj);
	}
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
