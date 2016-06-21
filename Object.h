#pragma once
#include "Mesh.h"
#include "Camera.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	void AddRef() { ++reference; }
	void Release() { if (--reference < 0) delete this; }

	virtual void SetMesh(CMesh* m);
	virtual void Animate(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext);

	void Move(float x, float y, float z);
	void Move(FXMVECTOR shift);
	void Move(DWORD dir, float speed);

	XMVECTOR GetPosition() const { return XMVectorSet(mtxWorld._41, mtxWorld._42, mtxWorld._43, 0); }
	void SetPosition(FXMVECTOR pos)
	{
		XMFLOAT3A vp;
		XMStoreFloat3A(&vp, pos);
		mtxWorld._41 = vp.x; mtxWorld._42 = vp.y; mtxWorld._43 = vp.z;
	}

	bool GetAABB(BoundingBox& aabb) const;
	bool GetOOBB(BoundingOrientedBox& oobb) const;
	bool GetBoundingSphere(BoundingSphere& sphere) const;
	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4A(&mtxWorld); }
	void SetWorldMatrix(FXMMATRIX mat) { XMStoreFloat4x4A(&mtxWorld, mat); }

	XMFLOAT4X4A mtxWorld;
	CMesh* mesh;

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

class ChasingObject : public CGameObject
{
public:
	ChasingObject(CGameObject* tg = nullptr) :target{ tg }, angleSpeed{ 180.f } {};
	virtual ~ChasingObject() {}

	virtual void Animate(float deltaTime);

	void SetTarget(CGameObject* t) { target = t; }

protected:
	CGameObject* target;
	float speed{ 3.f }, angleSpeed;
};
