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
	~CMesh();
};

