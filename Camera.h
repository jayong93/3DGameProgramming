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

	void SetPlayer(CPlayer* p) { player = p; }

	void SetViewport(ID3D11DeviceContext* deviceContext, DWORD xStart, DWORD yStart, DWORD width, DWORD height, float minZ = 0.0f, float maxZ = 1.0f);

	void CreateViewMatrix(D3DXVECTOR3 eyePos, D3DXVECTOR3 lookAt, D3DXVECTOR3 up);
	void CreateProjectionMatrix(float nearDist, float farDist, float aspect, float fov);
	void CreateViewMatrix() { D3DXMatrixLookAtLH(&mtxView, &position, &lookAt, &up); }
	
	void CreateShaderVariable(ID3D11Device* device);
	void UpdateShaderVariable(ID3D11DeviceContext* deviceContext);

	const D3DXMATRIX& GetViewMatrix() const { return mtxView; }
	const D3DXMATRIX& GetProjectionMatrix() const { return mtxProjection; }
	const D3D11_VIEWPORT& GetViewport() const { return viewport; }

	void SetPosition(D3DXVECTOR3 const& pos) { position = pos; }
	const D3DXVECTOR3& GetPosition() const { return position; }
protected:
	D3DXMATRIX mtxView;
	D3DXMATRIX mtxProjection;
	D3DXVECTOR3 position{ 0,0,0 };
	D3DXVECTOR3 lookAt{ 0,0,0 };
	D3DXVECTOR3 up{ 0,1,0 };

	D3D11_VIEWPORT viewport;

	ID3D11Buffer* cbCamera;

	CPlayer* player;
};

