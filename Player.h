#pragma once
#include "Object.h"
#include "Camera.h"

#define FORWARD 0x01
#define BACKWARD 0x02
#define LEFT 0x04
#define RIGHT 0x08
#define UP 0x10
#define DOWN 0x20

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

	void SetCamera(CCamera* c) { camera = c; }
	CCamera* GetCamera() { return camera; }

	void CreateShaderVariables(ID3D11Device* device);
	void UpdateShaderVariables(ID3D11DeviceContext* deviceContext);

	void Update(float timeElapsed) { if (camera) camera->UpdateViewMatrix(); }

protected:
	CCamera* camera;
};

