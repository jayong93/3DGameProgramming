#include "stdafx.h"
#include "Shader.h"
#include "Object.h"


CGameObject::CGameObject(int meshNum) : meshes{ nullptr }, reference{ 1 }, meshCount{ meshNum }
{
	if (meshCount > 0) meshes = new CMesh*[meshCount];
	ZeroMemory(meshes, sizeof(CMesh*)*meshCount);
	XMStoreFloat4x4A(&mtxWorld, XMMatrixIdentity());
}


CGameObject::~CGameObject()
{
	if (meshes)
	{
		for (int i = 0; i < meshCount; i++)
		{
			if (meshes[i]) meshes[i]->Release();
		}
		delete[] meshes;
	}
}

void CGameObject::SetMesh(CMesh* m, int index)
{
	if (meshes[index]) meshes[index]->Release();
	meshes[index] = m;
	if (meshes[index]) meshes[index]->AddRef();

	aabb = BoundingBox{ {0,0,0},{0,0,0} };
	for (int i = 0; i < meshCount; ++i)
	{
		if (meshes[i])
		{
			BoundingBox box = meshes[i]->GetAABB();
			BoundingBox::CreateMerged(aabb, aabb, box);
		}
	}
}

void CGameObject::Animate(float deltaTime)
{
}

void CGameObject::Render(ID3D11DeviceContext * deviceContext)
{
	CShader::UpdateShaderVariable(deviceContext, XMLoadFloat4x4A(&mtxWorld));
	for (int i = 0; i < meshCount; i++)
	{
		if (meshes[i]) meshes[i]->Render(deviceContext);
	}
}

void CGameObject::Move(float x, float y, float z)
{
	this->Move(XMVectorSet(x, y, z, 0));
}

void CGameObject::Move(FXMVECTOR shift)
{
	auto pos = this->GetPosition();
	pos += shift;
	this->SetPosition(pos);
}

void CGameObject::Move(DWORD dir, float speed)
{
	if (dir)
	{
		XMFLOAT3A v{ 0,0,0 };

		if (dir & FORWARD) v.z += 1.f;
		if (dir & BACKWARD) v.z -= 1.f;

		if (dir & RIGHT) v.x += 1.f;
		if (dir & LEFT) v.x -= 1.f;

		if (dir & UP) v.y += 1.f;
		if (dir & DOWN) v.y -= 1.f;

		XMVECTOR shift = XMVector3Normalize(XMLoadFloat3A(&v));

		Move(shift * speed);
	}
}

bool CGameObject::GetAABB(BoundingBox & aabb) const
{
	if (meshes)
	{
		this->aabb.Transform(aabb, XMLoadFloat4x4A(&mtxWorld));
		return true;
	}
	return false;
}

bool CGameObject::GetOOBB(BoundingOrientedBox & oobb) const
{
	if (meshes)
	{
		BoundingOrientedBox::CreateFromBoundingBox(oobb, aabb);
		oobb.Transform(oobb, XMLoadFloat4x4A(&mtxWorld));
		return true;
	}
	return false;
}

bool CGameObject::GetBoundingSphere(BoundingSphere & sphere) const
{
	if (meshes)
	{
		BoundingSphere::CreateFromBoundingBox(sphere, aabb);
		sphere.Transform(sphere, XMLoadFloat4x4A(&mtxWorld));
		return true;
	}
	return false;
}

CRotatingObject::CRotatingObject()
{
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float deltaTime)
{
	XMMATRIX rMatrix;
	rMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f*deltaTime));
	XMStoreFloat4x4A(&mtxWorld, rMatrix * XMLoadFloat4x4A(&mtxWorld));
}

void CRotatingObject::Render(ID3D11DeviceContext * deviceContext)
{
	CGameObject::Render(deviceContext);
}

void ChasingObject::Animate(float deltaTime)
{
	if (target)
	{
		XMFLOAT4X4A rm;
		XMStoreFloat4x4A(&rm, this->GetWorldMatrix());
		rm._41 = rm._42 = rm._43 = 0.f;
		XMVECTOR forward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMVECTOR look = XMVector3TransformCoord(forward, XMLoadFloat4x4A(&rm));

		XMVECTOR targetPos = target->GetPosition();
		XMVECTOR myPos = this->GetPosition();
		XMVECTOR targetDir = targetPos - myPos;
		targetDir = XMVector3Normalize(targetDir);
		XMVECTOR axis = XMVector3Cross(targetDir, look);

		float dotResult = XMVectorGetX(XMVector3Dot(targetDir, look));
		dotResult = MinMax(-1.f, dotResult, 1.f);
		float angle = XMConvertToDegrees(XMScalarACos(dotResult));

		if (angle >= 0.01)
		{
			if (angle > angleSpeed*deltaTime)
				angle = angleSpeed*deltaTime;
			if (XMVectorGetY(axis) > 0)
				angle = -angle;
			angle = XMConvertToRadians(angle);

			XMMATRIX rm = XMMatrixRotationY(angle);
			this->SetWorldMatrix(rm * this->GetWorldMatrix());
			look = XMVector3TransformCoord(look, rm);
		}

		look *= speed*deltaTime;
		this->Move(look);
	}
}

