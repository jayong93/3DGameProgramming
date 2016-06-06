#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh() : strideByte{ sizeof(CVertex) }, offset{ 0 }, reference{ 0 }, primitiveTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST }, rasterizserState{ nullptr }
{
}


CMesh::~CMesh()
{
	if (vertexBuffer) vertexBuffer->Release();
	if (rasterizserState) rasterizserState->Release();
}

void CMesh::AddRef()
{
	++reference;
}

void CMesh::Release()
{
	--reference;
	if (reference <= 0) delete this;
}

void CMesh::Render(ID3D11DeviceContext * deviceContext)
{
	if (vertexBuffer) deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &strideByte, &offset);
	deviceContext->IASetPrimitiveTopology(primitiveTopology);
	deviceContext->RSSetState(rasterizserState);
	deviceContext->Draw(vertexCnt, offset);
}

void CMesh::CreateRasterizerState(ID3D11Device * device)
{
}

CTriangleMesh::CTriangleMesh(ID3D11Device * device)
{
	vertexCnt = 3;
	strideByte = sizeof(CDiffusedVertex);
	offset = 0;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex v[3];
	v[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR{ 1.0f,0.0f,0.0f,1.0f });
	v[1] = CDiffusedVertex(D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR{ 0.0f,1.0f,0.0f,1.0f });
	v[2] = CDiffusedVertex(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR{ 0.0f,0.0f,1.0f,1.0f });

	D3D11_BUFFER_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.ByteWidth = strideByte * 3;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA bData;
	ZeroMemory(&bData, sizeof(D3D11_SUBRESOURCE_DATA));
	bData.pSysMem = v;
	device->CreateBuffer(&bDesc, &bData, &vertexBuffer);

	CreateRasterizerState(device);
}

CTriangleMesh::~CTriangleMesh()
{
}

void CTriangleMesh::Render(ID3D11DeviceContext * deviceContext)
{
	CMesh::Render(deviceContext);
}

void CTriangleMesh::CreateRasterizerState(ID3D11Device * device)
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = D3D11_CULL_NONE;
	rd.FillMode = D3D11_FILL_SOLID;
	device->CreateRasterizerState(&rd, &rasterizserState);
}
