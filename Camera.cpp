#include "stdafx.h"
#include "Player.h"
#include "Camera.h"


CCamera::CCamera() : player(nullptr), cbCamera{ nullptr }
{
	D3DXMatrixIdentity(&mtxView);
	D3DXMatrixIdentity(&mtxProjection);
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
	D3DXMatrixLookAtLH(&mtxView, &position, &lookAt, &up);
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

ThirdCam::ThirdCam()
{
	offset = D3DXVECTOR3{ 0.f,2.f,-5.f };
	lookAt = D3DXVECTOR3{ 0.f,-5.f,10.f };
}

void ThirdCam::SetPlayer(CPlayer * p)
{
	CCamera::SetPlayer(p);

	position = p->GetPosition() + offset;
	lookAt = p->GetPosition();
}

void ThirdCam::CreateViewMatrix()
{
	D3DXMATRIX rm;
	D3DXVECTOR3 rotatedOffset;
	D3DXMatrixRotationX(&rm, D3DXToRadian(xAngle));
	D3DXVec3TransformCoord(&rotatedOffset, &offset, &rm);
	D3DXMatrixRotationY(&rm, D3DXToRadian(yAngle));
	D3DXVec3TransformCoord(&rotatedOffset, &rotatedOffset, &rm);

	position = player->GetPosition() + rotatedOffset;
	lookAt = player->GetPosition();
	D3DXMatrixLookAtLH(&mtxView, &position, &lookAt, &up);
}

void ThirdCam::UpdateViewMatrix()
{
	D3DXMATRIX rm;
	D3DXVECTOR3 rotatedOffset;
	D3DXMatrixRotationX(&rm, D3DXToRadian(xAngle));
	D3DXVec3TransformCoord(&rotatedOffset, &offset, &rm);
	D3DXMatrixRotationY(&rm, D3DXToRadian(yAngle));
	D3DXVec3TransformCoord(&rotatedOffset, &rotatedOffset, &rm);

	position = player->GetPosition() + rotatedOffset;
	lookAt = player->GetPosition();
	D3DXMatrixLookAtLH(&mtxView, &position, &lookAt, &up);
}

void ThirdCam::Rotate(float x, float y, float z)
{
	xAngle += x;
	yAngle += y;
	xAngle = MinMax(-10.f, xAngle, 60.f);
	if (yAngle > 360) yAngle -= 360;
	else if (yAngle < 0) yAngle += 360;

	this->UpdateViewMatrix();
}
