#pragma once

class CVertex
{
public:
	CVertex() : position{ D3DXVECTOR3{0.0f,0.0f,0.0f} } {};
	CVertex(const D3DXVECTOR3& pos) : position{ pos } {}

private:
	D3DXVECTOR3 position;
};

class CMesh
{
public:
	CMesh();
	virtual ~CMesh();
	void AddRef();
	void Release();
	virtual void Render(ID3D11DeviceContext* deviceContext);

protected:
	ID3D11Buffer* vertexBuffer;
	UINT vertexCnt;
	UINT strideByte;
	UINT offset;

	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;

private:
	int reference;

};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D11Device* device);
	virtual ~CTriangleMesh();

	virtual void Render(ID3D11DeviceContext* deviceContext);
};

