#pragma once
#include "Object.h"
#include "Camera.h"
#include "Player.h"

struct VS_CB_WORLD_MATRIX
{
	XMFLOAT4X4A mtxWorld;
};

class CShader
{
public:

	CShader();
	virtual ~CShader();

	void AddRef() { ++reference; }
	void Release() { if (--reference <= 0) delete this; }

	void CreateVertexShaderFromFile(ID3D11Device* device, TCHAR* fileName, LPCSTR shaderName, LPCSTR shaderModel, ID3D11VertexShader** vertexShader, D3D11_INPUT_ELEMENT_DESC* inputLayout, UINT elementCnt, ID3D11InputLayout** vertexLayout);
	void CreatePixelShaderFromFile(ID3D11Device* device, TCHAR* fileName, LPCSTR shaderName, LPCSTR shaderModel, ID3D11PixelShader** pixelShader);
	void CreateVertexShaderFromCompiledFile(ID3D11Device* device, TCHAR* fileName, D3D11_INPUT_ELEMENT_DESC* inputLayout, UINT elementCnt);
	void CreatePixelShaderFromCompiledFile(ID3D11Device* device, TCHAR* fileName);

	virtual void CreateShader(ID3D11Device* device);
	static void CreateShaderVariables(ID3D11Device* device);
	static void ReleaseShaderVariables();
	static void UpdateShaderVariable(ID3D11DeviceContext* deviceContext, FXMMATRIX mWorld);

	virtual void BuildObjects(ID3D11Device* device) {}
	virtual void ReleaseObjects();
	virtual void AnimateObjects(float timeElapsed);
	virtual void OnPrepareRender(ID3D11DeviceContext* deviceContext);
	virtual void Render(ID3D11DeviceContext *deviceContext, CCamera *camera = NULL);

	ID3D11VertexShader *vertexShader;
	ID3D11InputLayout *vertexLayout;
	ID3D11PixelShader *pixelShader;
	std::vector<CGameObject*> objList;
	
protected:
	int reference;

	static ID3D11Buffer* cbMtxWorld;
};

class MinimapShader : public CShader
{
public:
	virtual void CreateShader(ID3D11Device* device);
	virtual void Render(ID3D11DeviceContext *deviceContext, CCamera *camera = NULL);
};
