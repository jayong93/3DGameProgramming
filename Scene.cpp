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
	if (player)
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
		s->Render(deviceContext, camera);
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

void FirstScene::ReleaseObject()
{
	CScene::ReleaseObject();
	floor->Release();
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

		if (direction)
		{
			XMFLOAT3A pos;
			player->Move(direction, 5.f*elapsedTime);
			XMStoreFloat3A(&pos, player->GetPosition());
			if (abs(pos.x) > 50 || abs(pos.z) > 50)
				player->Move(direction, -5.f*elapsedTime);
		}
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

void SecondScene::BuildObject(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	CShader* shader{ new CShader };
	shader->CreateShader(device);
	shaderList.emplace_back(shader);

	// 미니맵 쉐이더
	CShader* minimapShader = new MinimapShader;
	minimapShader->CreateShader(device);
	shaderList.emplace_back(minimapShader);

	// 플레이어 생성
	player = new CPlayer;
	player->SetPosition({ 250.f, 150.f, -100.f });
	shader->objList.emplace_back(player);
	player->AddRef();

	// 카메라 생성
	CCamera* cam = new ThirdCam;
	cam->CreateShaderVariable(device);
	cam->SetViewport(deviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	cam->CreateProjectionMatrix(1.0, 1000.0f, FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT, 90.0f);
	cam->SetPlayer(player);
	cam->CreateViewMatrix();

	player->SetCamera(cam);
	player->CreateShaderVariables(device);

	// 지형 생성
	XMVECTOR scale = XMVectorSet(2.0f, 0.5f, 2.0f, 1.0f);
	XMVECTOR color = XMVectorSet(0.7f, 0.7f, 0.7f, 0.0f);
	terrain = new HeightMapTerrain(device, TEXT("HeightMap.raw"), 257, 257, 17, 17,
		scale, color);
	shader->objList.emplace_back(terrain);
	minimapShader->objList.emplace_back(terrain);
	terrain->AddRef();
	terrain->AddRef();

	// 구 생성
	RollingObject* obj;
	XMVECTOR sphereColor = XMVectorSet(1.f, 0.7f, 0.7f, 1.f);
	for (int i = 0; i < 100; ++i)
	{
		obj = new RollingObject{ device, sphereColor, 5.f };
		int x = RandomRangeFloat(5.f, 510.f), z = RandomRangeFloat(5.f, 510.f);
		int y = terrain->GetHeight(x, z) + 5.f;
		obj->SetPosition(XMVectorSet(x, y, z, 0));
		shader->objList.emplace_back(obj);
		minimapShader->objList.emplace_back(obj);
		objectList.emplace_back(obj);
		obj->AddRef();
		obj->AddRef();
	}

	// 미니맵을 위한 카메라
	minimapCam = new OrthoCam{ 514.f,514.f};
	minimapCam->CreateShaderVariable(device);
	minimapCam->SetViewport(deviceContext, FRAME_BUFFER_WIDTH - 200, FRAME_BUFFER_HEIGHT - 200, 200, 200);
	minimapCam->SetPosition(XMVectorSet(257.f, 100.f, 257.f, 0.f));
	minimapCam->SetLookAt(XMVectorSet(257.f, 0.f, 257.f, 0.f));
	minimapCam->CreateViewMatrix();
}

void SecondScene::ReleaseObject()
{
	CScene::ReleaseObject();
	if (terrain)
		terrain->Release();
	if (minimapCam)
		delete minimapCam;
}

bool SecondScene::ProcessInput(const InputData & inputData, float elapsedTime)
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
			{
				player->Rotate(XMVectorSet(0.f, XMConvertToRadians(cx), 0.f, 0.f));
				player->GetCamera()->Rotate(cy, cx, 0.f);
			}
		}

		if (direction)
		{
			player->Move(direction, 50.f*elapsedTime);
		}
		player->Update(elapsedTime);
	}
	return true;
}

void SecondScene::AnimateObject(float deltaTime)
{
	for (auto& s : shaderList)
	{
		s->AnimateObjects(deltaTime);
	}
	for (auto& o : objectList)
	{
		if (o == terrain) continue;
		XMFLOAT3A pos;
		XMStoreFloat3A(&pos, o->GetPosition());

		RollingObject* ro = (RollingObject*)o;
		if (pos.x < 5.f)
		{
			pos.x = 5.f;
			XMVECTOR dir = ro->GetDirection();
			ro->SetDirection(XMVector3Reflect(dir, XMVectorSet(1.f, 0.f, 0.f, 0.f)));
		}
		else if (pos.x > 509.f)
		{
			pos.x = 509.f;
			XMVECTOR dir = ro->GetDirection();
			ro->SetDirection(XMVector3Reflect(dir, XMVectorSet(-1.f, 0.f, 0.f, 0.f)));
		}
		if (pos.z < 5.f)
		{
			pos.z = 5.f;
			XMVECTOR dir = ro->GetDirection();
			ro->SetDirection(XMVector3Reflect(dir, XMVectorSet(0.f, 0.f, 1.f, 0.f)));
		}
		else if (pos.z > 509.f)
		{
			pos.z = 509.f;
			XMVECTOR dir = ro->GetDirection();
			ro->SetDirection(XMVector3Reflect(dir, XMVectorSet(0.f, 0.f, -1.f, 0.f)));
		}

		pos.y = terrain->GetHeight(pos.x, pos.z) + 5.f;
		o->SetPosition(XMLoadFloat3A(&pos));
	}
}

void SecondScene::Render(ID3D11DeviceContext * deviceContext, CCamera * camera)
{
	player->UpdateShaderVariables(deviceContext);
	player->GetCamera()->SetViewport(deviceContext);
	auto s = shaderList.front();
	s->Render(deviceContext, camera);

	minimapCam->SetViewport(deviceContext);
	s = shaderList.back();
	s->Render(deviceContext, minimapCam);
}
