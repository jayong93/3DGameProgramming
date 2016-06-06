#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef() { ++reference; }
	void Release() { if (--reference < 0) delete this; }

	virtual void SetMesh(CMesh* m);
	virtual void SetShader(CShader* s);
	virtual void Animate(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	void SetPos(const D3DXVECTOR3& pos) { mtxWorld._41 = pos.x; mtxWorld._42 = pos.y; mtxWorld._43 = pos.z; }
	D3DXVECTOR3 GetPos() const { return D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43); }

	D3DXMATRIX mtxWorld;
	CMesh* mesh;
	CShader* shader;
private:
	int reference;
};

class CRotatingObject : public CGameObject
{
public:
	// degree angle
	CRotatingObject(float yAngle);
	virtual ~CRotatingObject();

	virtual void Animate(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext);
	void SetVelocity(float vx, float vy, float vz);
	const D3DXVECTOR3& GetVelocity() const { return velocity; }

protected:
	float yAnglePerSecond;
	D3DXVECTOR3 velocity{ 0,0,0 };
};

