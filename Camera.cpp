#include "stdafx.h"
#include "Player.h"
#include "Camera.h"


CCamera::CCamera(CCamera* cam) : player(nullptr), cbCamera{ nullptr }
{
	if (cam)
	{
		position = cam->GetPosition();
		right = cam->GetRightVector();
		look = cam->GetLookVector();
		up = cam->GetUpVector();
		pitch = cam->GetPitch();
		yaw = cam->GetYaw();
		roll = cam->GetRoll();
		mtxProjection = cam->GetProjectionMatrix();
		mtxView = cam->GetViewMatrix();
		viewport = cam->GetViewport();
		offset = cam->GetOffset();
		timeLag = cam->GetTimeLag();
		player = cam->GetPlayer();
		cbCamera = cam->GetCameraConstantBuffer();
		if (cbCamera) cbCamera->AddRef();
	}
	else
	{
		position = D3DXVECTOR3{ 0.f,0.f,0.f };
		right = D3DXVECTOR3{ 1.f,0.f,0.f };
		up = D3DXVECTOR3{ 0.f,1.f,0.f };
		look = D3DXVECTOR3{ 0.f,0.f,1.f };
		pitch = 0.f;
		yaw = 0.f;
		roll = 0.f;
		offset = D3DXVECTOR3{ 0.f,0.f,0.f };
		timeLag = 0.f;
		player = nullptr;
		cbCamera = nullptr;
		D3DXMatrixIdentity(&mtxView);
		D3DXMatrixIdentity(&mtxProjection);
		mode = CameraMode::NONE;
	}
}


CCamera::~CCamera()
{
	if (cbCamera) cbCamera->Release();
}

void CCamera::SetViewport(ID3D11DeviceContext * deviceContext, DWORD xStart, DWORD yStart, DWORD width, DWORD height, float minZ, float maxZ)
{
	viewport.TopLeftX = (float)xStart;
	viewport.TopLeftY = (float)yStart;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = minZ;
	viewport.MaxDepth = maxZ;

	deviceContext->RSSetViewports(1, &viewport);
}

void CCamera::CreateViewMatrix()
{
	D3DXMatrixLookAtLH(&mtxView, &position, &look, &up);
}

void CCamera::RecreateViewMatrix()
{
	D3DXVec3Normalize(&look, &look);
	D3DXVec3Cross(&right, &up, &look);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Cross(&up, &look, &right);
	D3DXVec3Normalize(&up, &up);

	mtxView._11 = right.x;
	mtxView._12 = up.x;
	mtxView._13 = look.x;
	mtxView._21 = right.y;
	mtxView._22 = up.y;
	mtxView._23 = look.y;
	mtxView._31 = right.z;
	mtxView._32 = up.z;
	mtxView._33 = look.z;
	mtxView._41 = D3DXVec3Dot(&position, &right);
	mtxView._42 = D3DXVec3Dot(&position, &up);
	mtxView._43 = D3DXVec3Dot(&position, &look);
}

void CCamera::CreateProjectionMatrix(float nearDist, float farDist, float aspect, float fov)
{
	D3DXMatrixPerspectiveFovLH(&mtxProjection, (float)D3DXToRadian(fov), aspect, nearDist, farDist);
}

void CCamera::CreateShaderVariable(ID3D11Device * device)
{
	D3D11_BUFFER_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.ByteWidth = sizeof(VS_CB_CAMERA);
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device->CreateBuffer(&bDesc, nullptr, &cbCamera);
}

void CCamera::UpdateShaderVariable(ID3D11DeviceContext * deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mapRes;

	deviceContext->Map(cbCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes);
	VS_CB_CAMERA* viewProjection = (VS_CB_CAMERA*)mapRes.pData;
	D3DXMatrixTranspose(&viewProjection->mtxView, &mtxView);
	D3DXMatrixTranspose(&viewProjection->mtxProjection, &mtxProjection);
	deviceContext->Unmap(cbCamera, 0);
	deviceContext->VSSetConstantBuffers(VS_SLOT_CAMERA, 1, &cbCamera);
}

CSpaceShipCamera::CSpaceShipCamera(CCamera * cam) : CCamera(cam)
{
	mode = CameraMode::SPACESHIP;
}

