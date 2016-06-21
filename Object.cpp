#include "stdafx.h"
#include "Shader.h"
#include "Object.h"


CGameObject::CGameObject() : mesh{ nullptr }, reference{ 1 }
{
	XMStoreFloat4x4A(&mtxWorld, XMMatrixIdentity());
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
	CShader::UpdateShaderVariable(deviceContext, XMLoadFloat4x4A(&mtxWorld));
	if (mesh) mesh->Render(deviceContext);
}

void CGameObject::Move(float x, float y, float z)
{
	this->Move(XMVectorSet(x, y, z, 0));
}

void CGameObject::Move(FXMVECTOR shift)
{
	auto pos = this->GetPosition();
	pos += shift;
	this->SetPosition(pos);
}

void CGameObject::Move(DWORD dir, float speed)
{
	if (dir)
	{
		XMFLOAT3A shift{ 0,0,0 };

		if (dir & FORWARD) shift.z += speed;
		if (dir & BACKWARD) shift.z -= speed;

		if (dir & RIGHT) shift.x += speed;
		if (dir & LEFT) shift.x -= speed;

		if (dir & UP) shift.y += speed;
		if (dir & DOWN) shift.y -= speed;

		Move(XMLoadFloat3A(&shift));
	}
}

bool CGameObject::GetAABB(BoundingBox & aabb) const
{
	if (mesh)
	{
		auto modelAABB = mesh->GetAABB();
		modelAABB.Transform(aabb, XMLoadFloat4x4A(&mtxWorld));
		return true;
	}
	return false;
}

bool CGameObject::GetOOBB(BoundingOrientedBox & oobb) const
{
	if (mesh)
	{
		BoundingOrientedBox modelOOBB;
		BoundingOrientedBox::CreateFromBoundingBox(modelOOBB, mesh->GetAABB());
		modelOOBB.Transform(oobb, XMLoadFloat4x4A(&mtxWorld));
		return true;
	}
	return false;
}

bool CGameObject::GetBoundingSphere(BoundingSphere & sphere) const
{
	if (mesh)
	{
		BoundingSphere modelSphere;
		BoundingSphere::CreateFromBoundingBox(modelSphere, mesh->GetAABB());
		modelSphere.Transform(sphere, XMLoadFloat4x4A(&mtxWorld));
		return true;
	}
	return false;
}

CRotatingObject::CRotatingObject()
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float deltaTime)
{
	XMMATRIX rMatrix;
	rMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f*deltaTime));
	XMStoreFloat4x4A(&mtxWorld, rMatrix * XMLoadFloat4x4A(&mtxWorld));
}

void CRotatingObject::Render(ID3D11DeviceContext * deviceContext)
{
	CGameObject::Render(deviceContext);
}

void ChasingObject::Animate(float deltaTime)
{
	if (target)
	{
		XMVECTOR targetPos = target->GetPosition();
		XMVECTOR myPos = this->GetPosition();
		XMVECTOR targetDir = targetPos - myPos;
		targetDir = XMVector3Normalize(targetDir);
		XMVECTOR forward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMVECTOR axis = XMVector3Cross(targetDir, forward);

		float dotResult = XMVectorGetX(XMVector3Dot(targetDir, forward));
		dotResult = MinMax(0.f, dotResult, 1.f);
		float angle = XMConvertToDegrees(XMScalarACos(dotResult));

		if (angle >= 0.01)
		{
			if (angle > angleSpeed*deltaTime)
				angle = angleSpeed*deltaTime;
			if (XMVectorGetY(axis) < 0)
				angle = -angle;
			angle = XMConvertToRadians(angle);

			XMMATRIX rm = XMMatrixRotationY(angle);
			this->SetWorldMatrix(rm * this->GetWorldMatrix());
		}

		XMVECTOR look = XMVector3TransformCoord(forward, this->GetWorldMatrix());
		look *= speed*deltaTime;
		this->Move(look);
	}
}
