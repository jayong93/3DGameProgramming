#include "stdafx.h"
#include "Mesh.h"
#include "Object.h"


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
	v[0] = CDiffusedVertex(XMVectorSet(0.0f, 0.5f, 0.0f, 0.f), XMVectorSet(0.0f, 0.5f, 0.0f, 0.f), XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f));
	v[1] = CDiffusedVertex(XMVectorSet(0.5f, -0.5f, 0.0f, 0.f), XMVectorSet(0.5f, -0.5f, 0.0f, 0.f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
	v[2] = CDiffusedVertex(XMVectorSet(-0.5f, -0.5f, 0.0f, 0.f), XMVectorSet(-0.5f, -0.5f, 0.0f, 0.f), XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f));

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

CCubeMesh::CCubeMesh(ID3D11Device * device, D3D11_FILL_MODE type, FXMVECTOR color, float width, float height, float depth) : CMesh(type)
{
	vertexCnt = 8;
	strideByte = sizeof(CDiffusedVertex);
	offset = 0;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float x{ width*0.5f }, y{ height*0.5f }, z{ depth*0.5f };

	CDiffusedVertex v[8];
	int i = 0;

	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다.
	v[i++] = CDiffusedVertex(XMVectorSet(-x, +y, -z, 0.f), XMVectorSet(-1.f, +1.f, -1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(+x, +y, -z, 0.f), XMVectorSet(+1.f, +1.f, -1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(+x, +y, +z, 0.f), XMVectorSet(+1.f, +1.f, +1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(-x, +y, +z, 0.f), XMVectorSet(-1.f, +1.f, +1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(-x, -y, -z, 0.f), XMVectorSet(-1.f, -1.f, -1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(+x, -y, -z, 0.f), XMVectorSet(+1.f, -1.f, -1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(+x, -y, +z, 0.f), XMVectorSet(+1.f, -1.f, +1.f, 0.f), color);
	v[i++] = CDiffusedVertex(XMVectorSet(-x, -y, +z, 0.f), XMVectorSet(-1.f, -1.f, +1.f, 0.f), color);

	DirectX::BoundingBox::CreateFromPoints(aabb, 8, (DirectX::XMFLOAT3*)v, sizeof(CDiffusedVertex));

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

SphereMesh::SphereMesh(ID3D11Device * device, D3D11_FILL_MODE type, FXMVECTOR color, float radius, int slice, int stack) : CMesh{ type }
{
	vertexCnt = slice*stack * 3 * 2;
	strideByte = sizeof(CDiffusedVertex);
	offset = 0;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex* v = new CDiffusedVertex[vertexCnt];

	float theta_i, theta_ii, phi_j, phi_jj, radius_j, radius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < stack; ++j) {
		phi_j = XM_PI / float(stack) * j;
		phi_jj = XM_PI / float(stack) * (j + 1);

		radius_j = radius * sinf(phi_j);
		radius_jj = radius * sinf(phi_jj);
		y_j = radius*cosf(phi_j);
		y_jj = radius*cosf(phi_jj);

		for (int i = 0; i < slice; ++i) {
			theta_i = ((2 * XM_PI) / (float)slice)*i;
			theta_ii = ((2 * XM_PI) / (float)slice)*(i + 1);

			XMVECTOR pos = XMVectorSet(radius_j * cosf(theta_i), y_j, radius_j * sinf(theta_i), 0.f);
			v[k++] = CDiffusedVertex{ pos, XMVector3Normalize(pos), color };
			pos = XMVectorSet(radius_jj * cosf(theta_i), y_jj, radius_jj * sinf(theta_i), 0.f);
			v[k++] = CDiffusedVertex{ pos, XMVector3Normalize(pos), color };
			pos = XMVectorSet(radius_j * cosf(theta_ii), y_j, radius_j * sinf(theta_ii), 0.f);
			v[k++] = CDiffusedVertex{ pos, XMVector3Normalize(pos), color };

			pos = XMVectorSet(radius_jj * cosf(theta_i), y_jj, radius_jj * sinf(theta_i), 0.f);
			v[k++] = CDiffusedVertex{ pos, XMVector3Normalize(pos), color };
			pos = XMVectorSet(radius_jj * cosf(theta_ii), y_jj, radius_jj * sinf(theta_ii), 0.f);
			v[k++] = CDiffusedVertex{ pos, XMVector3Normalize(pos), color };
			pos = XMVectorSet(radius_j * cosf(theta_ii), y_j, radius_j * sinf(theta_ii), 0.f);
			v[k++] = CDiffusedVertex{ pos, XMVector3Normalize(pos), color };
		}
	}

	DirectX::BoundingBox::CreateFromPoints(aabb, vertexCnt, (DirectX::XMFLOAT3*)v, sizeof(CDiffusedVertex));

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

	delete[] v;

	CreateRasterizerState(device);
}

SphereMesh::~SphereMesh()
{
}

void SphereMesh::CreateRasterizerState(ID3D11Device * device)
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = D3D11_CULL_NONE;
	rd.FillMode = drawType;
	rd.DepthClipEnable = true;
	device->CreateRasterizerState(&rd, &rasterizserState);
}

void SphereMesh::Render(ID3D11DeviceContext * deviceContext)
{
	CMesh::Render(deviceContext);
}

HeightMapGridMesh::HeightMapGridMesh(ID3D11Device * device, D3D11_FILL_MODE type, int xStart, int zStart, int width, int length, FXMVECTOR scale, FXMVECTOR color, void * context) : CMesh(type), width{ width }, length{ length }
{
	XMStoreFloat3A(&this->scale, scale);

	vertexCnt = width*length;
	strideByte = sizeof(CDiffusedVertex);
	offset = 0;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	CDiffusedVertex* v = new CDiffusedVertex[vertexCnt];

	float height{ 0.0f }, minHeight{ +FLT_MAX }, maxHeight{ -FLT_MAX };
	for (int i = 0, z = zStart; z < (zStart + length); ++z) {
		for (int x = xStart; x < (xStart + width); ++x) {
			height = OnGetHeight(x, z, context);
			v[i++] = CDiffusedVertex{ XMVectorSet(x*this->scale.x, height, z*this->scale.z, 1.0f),
				OnGetNormal(x, z, context),
				XMVectorMultiply(OnGetColor(x, z, context), color) };
			if (height < minHeight) minHeight = height;
			if (height > maxHeight) maxHeight = height;
		}
	}

	DirectX::BoundingBox::CreateFromPoints(aabb, vertexCnt, (DirectX::XMFLOAT3*)v, sizeof(CDiffusedVertex));

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

	delete[] v;

	indexCnt = ((width * 2)*(length - 1)) + ((length - 1) - 1);
	UINT* indices = new UINT[indexCnt];
	for (int j = 0, z = 0; z < length - 1; ++z) {
		if ((z % 2) == 0) {
			for (int x = 0; x < width; ++x) {
				if ((x == 0) && (z > 0))
					indices[j++] = (UINT)(x + (z * width));
				indices[j++] = (UINT)(x + (z * width));
				indices[j++] = (UINT)((x + (z * width)) + width);
			}
		}
		else {
			for (int x = width - 1; x >= 0; --x) {
				if (x == (width - 1)) indices[j++] = (UINT)(x + (z*width));
				indices[j++] = (UINT)(x + (z*width));
				indices[j++] = (UINT)((x + (z*width)) + width);
			}
		}
	}

	bd.ByteWidth = sizeof(UINT) * indexCnt;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	data.pSysMem = indices;
	device->CreateBuffer(&bd, &data, &indexBuffer);

	delete[] indices;

	CreateRasterizerState(device);
}

HeightMapGridMesh::~HeightMapGridMesh()
{
}

float HeightMapGridMesh::OnGetHeight(int x, int z, void * context)
{
	HeightMap* heightMap = (HeightMap*)context;
	BYTE* imageData = heightMap->GetHeightMapImage();
	XMVECTOR scale = heightMap->GetScale();
	int cxTerrain = heightMap->GetHeightMapWidth();
	float height = imageData[x + (z* cxTerrain)] * XMVectorGetY(scale);
	return height;
}

XMVECTOR HeightMapGridMesh::OnGetColor(int x, int z, void * context)
{
	return XMVectorReplicate(1.f);
}

XMVECTOR HeightMapGridMesh::OnGetNormal(int x, int z, void * context)
{
	HeightMap* heightMap = (HeightMap*)context;
	return heightMap->GetHeightMapNormal(x, z);
}

void HeightMapGridMesh::CreateRasterizerStaion(ID3D11Device * device)
{
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = (drawType == D3D11_FILL_SOLID) ? D3D11_CULL_BACK : D3D11_CULL_NONE;
	rd.FillMode = drawType;
	device->CreateRasterizerState(&rd, &rasterizserState);
}

void HeightMapGridMesh::Render(ID3D11DeviceContext * deviceContext)
{
	CMesh::Render(deviceContext);
}
