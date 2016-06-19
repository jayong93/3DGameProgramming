#pragma once

enum class CameraMode { NONE, FIRST_PERSON, SPACESHIP, THIRD_PERSON };
const float ASPECT_RATIO{ (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT };

struct VS_CB_CAMERA
{
	D3DXMATRIX mtxView;
	D3DXMATRIX mtxProjection;
};

class CPlayer;

class CCamera
{
public:
	CCamera(CCamera* camera);
	virtual ~CCamera();

	void SetMode(CameraMode m) { mode = m; }
	CameraMode GetMode() const { return mode; }

	void SetPlayer(CPlayer* p) { player = p; }
	CPlayer* GetPlayer() { return player; }

	void SetViewport(ID3D11DeviceContext* deviceContext, DWORD xStart, DWORD yStart, DWORD width, DWORD height, float minZ = 0.0f, float maxZ = 1.0f);
	D3D11_VIEWPORT GetViewport() const { return viewport; }

	void CreateViewMatrix();
	void RecreateViewMatrix();
	D3DXMATRIX GetViewMatrix() const { return mtxView; }
	void CreateProjectionMatrix(float nearDist, float farDist, float aspect, float fov);
	D3DXMATRIX GetProjectionMatrix() const { return mtxProjection; }
	ID3D11Buffer* GetCameraConstantBuffer() const { return cbCamera; }

	void SetPosition(D3DXVECTOR3 pos) { position = pos; }
	D3DXVECTOR3& GetPosition() { return position; }

	D3DXVECTOR3& GetRightVector() { return right; }
	D3DXVECTOR3& GetUpVector() { return up; }
	D3DXVECTOR3& GetLookVector() { return look; }

	float& GetPitch() { return pitch; }
	float& GetYaw() { return yaw; }
	float& GetRoll() { return roll; }

	void SetOffset(D3DXVECTOR3 os) { offset = os; position += os; }
	D3DXVECTOR3& GetOffset() { return offset; }

	void SetTimeLag(float lag) { timeLag = lag; }
	float GetTimeLag() const { return timeLag; }

	virtual void Move(const D3DXVECTOR3& shift) { position += shift; }
	virtual void Rotate(float pitch = 0.f, float yaw = 0.f, float roll = 0.f) {}
	virtual void Update(D3DXVECTOR3& lookAt, float timeElapsed) {}
	virtual void SetLookAt(D3DXVECTOR3& lookAt) {}

	void CreateShaderVariable(ID3D11Device* device);
	void UpdateShaderVariable(ID3D11DeviceContext* deviceContext);

protected:
	D3DXMATRIX mtxView;
	D3DXMATRIX mtxProjection;
	D3DXVECTOR3 position;
	D3DXVECTOR3 right;
	D3DXVECTOR3 up;
	D3DXVECTOR3 look;

	float pitch, yaw, roll;

	CameraMode mode;

	D3DXVECTOR3 offset;

	float timeLag;

	D3D11_VIEWPORT viewport;

	ID3D11Buffer* cbCamera;

	CPlayer* player;
};

class CSpaceShipCamera : public CCamera
{
public:
	CSpaceShipCamera(CCamera* cam);
	virtual ~CSpaceShipCamera() {}

	virtual void Rotate(float pitch = 0.f, float yaw = 0.f, float roll = 0.f);
};

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera(CCamera* cam);
	virtual ~CFirstPersonCamera() {}

	virtual void Rotate(float pitch = 0.f, float yaw = 0.f, float roll = 0.f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera(CCamera* cam);
	virtual ~CThirdPersonCamera() {}

	virtual void Update(D3DXVECTOR3& lookAt, float timeScale);
	virtual void SetLookAt(D3DXVECTOR3& lookAt);
};