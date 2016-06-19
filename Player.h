#pragma once
#include "Object.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	void SetCamera(CCamera* c) { camera = c; }
	CCamera* GetCamera() { return camera; }

	void CreateShaderVariables(ID3D11Device* device);
	void UpdateShaderVariables(ID3D11DeviceContext* deviceContext);

	D3DXVECTOR3 GetPosition() const { return position; }
	D3DXVECTOR3 GetLookVector() const { return look; }
	D3DXVECTOR3 GetUpVector() const { return up; }
	D3DXVECTOR3 GetRightVector() const { return right; }

	void SetFriction(float fric) { friction = fric; }
	void SetGravity(const D3DXVECTOR3& g) { gravity = g; }
	void SetMaxVelocityXZ(float v) { maxVelocityXZ = v; }
	void SetMaxVelocityY(float v) { maxVelocityY = v; }
	void SetVelocity(const D3DXVECTOR3& v) { velocity = v; }

	void SetPosition(const D3DXVECTOR3& pos) { Move((pos - position), false); }

	const D3DXVECTOR3& GetVelocity() const { return velocity; }
	float GetYaw() const { return yaw; }
	float GetPitch() const { return pitch; }
	float GetRoll() const { return roll; }

	void Move(ULONG direction, float dist, bool velocity = false);
	void Move(const D3DXVECTOR3& shift, bool velocity = false);
	void Move(float xOffset = 0.f, float yOffset = 0.f, float zOffset = 0.f) {}

	void Rotate(float x, float y, float z);

	void Update(float timeElapsed);

	virtual void OnPlayerUpdated(float timeElapsed);
	void SetPlayerUpdatedContext(void* context) { playerUpdatedContext = context; }
	void OnCameraUpdated(float timeElapsed);
	void SetCameraUpdatedContext(void* context) { cameraUpdatedContext = context; }

	CCamera* OnChangeCamera(ID3D11Device* device, CameraMode newMode, CameraMode currentMode);
	virtual void ChangeCamera(ID3D11Device* device, CameraMode newMode, float timeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(ID3D11DeviceContext* deviceContext);

protected:
	CCamera* camera;

	D3DXVECTOR3 position{ 0.f,0.f,0.f };
	D3DXVECTOR3 right{ 1.f,0.f,0.f };
	D3DXVECTOR3 up{ 0.f,1.f,0.f };
	D3DXVECTOR3 look{ 0.f,0.f,1.f };

	float pitch{ 0.f }, yaw{ 0.f }, roll{ 0.f };

	D3DXVECTOR3 velocity{ 0.f,0.f,0.f };
	D3DXVECTOR3 gravity{ 0.f,0.f,0.f };
	float maxVelocityXZ{ 0.f };
	float maxVelocityY{ 0.f };
	float friction{ 0.f };

	void* playerUpdatedContext{ nullptr };
	void* cameraUpdatedContext{ nullptr };
};

class CCubePlayer : public CPlayer
{
public:
	CCubePlayer(ID3D11Device* device);
	virtual ~CCubePlayer() {}

	virtual void ChangeCamera(ID3D11Device* device, CameraMode newMode, float timeElapsed);
	virtual void OnPrepareRender();
};