#pragma once
#include "Object.h"
#include <vector>

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

	bool CheckRayCast(ID3D11Device* device, D3DXVECTOR3 const& rayStart, D3DXVECTOR3 const& rayDir);

private:
	std::vector<CGameObject*> objectList;
};

