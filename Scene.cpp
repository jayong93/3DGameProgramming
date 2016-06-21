#include "stdafx.h"
#include "Scene.h"


CScene::CScene()
{
}


CScene::~CScene()
{
}

bool CScene::OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnKeyEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObject(ID3D11Device * device, ID3D11DeviceContext* deviceContext)
{
	
}

void CScene::ReleaseObject()
{
	for (auto& s : shaderList)
	{
		s->ReleaseObjects();
		s->Release();
	}
	for (auto& o : objectList)
		o->Release();
	player->Release();
}

bool CScene::ProcessInput(const InputData& inputData, float elapsedTime)
{
	return false;
}

void CScene::AnimateObject(float elapsedTime)
{
	for (auto& s : shaderList)
	{
		s->AnimateObjects(elapsedTime);
	}
}

void CScene::Render(ID3D11DeviceContext * deviceContext, CCamera* camera)
{
	player->UpdateShaderVariables(deviceContext);
	for (auto& s : shaderList)
		s->Render(deviceContext);
}

void FirstScene::BuildObject(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	CShader* shader{ new CShader };
	shader->CreateShader(device);
	shaderList.emplace_back(shader);

	// 플레이어 생성
	player = new CPlayer;
	XMVECTOR playerColor = XMVectorSet(0.4f, 0.4f, 1.f, 1.f);
	CMesh* playerMesh = new CCubeMesh{ device,D3D11_FILL_SOLID, playerColor, 0.5f,0.5f,0.5f };
	player->SetMesh(playerMesh);
	player->SetPosition({ 0.f, 1.25f, 0.f });
	shader->objList.emplace_back(player);
	player->AddRef();

	// 카메라 생성
	CCamera* cam = new ThirdCam;
	cam->CreateShaderVariable(device);
	cam->SetViewport(deviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	cam->CreateProjectionMatrix(1.0, 500.0f, FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT, 90.0f);
	cam->SetPlayer(player);
	cam->CreateViewMatrix();

	player->SetCamera(cam);
	player->CreateShaderVariables(device);

	// 적 그리기
	XMVECTOR cubeColor = XMVectorSet(1.f, 0.4f, 0.4f, 1.f);
	CCubeMesh* mesh{ new CCubeMesh{ device, D3D11_FILL_SOLID, cubeColor, 0.5f, 0.5f, 0.5f } };
	for (int i = 0; i < 80; i++)
	{
		ChasingObject* obj{ new ChasingObject{ player } };
		obj->SetMesh(mesh);
		obj->SetPosition({ RandomRangeFloat(-49.f,49.f),1.25f,RandomRangeFloat(-49.f,49.f) });
		shader->objList.emplace_back(obj);
		objectList.emplace_back(obj);
		obj->AddRef();
	}

	// 바닥 그리기
	XMVECTOR floorColor = XMVectorSet(0.5f, 0.5f, 0.5f, 1.f);
	mesh = new CCubeMesh{ device, D3D11_FILL_SOLID, floorColor, 100.f,2.f,100.f };
	floor = { new CGameObject };
	floor->SetMesh(mesh);
	shader->objList.emplace_back(floor);
	floor->AddRef();
}

bool FirstScene::ProcessInput(const InputData & inputData, float elapsedTime)
{
	DWORD direction{ 0 };
	if (inputData.keyBuffer['W'] & 0xf0) direction |= FORWARD;
	if (inputData.keyBuffer['S'] & 0xf0) direction |= BACKWARD;
	if (inputData.keyBuffer['A'] & 0xf0) direction |= LEFT;
	if (inputData.keyBuffer['D'] & 0xf0) direction |= RIGHT;

	float cx = 0.f, cy = 0.f;
	if (inputData.keyBuffer[VK_RBUTTON])
	{
		cx = (float)(inputData.cursorPos.x - inputData.oldCursorPos.x) / 3.f;
		cy = (float)(inputData.cursorPos.y - inputData.oldCursorPos.y) / 3.f;
	}

	if (direction != 0 || cx != 0.f || cy != 0.f)
	{
		if (cx || cy)
		{
			if (inputData.keyBuffer[VK_RBUTTON] & 0xf0)
				player->GetCamera()->Rotate(cy, cx, 0.f);
		}

		if (direction) player->Move(direction, 5.f*elapsedTime);
		player->Update(elapsedTime);
	}

	// 폭발
	if (inputData.keyBuffer[VK_SPACE] & 0xf0)
	{
		XMFLOAT3 playerPos;
		XMStoreFloat3(&playerPos, player->GetPosition());
		BoundingSphere blowSphere{ playerPos, 5.f };

		for (auto it = objectList.begin(); it != objectList.end();)
		{
			BoundingSphere objBounding;
			(*it)->GetBoundingSphere(objBounding);
			if (blowSphere.Contains(objBounding) != DISJOINT)
			{
				// 쉐이더의 객체 배열에서 삭제
				for (auto& s : shaderList)
				{
					auto f = std::find(s->objList.begin(), s->objList.end(), *it);
					if (f != s->objList.end())
					{
						(*f)->Release();
						s->objList.erase(f);
					}
				}
				// 오브젝트 객체 배열에서 삭제
				(*it)->Release();
				it = objectList.erase(it);
			}
			else
				++it;
		}
	}
	return true;
}
