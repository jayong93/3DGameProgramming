#include "stdafx.h"
#include "Shader.h"
#include "Object.h"


CGameObject::CGameObject() : mesh{ nullptr }, reference{ 1 }
{
	D3DXMatrixIdentity(&mtxWorld);
}


CGameObject::~CGameObject()
{
	if (mesh) mesh->Release();
}

void CGameObject::SetMesh(CMesh * m)
{
	if (mesh) mesh->Release();
	mesh = m;
	if (mesh) mesh->AddRef();
}

void CGameObject::Animate(float deltaTime)
{
}

void CGameObject::Render(ID3D11DeviceContext * deviceContext)
{
	CShader::UpdateShaderVariable(deviceContext, &mtxWorld);
	if (mesh) mesh->Render(deviceContext);
}

CRotatingObject::CRotatingObject()
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float deltaTime)
{
	D3DXMATRIX rMatrix;
	D3DXMatrixRotationY(&rMatrix, (float)D3DXToRadian(45.0f*deltaTime));
	mtxWorld = rMatrix * mtxWorld;
}

void CRotatingObject::Render(ID3D11DeviceContext * deviceContext)
{
	CGameObject::Render(deviceContext);
}
