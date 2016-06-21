#pragma once

struct VS_CB_CAMERA
{
	XMFLOAT4X4A mtxView;
	XMFLOAT4X4A mtxProjection;
};

class CPlayer;

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	virtual void SetPlayer(CPlayer* p) { player = p; }

	void SetViewport(ID3D11DeviceContext* deviceContext, DWORD xStart, DWORD yStart, DWORD width, DWORD height, float minZ = 0.0f, float maxZ = 1.0f);

	virtual void CreateViewMatrix();
	virtual void UpdateViewMatrix() {}
	void CreateProjectionMatrix(float nearDist, float farDist, float aspect, float fov);

	void CreateShaderVariable(ID3D11Device* device);
	void UpdateShaderVariable(ID3D11DeviceContext* deviceContext);

	void SetPosition(FXMVECTOR pos) { XMStoreFloat3A(&position, pos); }
	XMVECTOR GetPosition() const { return XMLoadFloat3A(&position); }

	void SetLookAt(FXMVECTOR look) { XMStoreFloat3A(&lookAt, look); }
	XMVECTOR GetLookAt() const { return XMLoadFloat3A(&lookAt); }

	void SetUp(FXMVECTOR up) { XMStoreFloat3A(&this->up, up); }
	XMVECTOR GetUp() const { return XMLoadFloat3A(&up); }

	virtual void Rotate(float x, float y, float z) {}

protected:
	XMFLOAT4X4A mtxView;
	XMFLOAT4X4A mtxProjection;

	XMFLOAT3A position{ 0.f,0.f,0.f };
	XMFLOAT3A lookAt{ 0.f,0.f,0.f };
	XMFLOAT3A up{ 0.f,1.f,0.f };
	XMFLOAT3A offset{ 0.f,0.f,0.f };

	D3D11_VIEWPORT viewport;

	ID3D11Buffer* cbCamera;

	CPlayer* player;
};

class ThirdCam : public CCamera
{
public:
	ThirdCam();
	virtual ~ThirdCam() {}

	virtual void SetPlayer(CPlayer* p);
	virtual void CreateViewMatrix();
	virtual void UpdateViewMatrix();

	virtual void Rotate(float x, float y, float z);

protected:
	float xAngle{ 0.f }, yAngle{ 0.f };
};