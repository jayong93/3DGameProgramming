#pragma once
#include "Mesh.h"
#include "Camera.h"

class CGameObject
{
public:
	CGameObject(int meshNum = 1);
	virtual ~CGameObject();

	void AddRef() { ++reference; }
	void Release() { if (--reference < 0) delete this; }

	virtual void SetMesh(CMesh* m, int index = 0);
	virtual CMesh* GetMesh(int index = 0) { return meshes[index]; }
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

	CMesh** meshes;
	int meshCount;

private:
	BoundingBox aabb;
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

class HeightMap
{
public:
	HeightMap(LPCTSTR fileName, int width, int length, FXMVECTOR scale);
	~HeightMap();

	float GetHeight(float x, float z, bool reverseQuad = false);
	XMVECTOR GetHeightMapNormal(int x, int z);
	XMVECTOR GetScale() { return XMLoadFloat3A(&scale); }

	BYTE* GetHeightMapImage() { return imageData; }
	int GetHeightMapWidth() { return width; }
	int GetHeightMapLength() { return length; }

private:
	BYTE* imageData;

	int width;
	int length;

	XMFLOAT3A scale;
};

class HeightMapTerrain : public CGameObject
{
public:
	HeightMapTerrain(ID3D11Device* device, LPCTSTR fileName, int width, int length, int blockWidth, int blockLength, FXMVECTOR scale, FXMVECTOR color);
	virtual ~HeightMapTerrain() { if (heightMap) delete heightMap; }

	float GetHeight(float x, float z, bool reverseQuad = false) { return heightMap->GetHeight(x, z, reverseQuad)*scale.y; }

	int GetHeightMapWidth() { return heightMap->GetHeightMapWidth(); }
	int GetHeightMapLength() { return heightMap->GetHeightMapLength(); }

	XMVECTOR GetScale() { return XMLoadFloat3A(&scale); }

	float GetWidth() { return width*scale.x; }
	float GetLength() { return length*scale.z; }

	float GetPeakHeight();

private:
	HeightMap* heightMap;

	int width;
	int length;

	XMFLOAT3A scale;
};