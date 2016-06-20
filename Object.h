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

	D3DXVECTOR3 GetPosition() const { return D3DXVECTOR3{ mtxWorld._41,mtxWorld._42,mtxWorld._43 }; }
	void SetPosition(const D3DXVECTOR3& pos) { mtxWorld._41 = pos.x; mtxWorld._42 = pos.y; mtxWorld._43 = pos.z; }

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
};

