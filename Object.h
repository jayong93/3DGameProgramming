#pragma once
#include "Mesh.h"
#include "Shader.h"

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

class CTriangleObject : public CGameObject
{
public:
	CTriangleObject();
	virtual ~CTriangleObject();

	virtual void Animate(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

