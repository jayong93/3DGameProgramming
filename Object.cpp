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

void CGameObject::MoveStrafe(float dist)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 right = GetRight();
	pos += dist * right;
	CGameObject::SetPosition(pos);
}

void CGameObject::MoveUp(float dist)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 up = GetUp();
	pos += dist * up;
	CGameObject::SetPosition(pos);
}

void CGameObject::MoveForward(float dist)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 look = GetLookAt();
	pos += dist * look;
	CGameObject::SetPosition(pos);
}

void CGameObject::Rotate(float pitch, float yaw, float roll)
{
	D3DXMATRIX rm;
	D3DXMatrixRotationYawPitchRoll(&rm, (float)D3DXToRadian(yaw), (float)D3DXToRadian(pitch), (float)D3DXToRadian(roll));
	mtxWorld = rm * mtxWorld;
}

void CGameObject::Rotate(D3DXVECTOR3 * axis, float angle)
{
	D3DXMATRIX rm;
	D3DXMatrixRotationAxis(&rm, axis, (float)D3DXToRadian(angle));
	mtxWorld = rm * mtxWorld;
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
	OnPrepareRender();

	if (shader)
	{
		shader->UpdateShaderVariable(deviceContext, &mtxWorld);
		shader->Render(deviceContext);
	}
	if (mesh) mesh->Render(deviceContext);
}

D3DXVECTOR3 CGameObject::GetLookAt()
{
	D3DXVECTOR3 lookAt{ mtxWorld._31, mtxWorld._32, mtxWorld._33 };
	D3DXVec3Normalize(&lookAt, &lookAt);
	return lookAt;
}

D3DXVECTOR3 CGameObject::GetUp()
{
	D3DXVECTOR3 up{ mtxWorld._21, mtxWorld._22,mtxWorld._23 };
	D3DXVec3Normalize(&up, &up);
	return up;
}

D3DXVECTOR3 CGameObject::GetRight()
{
	D3DXVECTOR3 right{ mtxWorld._11, mtxWorld._12, mtxWorld._13 };
	D3DXVec3Normalize(&right, &right);
	return right;
}

D3DXVECTOR3 CGameObject::GetPosition()
{
	return D3DXVECTOR3{ mtxWorld._41, mtxWorld._42, mtxWorld._43 };
}

void CGameObject::SetPosition(const D3DXVECTOR3 & pos)
{
	mtxWorld._41 = pos.x; mtxWorld._42 = pos.y; mtxWorld._43 = pos.z;
}

CRotatingObject::CRotatingObject() : rotationSpeed{ 15.f }
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float deltaTime)
{
	CGameObject::Rotate(&rotationAxis, rotationSpeed*deltaTime);
}

void CRotatingObject::Render(ID3D11DeviceContext * deviceContext)
{
	CGameObject::Render(deviceContext);
}