void CSpaceShipCamera::Rotate(float pitch, float yaw, float roll)
{
	D3DXMATRIX rm;
	if (player&&pitch != 0.f)
	{
		D3DXMatrixRotationAxis(&rm, &player->GetRightVector(), (float)D3DXToRadian(pitch));
		D3DXVec3TransformNormal(&right, &right, &rm);
		D3DXVec3TransformNormal(&up, &up, &rm);
		D3DXVec3TransformNormal(&look, &look, &rm);

		position -= player->GetPosition();

		D3DXVec3TransformCoord(&position, &position, &rm);
		position += player->GetPosition();

	}
	if (player && (yaw != 0.f))
	{
		D3DXMatrixRotationAxis(&rm, &player->GetUpVector(), (float)D3DXToRadian(yaw));
		D3DXVec3TransformNormal(&right, &right, &rm);
		D3DXVec3TransformNormal(&up, &up, &rm);
		D3DXVec3TransformNormal(&look, &look, &rm);

		position -= player->GetPosition();

		D3DXVec3TransformCoord(&position, &position, &rm);
		position += player->GetPosition();
	}
	if (player && (roll != 0.f))
	{
		D3DXMatrixRotationAxis(&rm, &player->GetLookVector(), (float)D3DXToRadian(roll));
		D3DXVec3TransformNormal(&right, &right, &rm);
		D3DXVec3TransformNormal(&up, &up, &rm);
		D3DXVec3TransformNormal(&look, &look, &rm);

		position -= player->GetPosition();

		D3DXVec3TransformCoord(&position, &position, &rm);
		position += player->GetPosition();
	}
}

CFirstPersonCamera::CFirstPersonCamera(CCamera * cam) : CCamera{ cam }
{
	mode = CameraMode::FIRST_PERSON;
	if (cam)
	{
		if (cam->GetMode() == CameraMode::SPACESHIP)
		{
			up = D3DXVECTOR3(0.f, 1.f, 0.f);
			right.y = 0.f;
			look.y = 0.f;
			D3DXVec3Normalize(&right, &right);
			D3DXVec3Normalize(&look, &look);
		}
	}
}

void CFirstPersonCamera::Rotate(float pitch, float yaw, float roll)
{
	D3DXMATRIX rm;
	if (pitch != 0.f)
	{
		D3DXMatrixRotationAxis(&rm, &player->GetRightVector(), (float)D3DXToRadian(pitch));
		D3DXVec3TransformNormal(&right, &right, &rm);
		D3DXVec3TransformNormal(&up, &up, &rm);
		D3DXVec3TransformNormal(&look, &look, &rm);
	}
	if (yaw != 0.f)
	{
		D3DXMatrixRotationAxis(&rm, &player->GetUpVector(), (float)D3DXToRadian(yaw));
		D3DXVec3TransformNormal(&right, &right, &rm);
		D3DXVec3TransformNormal(&up, &up, &rm);
		D3DXVec3TransformNormal(&look, &look, &rm);
	}
	if (roll != 0.f)
	{
		D3DXMatrixRotationAxis(&rm, &player->GetLookVector(), (float)D3DXToRadian(roll));

		position -= player->GetPosition();
		D3DXVec3TransformCoord(&position, &position, &rm);

		position += player->GetPosition();

		D3DXVec3TransformNormal(&right, &right, &rm);
		D3DXVec3TransformNormal(&up, &up, &rm);
		D3DXVec3TransformNormal(&look, &look, &rm);
	}
}

CThirdPersonCamera::CThirdPersonCamera(CCamera * cam) : CCamera{ cam }
{
	mode = CameraMode::THIRD_PERSON;
	if (cam)
	{
		if (cam->GetMode() == CameraMode::SPACESHIP)
		{
			up = D3DXVECTOR3{ 0.f,1.f,0.f };
			right.y = 0.f;
			look.y = 0.f;
			D3DXVec3Normalize(&right, &right);
			D3DXVec3Normalize(&look, &look);
		}
	}
}

void CThirdPersonCamera::Update(D3DXVECTOR3 & lookAt, float timeScale)
{
	if (player)
	{
		D3DXMATRIX rm;
		D3DXMatrixIdentity(&rm);
		D3DXVECTOR3 vr = player->GetRightVector();
		D3DXVECTOR3 vu = player->GetUpVector();
		D3DXVECTOR3 vl = player->GetLookVector();

		rm._11 = vr.x; rm._21 = vu.x; rm._31 = vl.x;
		rm._12 = vr.y; rm._22 = vu.y; rm._32 = vl.y;
		rm._13 = vr.z; rm._23 = vu.z; rm._33 = vl.z;

		D3DXVECTOR3 os;
		D3DXVec3TransformCoord(&os, &offset, &rm);
		D3DXVECTOR3 pos = player->GetPosition() + os;
		D3DXVECTOR3 direction = pos - position;
		float length = D3DXVec3Length(&direction);
		direction /= length;

		float timeLagScale = timeLag ? timeScale * (1.f / timeLag) : 1.f;
		float distance = length * timeLagScale;
		if (distance > length || length < 0.01f) distance = length;
		if (distance > 0)
		{
			position += direction * distance;
			SetLookAt(lookAt);
		}
	}
}

void CThirdPersonCamera::SetLookAt(D3DXVECTOR3 & lookAt)
{
	D3DXMATRIX mla;
	D3DXMatrixLookAtLH(&mla, &position, &lookAt, &player->GetUpVector());
	right = D3DXVECTOR3{ mla._11,mla._21,mla._31 };
	up = D3DXVECTOR3{ mla._12,mla._22,mla._32 };
	look = D3DXVECTOR3{ mla._13,mla._23,mla._33 };
}
