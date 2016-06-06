#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh(D3D11_FILL_MODE type) : strideByte{ sizeof(CVertex) }, offset{ 0 }, reference{ 0 }, primitiveTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST }, rasterizserState{ nullptr }, drawType{ type }, indexBuffer{ nullptr }, indexCnt{ 0 }, startIndex{ 0 }, baseVertex{ 0 }
{
}


CMesh::~CMesh()
{
	if (vertexBuffer) vertexBuffer->Release();
	if (indexBuffer) indexBuffer->Release();
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
	if (indexBuffer) deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(primitiveTopology);
	deviceContext->RSSetState(rasterizserState);
	if (indexBuffer)
	{
		deviceContext->DrawIndexed(indexCnt, startIndex, baseVertex);
	}
	else
	{
		deviceContext->Draw(vertexCnt, offset);
	}
}

void CMesh::CreateRasterizerState(ID3D11Device * device)
{
}

CTriangleMesh::CTriangleMesh(ID3D11Device * device, D3D11_FILL_MODE type) : CMesh(type)
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
	rd.CullMode = (drawType == D3D11_FILL_SOLID) ? D3D11_CULL_BACK : D3D11_CULL_NONE;
	rd.FillMode = drawType;
	device->CreateRasterizerState(&rd, &rasterizserState);
}

CCubeMesh::CCubeMesh(ID3D11Device * device, D3D11_FILL_MODE type, float width, float height, float depth) : CMesh(type)
{
	vertexCnt = 8;
	strideByte = sizeof(CDiffusedVertex);
	offset = 0;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float x{ width*0.5f }, y{ height*0.5f }, z{ depth*0.5f };

	CDiffusedVertex v[36];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	v[i++] = CDiffusedVertex(D3DXVECTOR3(-x, +y, -z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(+x, +y, -z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(+x, +y, +z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(-x, +y, +z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(-x, -y, -z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(+x, -y, -z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(+x, -y, +z), RANDOM_COLOR());
	v[i++] = CDiffusedVertex(D3DXVECTOR3(-x, -y, +z), RANDOM_COLOR());

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = strideByte*vertexCnt;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = v;
	device->CreateBuffer(&bd, &data, &vertexBuffer);

	indexCnt = 36;

	UINT pIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pIndices[0] = 3; pIndices[1] = 1; pIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pIndices[6] = 0; pIndices[7] = 5; pIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pIndices[9] = 1; pIndices[10] = 5; pIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pIndices[12] = 3; pIndices[13] = 4; pIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pIndices[15] = 0; pIndices[16] = 4; pIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pIndices[18] = 1; pIndices[19] = 6; pIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pIndices[21] = 2; pIndices[22] = 6; pIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pIndices[24] = 2; pIndices[25] = 7; pIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pIndices[27] = 3; pIndices[28] = 7; pIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pIndices[30] = 6; pIndices[31] = 4; pIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pIndices[33] = 7; pIndices[34] = 4; pIndices[35] = 6;

	bd.ByteWidth = sizeof(UINT) * indexCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	data.pSysMem = pIndices;
	device->CreateBuffer(&bd, &data, &indexBuffer);

	CreateRasterizerState(device);
}

CCubeMesh::~CCubeMesh()
{
}

void CCubeMesh::CreateRasterizerState(ID3D11Device * device)
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = (drawType == D3D11_FILL_SOLID) ? D3D11_CULL_BACK : D3D11_CULL_NONE;
	rd.FillMode = drawType;
	device->CreateRasterizerState(&rd, &rasterizserState);
}

void CCubeMesh::Render(ID3D11DeviceContext * deviceContext)
{
	CMesh::Render(deviceContext);
}
