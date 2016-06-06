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
	if (shader) shader->Render(deviceContext);
	if (mesh) mesh->Render(deviceContext);
}

CTriangleObject::CTriangleObject()
{
}

CTriangleObject::~CTriangleObject()
{
}

void CTriangleObject::Animate(float deltaTime)
{
	CGameObject::Animate(deltaTime);
}

void CTriangleObject::Render(ID3D11DeviceContext * deviceContext)
{
	CGameObject::Render(deviceContext);
}
