#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

#define FORWARD 0x01
#define BACKWARD 0x02
#define LEFT 0x04
#define RIGHT 0x08
#define UP 0x10
#define DOWN 0x20

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef() { ++reference; }
	void Release() { if (--reference < 0) delete this; }

	void MoveStrafe(float dist = 1.f);
	void MoveUp(float dist = 1.f);
	void MoveForward(float dist = 1.f);

	void Rotate(float pitch = 10.f, float yaw = 10.f, float roll = 10.f);
	void Rotate(D3DXVECTOR3* axis, float angle);

	D3DXVECTOR3 GetLookAt();
	D3DXVECTOR3 GetUp();
	D3DXVECTOR3 GetRight();
	D3DXVECTOR3 GetPosition();
	void SetPosition(const D3DXVECTOR3& pos);

	virtual void OnPrepareRender() {}

	virtual void SetMesh(CMesh* m);
	virtual void SetShader(CShader* s);
	virtual void Animate(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	D3DXMATRIX mtxWorld;
	CMesh* mesh;
	CShader* shader;
private:
	int reference;
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	void SetRotationSpeed(float speed) { rotationSpeed = speed; }
	void SetRotatedAxis(D3DXVECTOR3 axis) { rotationAxis = axis; }

protected:
	float rotationSpeed;
	D3DXVECTOR3 rotationAxis;
};

