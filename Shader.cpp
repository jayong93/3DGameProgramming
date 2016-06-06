#include "stdafx.h"
#include "Shader.h"


CShader::CShader() : vertexShader{ nullptr }, pixelShader{ nullptr }, vertexLayout{ nullptr }, reference{ 0 }
{
}


CShader::~CShader()
{
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();
	if (vertexLayout) vertexLayout->Release();
}

void CShader::CreateVertexShaderFromFile(ID3D11Device * device, TCHAR * fileName, LPCSTR shaderName, LPCSTR shaderModel, ID3D11VertexShader ** vertexShader, D3D11_INPUT_ELEMENT_DESC * inputLayout, UINT elementCnt, ID3D11InputLayout ** vertexLayout)
{
	HRESULT r;
	DWORD sFlag{ D3DCOMPILE_ENABLE_STRICTNESS };

	ID3DBlob* sBlob{ nullptr }, *errorBlob{ nullptr };

	if (SUCCEEDED(r = D3DX11CompileFromFile(fileName, nullptr, nullptr, shaderName, shaderModel, sFlag, 0, nullptr, &sBlob, &errorBlob, nullptr)))
	{
		device->CreateVertexShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, vertexShader);
		device->CreateInputLayout(inputLayout, elementCnt, sBlob->GetBufferPointer(), sBlob->GetBufferSize(), vertexLayout);
		sBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device * device, TCHAR * fileName, LPCSTR shaderName, LPCSTR shaderModel, ID3D11PixelShader ** pixelShader)
{
	HRESULT r;

	DWORD sFlag{ D3DCOMPILE_ENABLE_STRICTNESS };

	ID3DBlob* sBlob{ nullptr }, *errorBlob{ nullptr };

	if (SUCCEEDED(r = D3DX11CompileFromFile(fileName, nullptr, nullptr, shaderName, shaderModel, sFlag, 0, nullptr, &sBlob, &errorBlob, nullptr)))
	{
		device->CreatePixelShader(sBlob->GetBufferPointer(), sBlob->GetBufferSize(), nullptr, pixelShader);
		sBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device * device)
{
	D3D11_INPUT_ELEMENT_DESC inLayout[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT elementCnt = ARRAYSIZE(inLayout);
	CreateVertexShaderFromFile(device, TEXT("Effect.fx"), "VS", "vs_5_0", &vertexShader, inLayout, elementCnt, &vertexLayout);
	CreatePixelShaderFromFile(device, TEXT("Effect.fx"), "PS", "ps_5_0", &pixelShader);
}

void CShader::Render(ID3D11DeviceContext * deviceContext)
{
	if (vertexLayout) deviceContext->IASetInputLayout(vertexLayout);
	if (vertexShader) deviceContext->VSSetShader(vertexShader, nullptr, 0);
	if (pixelShader) deviceContext->PSSetShader(pixelShader, nullptr, 0);
}
