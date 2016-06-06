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

