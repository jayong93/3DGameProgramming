#include "stdafx.h"
#include "Object.h"


CGameObject::CGameObject() : mesh{ nullptr }, shader{ nullptr }, reference{ 0 }
{
	D3DXMatrixIdentity(&mtxWorld);
}


CGameObject::~CGameObject()
{
	if (mesh) mesh->Release();
	if (shader) shader->Release();
}

void CGameObject::SetMesh(CMesh * m)
{
	if (mesh) mesh->Release();
	mesh = m;
	if (mesh) mesh->AddRef();
}

void CGameObject::SetShader(CShader* s)
{
	if (shader) shader->Release();
	shader = s;
	if (shader) shader->AddRef();
}

void CGameObject::Animate(float deltaTime)
{
}

void CGameObject::Render(ID3D11DeviceContext * deviceContext)
{
	if (shader)
	{
		shader->UpdateShaderVariable(deviceContext, &mtxWorld);
		shader->Render(deviceContext);
	}
	if (mesh) mesh->Render(deviceContext);
}

bool CGameObject::CheckRayCast(D3DXVECTOR3 const& rayStart, D3DXVECTOR3 const& rayDir, float* dist) const
{
	if (mesh)
	{
		float minDist{ -1 }, curDist{ -1 };

		for (int i = 0; i < mesh->GetVertexCount(); i += 3)
		{
			D3DXVECTOR3 vertex[]{ mesh->GetVertexData(i).GetPosition(), mesh->GetVertexData(i + 1).GetPosition() , mesh->GetVertexData(i + 2).GetPosition() };
			for (int j = 0; j < 3; ++j)
				D3DXVec3TransformCoord(vertex + j, vertex + j, &mtxWorld);

			if (D3DXIntersectTri(vertex, vertex + 1, vertex + 2, &rayStart, &rayDir, nullptr, nullptr, &curDist))
			{
				if (minDist < 0 || minDist > curDist) minDist = curDist;
			}
		}

		if (minDist >= 0)
		{
			if (dist)
				*dist = minDist;
			return true;
		}
	}
	return false;
}

CRotatingObject::CRotatingObject(float yAngle) : CGameObject(), yAnglePerSecond{ yAngle }
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float deltaTime)
{
	D3DXMATRIX rMatrix, tMatrix;
	D3DXMatrixRotationY(&rMatrix, (float)D3DXToRadian(yAnglePerSecond*deltaTime));
	mtxWorld = rMatrix * mtxWorld;
	D3DXMatrixTranslation(&tMatrix, velocity.x*deltaTime, velocity.y*deltaTime, velocity.z*deltaTime);
	mtxWorld = mtxWorld * tMatrix;
}

void CRotatingObject::Render(ID3D11DeviceContext * deviceContext)
{
	CGameObject::Render(deviceContext);
}

void CRotatingObject::SetVelocity(float vx, float vy, float vz)
{
	velocity.x = vx; velocity.y = vy; velocity.z = vz;
}
