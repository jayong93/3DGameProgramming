#pragma once
#include "Shader.h"

struct InputData
{
	UCHAR keyBuffer[256];
	POINT cursorPos;
	POINT oldCursorPos;
};

class CScene
{
public:
	CScene();
	~CScene();

	bool OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	bool OnKeyEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	virtual void BuildObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual void ReleaseObject();

	virtual bool ProcessInput(const InputData& inputData, float elapsedTime);
	virtual void AnimateObject(float elapsedTime);
	virtual void Render(ID3D11DeviceContext* deviceContext, CCamera* camera);

	CPlayer* GetPlayer() const { return player; }
protected:
	std::vector<CShader*> shaderList;
	std::vector<CGameObject*> objectList;
	CPlayer* player;
};

class FirstScene : public CScene
{
public:
	virtual void BuildObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual void ReleaseObject();
	virtual bool ProcessInput(const InputData& inputData, float elapsedTime);

private:
	CGameObject* floor{ nullptr };
};

class SecondScene : public CScene
{
public:
	virtual void BuildObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual void ReleaseObject();
	virtual bool ProcessInput(const InputData& inputData, float elapsedTime);
	virtual void AnimateObject(float deltaTime);
	virtual void Render(ID3D11DeviceContext* deviceContext, CCamera* camera);

private:
	HeightMapTerrain* terrain{ nullptr };
	OrthoCam* minimapCam{ nullptr };
};