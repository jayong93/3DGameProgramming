#pragma once
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	bool OnMouseEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	bool OnKeyEvent(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	void BuildObject(ID3D11Device* device);
	void ReleaseObject();

	bool ProcessInput();
	void AnimateObject(float elapsedTime);
	void Render(ID3D11DeviceContext* deviceContext, CCamera* camera);

private:
	std::vector<CShader*> shaderList;
	std::vector<CGameObject*> objectList;
};