HeightMap::HeightMap(LPCTSTR fileName, int width, int length, FXMVECTOR scale) : width{ width }, length{ length }
{
	XMStoreFloat3(&this->scale, scale);

	BYTE* image = new BYTE[width * length];

	HANDLE hFile = ::CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, image, (width* length), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	imageData = new BYTE[width * length];
	for (int y = 0; y < length; ++y) {
		for (int x = 0; x < width; ++x) {
			imageData[x + ((length - 1 - y) * width)] = image[x + (y*width)];
		}
	}
	if (image) delete[] image;
}

HeightMap::~HeightMap()
{
	if (imageData) delete[] imageData;
}

float HeightMap::GetHeight(float x, float z, bool reverseQuad)
{
	x = x / scale.x;
	z = z / scale.z;

	if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= length))
		return 0.f;

	int ix = int(x);
	int iz = int(z);
	float xPercent{ x - ix };
	float zPercent{ z - iz };
	if (iz & 1) reverseQuad = true;

	float topLeft = imageData[ix + ((iz + 1) * width)];
	float topRight = imageData[(ix + 1) + ((iz + 1) * width)];
	float bottomLeft = imageData[ix + iz*width];
	float bottomRight = imageData[(ix + 1) + iz*width];

	if (reverseQuad) {
		if (zPercent >= xPercent)
			bottomRight = bottomLeft + (topRight - topLeft);
		else
			topLeft = topRight + (bottomLeft - bottomRight);
	}
	else {
		if (zPercent < (1.0f - xPercent))
			topRight = topLeft + (bottomRight - bottomLeft);
		else
			bottomLeft = topLeft + (bottomRight - topRight);
	}

	float topHeight = topLeft * (1 - xPercent) + topRight * xPercent;
	float bottomHeight = bottomLeft * (1 - xPercent) + bottomRight * xPercent;
	float height = bottomHeight * (1 - zPercent) + topHeight * zPercent;
	return height;
}

XMVECTOR HeightMap::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= length))
		return(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	int index{ x + (z*width) };
	int xAdd{ (x < (width - 1)) ? 1 : -1 };
	int zAdd{ (z < (length - 1)) ? width : -(signed)width };

	float y1 = (float)imageData[index] * scale.y;
	float y2 = (float)imageData[index + xAdd] * scale.y;
	float y3 = (float)imageData[index + zAdd] * scale.y;

	XMVECTOR edge1 = XMVectorSet(0.0f, y3 - y1, scale.z, 0.0f);
	XMVECTOR edge2 = XMVectorSet(scale.x, y2 - y1, scale.x, 0.0f);
	XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

	return normal;
}

HeightMapTerrain::HeightMapTerrain(ID3D11Device * device, LPCTSTR fileName, int width, int length, int blockWidth, int blockLength, FXMVECTOR scale, FXMVECTOR color) : CGameObject(0), width{ width }, length{ length }
{
	XMStoreFloat3A(&this->scale, scale);

	int xPerBlock = blockWidth - 1;
	int zPerBlock = blockLength - 1;

	heightMap = new HeightMap{ fileName, width, length, scale };

	int xBlockCount = (width - 1) / xPerBlock;
	int zBlockCount = (length - 1) / zPerBlock;

	meshCount = xBlockCount*zBlockCount;
	meshes = new CMesh*[meshCount];
	ZeroMemory(meshes, sizeof(CMesh*)*meshCount);

	HeightMapGridMesh* gridMesh{ nullptr };
	for (int z = 0, zStart = 0; z < zBlockCount; ++z) {
		for (int x = 0, xStart = 0; x < xBlockCount; ++x) {
			xStart = x * (blockWidth - 1);
			zStart = z * (blockLength - 1);
			gridMesh = new HeightMapGridMesh(device, D3D11_FILL_SOLID, xStart, zStart,
				blockWidth, blockLength, scale, color, heightMap);
			SetMesh(gridMesh, x + (z *  xBlockCount));
		}
	}
}

float HeightMapTerrain::GetPeakHeight()
{
	BoundingBox box;
	GetAABB(box);
	XMFLOAT3 corner[BoundingBox::CORNER_COUNT];
	box.GetCorners(corner);

	float r;
	r = corner[0].y;
	for (int i = 1; i < 8; ++i)
	{
		if (r < corner[i].y) r = corner[i].y;
	}
	return r;
}

RollingObject::RollingObject(ID3D11Device * device, FXMVECTOR color, float radius) : speed{ 20.f }
{
	SphereMesh* mesh{ new SphereMesh{device, D3D11_FILL_SOLID, color, radius, 20, 20} };
	SetMesh(mesh);
}

void RollingObject::Animate(float deltaTime)
{
}
