#pragma once

inline D3DXCOLOR RANDOM_COLOR() { return D3DXCOLOR{ (UINT)((rand() * 0xffffff) / RAND_MAX) }; }

class CVertex
{
public:
	CVertex() : position{ D3DXVECTOR3{0.0f,0.0f,0.0f} } {};
	CVertex(const D3DXVECTOR3& pos) : position{ pos } {}

private:
	D3DXVECTOR3 position;
};

class CDiffusedVertex
{
public:
	CDiffusedVertex(float x, float y, float z, D3DXCOLOR color) : position{ D3DXVECTOR3{x,y,z} }, diffuse{ color } {}
	CDiffusedVertex(D3DXVECTOR3 pos, D3DXCOLOR color) : position{ pos }, diffuse{ color } {}
	CDiffusedVertex() : position{ 0,0,0 }, diffuse{ 0,0,0,0 } {}
	~CDiffusedVertex() {}

private:
	D3DXVECTOR3 position;
	D3DXCOLOR diffuse;
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
	CCubeMesh(ID3D11Device* device, D3D11_FILL_MODE type, D3DXCOLOR color, float width = 2.0f, float height = 2.0f, float depth = 2.0f);
	virtual ~CCubeMesh();

	virtual void CreateRasterizerState(ID3D11Device* device);
	virtual void Render(ID3D11DeviceContext* deviceContext);
};
