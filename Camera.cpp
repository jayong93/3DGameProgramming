#include "stdafx.h"
#include "Player.h"
#include "Camera.h"


CCamera::CCamera() : player(nullptr), cbCamera{ nullptr }
{
	XMStoreFloat4x4A(&mtxView, XMMatrixIdentity());
	XMStoreFloat4x4A(&mtxProjection, XMMatrixIdentity());
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
	XMStoreFloat4x4A(&mtxView, XMMatrixLookAtLH(XMLoadFloat3A(&position), XMLoadFloat3A(&lookAt), XMLoadFloat3A(&up)));
}

void CCamera::CreateProjectionMatrix(float nearDist, float farDist, float aspect, float fov)
{
	XMMATRIX mat = XMMatrixPerspectiveFovLH((float)D3DXToRadian(fov), aspect, nearDist, farDist);
	XMStoreFloat4x4A(&mtxProjection, mat);
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
	XMMATRIX mat = XMLoadFloat4x4A(&mtxView);
	XMStoreFloat4x4A(&viewProjection->mtxView, XMMatrixTranspose(mat));
	mat = XMLoadFloat4x4A(&mtxProjection);
	XMStoreFloat4x4A(&viewProjection->mtxProjection, XMMatrixTranspose(mat));
	deviceContext->Unmap(cbCamera, 0);
	deviceContext->VSSetConstantBuffers(VS_SLOT_CAMERA, 1, &cbCamera);
}

ThirdCam::ThirdCam()
{
	offset = XMFLOAT3A{ 0.f,2.f,-5.f };
	lookAt = XMFLOAT3A{ 0.f,-5.f,10.f };
}

void ThirdCam::SetPlayer(CPlayer * p)
{
	CCamera::SetPlayer(p);

	XMStoreFloat3A(&position, p->GetPosition() + XMLoadFloat3A(&offset));
	XMStoreFloat3A(&lookAt, p->GetPosition());
}

void ThirdCam::CreateViewMatrix()
{
	XMMATRIX rm;
	XMVECTOR rotatedOffset;
	rm = XMMatrixRotationX(XMConvertToRadians(xAngle));
	rotatedOffset = XMVector3TransformCoord(XMLoadFloat3A(&offset), rm);
	rm = XMMatrixRotationY(XMConvertToRadians(yAngle));
	rotatedOffset = XMVector3TransformCoord(rotatedOffset, rm);

	XMStoreFloat3A(&position, player->GetPosition() + rotatedOffset);
	XMStoreFloat3A(&lookAt, player->GetPosition());
	XMStoreFloat4x4A(&mtxView, XMMatrixLookAtLH(XMLoadFloat3A(&position), XMLoadFloat3A(&lookAt), XMLoadFloat3A(&up)));
}

void ThirdCam::UpdateViewMatrix()
{
	XMMATRIX rm;
	XMVECTOR rotatedOffset;
	rm = XMMatrixRotationX(XMConvertToRadians(xAngle));
	rotatedOffset = XMVector3TransformCoord(XMLoadFloat3A(&offset), rm);
	rm = XMMatrixRotationY(XMConvertToRadians(yAngle));
	rotatedOffset = XMVector3TransformCoord(rotatedOffset, rm);

	XMStoreFloat3A(&position, player->GetPosition() + rotatedOffset);
	XMStoreFloat3A(&lookAt, player->GetPosition());
	XMStoreFloat4x4A(&mtxView, XMMatrixLookAtLH(XMLoadFloat3A(&position), XMLoadFloat3A(&lookAt), XMLoadFloat3A(&up)));
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
