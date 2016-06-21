#pragma once

inline XMFLOAT4A RANDOM_COLOR() { return XMFLOAT4A{ RandomNormalizeFloat(),RandomNormalizeFloat(),RandomNormalizeFloat(),1.f }; }

class CVertex
{
public:
	CVertex() : position{ D3DXVECTOR3{0.0f,0.0f,0.0f} } {};
	CVertex(const D3DXVECTOR3& pos) : position{ pos } {}

private:
	XMFLOAT3A position;
};

class CDiffusedVertex
{
public:
	CDiffusedVertex(float x, float y, float z, XMFLOAT4A color) : position{ XMFLOAT3A{x,y,z} }, diffuse{ color } {}
	CDiffusedVertex(FXMVECTOR pos, FXMVECTOR normal, FXMVECTOR color)
	{
		XMStoreFloat3A(&position, pos);
		XMStoreFloat4A(&diffuse, color);
		XMVECTOR n = XMVector3Normalize(normal);
		XMStoreFloat3A(&this->normal, n);
	}
	CDiffusedVertex() : position{ 0,0,0 }, diffuse{ 0,0,0,0 } {}
	~CDiffusedVertex() {}

private:
	XMFLOAT3A position;
	XMFLOAT4A diffuse;
	XMFLOAT3A normal;
};

class CMesh
{
public:
	CMesh(D3D11_FILL_MODE type);
	virtual ~CMesh();
	void AddRef();
	void Release();
	virtual void Render(ID3D11DeviceContext* deviceContext);
	virtual void CreateRasterizerState(ID3D11Device* device);

	const DirectX::BoundingBox& GetAABB() const { return aabb; }

protected:
	ID3D11Buffer* vertexBuffer;
	UINT vertexCnt;
	UINT strideByte;
	UINT offset;

	ID3D11Buffer* indexBuffer;
	UINT indexCnt;
	UINT startIndex;
	int baseVertex;

	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
	ID3D11RasterizerState* rasterizserState;
	D3D11_FILL_MODE drawType;

	DirectX::BoundingBox aabb;

private:
	int reference;

};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D11Device* device, D3D11_FILL_MODE type);
	virtual ~CTriangleMesh();

	virtual void Render(ID3D11DeviceContext* deviceContext);
	virtual void CreateRasterizerState(ID3D11Device* device);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(ID3D11Device* device, D3D11_FILL_MODE type, FXMVECTOR color, float width = 2.0f, float height = 2.0f, float depth = 2.0f);
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device* device);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};

class SphereMesh : public CMesh
{
public:
	SphereMesh(ID3D11Device* device, D3D11_FILL_MODE type, FXMVECTOR color, float radius, int slice, int stack);
	virtual ~SphereMesh();

	virtual void CreateRasterizerState(ID3D11Device* device);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};