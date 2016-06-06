#pragma once

class CShader
{
public:

	CShader();
	virtual ~CShader();

	void AddRef() { ++reference; }
	void Release() { if (--reference <= 0) delete this; }

	void CreateVertexShaderFromFile(ID3D11Device* device, TCHAR* fileName, LPCSTR shaderName, LPCSTR shaderModel, ID3D11VertexShader** vertexShader, D3D11_INPUT_ELEMENT_DESC* inputLayout, UINT elementCnt, ID3D11InputLayout** vertexLayout);

	void CreatePixelShaderFromFile(ID3D11Device* device, TCHAR* fileName, LPCSTR shaderName, LPCSTR shaderModel, ID3D11PixelShader** pixelShader);

	virtual void CreateShader(ID3D11Device* device);

	virtual void Render(ID3D11DeviceContext* deviceContext);

	ID3D11VertexShader *vertexShader;
	ID3D11InputLayout *vertexLayout;
	ID3D11PixelShader *pixelShader;

private:
	int reference;
};


