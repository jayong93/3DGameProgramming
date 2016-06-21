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

	void BuildObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void ReleaseObject();

	bool ProcessInput(const InputData& inputData, float elapsedTime);
	void AnimateObject(float elapsedTime);
	void Render(ID3D11DeviceContext* deviceContext, CCamera* camera);

	CPlayer* GetPlayer() const { return player; }
private:
	std::vector<CShader*> shaderList;
	std::vector<CGameObject*> objectList;
	CPlayer* player;
};

