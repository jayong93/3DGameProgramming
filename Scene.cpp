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

	// 외곽 큐브
	CWireCubeMesh* wMesh{ new CWireCubeMesh{device, 45.0f, 45.0f, 45.0f} };
	CGameObject* obj{ new CGameObject };
	obj->SetMesh(wMesh);
	obj->SetShader(shader);

	objectList.emplace_back(obj);

	// 내부 큐브들
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

	// 충돌 처리
	CGameObject* outerCube{ objectList.front() };
	D3DXPLANE plane[6];
	CMesh* mesh = outerCube->mesh;
	// 외곽 큐브 평면 구하기
	for (int i = 0, j = 0; j < 6 && i < mesh->GetVertexCount(); i += 6, ++j)
	{
		D3DXPlaneFromPoints(plane + j, &mesh->GetVertexData(i).GetPosition(), &mesh->GetVertexData(i + 1).GetPosition(), &mesh->GetVertexData(i + 2).GetPosition());
		D3DXPlaneNormalize(plane + j, plane + j);
	}
	// 충돌체크 및 반사
	for (auto it = objectList.begin() + 1; it != objectList.end(); ++it)
	{
		bool needReflection{ false };
		D3DXVECTOR3 planeNormal;
		auto pos = (*it)->GetPos();
		const CMesh& m = *(*it)->mesh;
		for (int i = 0; i < m.GetVertexCount() && !needReflection; ++i)
		{
			for (int j = 0; j < 6; ++j)
			{
				if (D3DXPlaneDotCoord(plane + j, &(m.GetVertexData(i).GetPosition() + pos)) > 0)
				{
					needReflection = true;
					memcpy_s(&planeNormal, sizeof(float) * 3, plane + j, sizeof(float) * 3);
					break;
				}
			}
		}

		if (needReflection)
		{
			CRotatingObject& rObj = *(CRotatingObject*)(*it);
			auto vel = rObj.GetVelocity();
			vel -= (vel - planeNormal) * 2;
			vel *= -1.0f;
			rObj.SetVelocity(vel.x, vel.y, vel.z);
		}
	}
}

void CScene::Render(ID3D11DeviceContext * deviceContext, CCamera* camera)
{
	for (auto& o : objectList)
		o->Render(deviceContext);
}
