#pragma once

struct VS_CB_CAMERA
{
	D3DXMATRIX mtxView;
	D3DXMATRIX mtxProjection;
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

	void SetPosition(const D3DXVECTOR3& pos) { position = pos; }
	const D3DXVECTOR3& GetPosition() const { return position; }

	void SetLookAt(const D3DXVECTOR3& look) { lookAt = look; }
	const D3DXVECTOR3& GetLookAt() const { return lookAt; }

	void SetUp(const D3DXVECTOR3& up) { this->up = up; }
	const D3DXVECTOR3& GetUp() const { return up; }

	virtual void Rotate(float x, float y, float z) {}

protected:
	D3DXMATRIX mtxView;
	D3DXMATRIX mtxProjection;

	D3DXVECTOR3 position{ 0.f,0.f,0.f };
	D3DXVECTOR3 lookAt{ 0.f,0.f,0.f };
	D3DXVECTOR3 up{ 0.f,1.f,0.f };
	D3DXVECTOR3 offset{ 0.f,0.f,0.f };

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