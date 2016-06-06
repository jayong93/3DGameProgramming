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

protected:
	CCamera* camera;
};

