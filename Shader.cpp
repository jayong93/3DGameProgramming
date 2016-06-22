#include "stdafx.h"
#include "Shader.h"

BYTE* ReadCompiledEffectFile(LPCTSTR fileName, int* readBytes)
{
	FILE* file{ nullptr };
	_tfopen_s(&file, fileName, TEXT("rb"));
	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	BYTE* byteCode = new BYTE[fileSize];
	rewind(file);
	*readBytes = fread(byteCode, sizeof(BYTE), fileSize, file);
	fclose(file);

	return byteCode;
}

CShader::CShader() : vertexShader{ nullptr }, pixelShader{ nullptr }, vertexLayout{ nullptr }, reference{ 1 }
{
}

ID3D11Buffer* CShader::cbMtxWorld = nullptr;

CShader::~CShader()
{
	if (vertexShader) vertexShader->Release();
	if (pixelShader) pixelShader->Release();
	if (vertexLayout) vertexLayout->Release();
	ReleaseShaderVariables();
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

void CShader::CreateVertexShaderFromCompiledFile(ID3D11Device * device, TCHAR * fileName, D3D11_INPUT_ELEMENT_DESC * inputLayout, UINT elementCnt)
{
	int readBytes{ 0 };
	BYTE* byteCode = ReadCompiledEffectFile(fileName, &readBytes);
	HRESULT r = device->CreateVertexShader(byteCode, readBytes, nullptr, &vertexShader);
	device->CreateInputLayout(inputLayout, elementCnt, byteCode, readBytes, &vertexLayout);

	if (byteCode) delete[] byteCode;
}

void CShader::CreatePixelShaderFromCompiledFile(ID3D11Device * device, TCHAR * fileName)
{
	int readBytes{ 0 };
	BYTE* byteCode = ReadCompiledEffectFile(fileName, &readBytes);
	HRESULT r = device->CreatePixelShader(byteCode, readBytes, nullptr, &pixelShader);

	if (byteCode) delete[] byteCode;
}

void CShader::CreateShader(ID3D11Device * device)
{
	D3D11_INPUT_ELEMENT_DESC inLayout[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT elementCnt = ARRAYSIZE(inLayout);
	CreateVertexShaderFromCompiledFile(device, TEXT("VS.fxo"), inLayout, elementCnt);
	CreatePixelShaderFromCompiledFile(device, TEXT("PS.fxo"));
}

void CShader::CreateShaderVariables(ID3D11Device * device)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device->CreateBuffer(&bd, nullptr, &cbMtxWorld);
}

void CShader::ReleaseShaderVariables()
{
	if (cbMtxWorld) cbMtxWorld->Release();
}

void CShader::UpdateShaderVariable(ID3D11DeviceContext * deviceContext, FXMMATRIX mWorld)
{
	D3D11_MAPPED_SUBRESOURCE mapRes;
	deviceContext->Map(cbMtxWorld, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes);
	VS_CB_WORLD_MATRIX* wMtx = (VS_CB_WORLD_MATRIX*)mapRes.pData;
	XMStoreFloat4x4A(&wMtx->mtxWorld, XMMatrixTranspose(mWorld));
	deviceContext->Unmap(cbMtxWorld, 0);

	deviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &cbMtxWorld);
}

void CShader::ReleaseObjects()
{
	for (auto& o : objList)
		o->Release();
}

void CShader::AnimateObjects(float timeElapsed)
{
	for (auto& o : objList)
		o->Animate(timeElapsed);
}

void CShader::OnPrepareRender(ID3D11DeviceContext * deviceContext)
{
	if (vertexLayout) deviceContext->IASetInputLayout(vertexLayout);
	if (vertexShader) deviceContext->VSSetShader(vertexShader, nullptr, 0);
	if (pixelShader) deviceContext->PSSetShader(pixelShader, nullptr, 0);
}

void CShader::Render(ID3D11DeviceContext * deviceContext, CCamera * camera)
{
	OnPrepareRender(deviceContext);

	for (auto& o : objList)
	{
		o->Render(deviceContext);
	}
}

void MinimapShader::CreateShader(ID3D11Device * device)
{
	D3D11_INPUT_ELEMENT_DESC inLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT elementCnt = ARRAYSIZE(inLayout);
	CreateVertexShaderFromCompiledFile(device, TEXT("Minimap_VS.fxo"), inLayout, elementCnt);
	CreatePixelShaderFromCompiledFile(device, TEXT("Minimap_PS.fxo"));
}

void MinimapShader::Render(ID3D11DeviceContext * deviceContext, CCamera * camera)
{
	if(camera)
		camera->UpdateShaderVariable(deviceContext);

	CShader::Render(deviceContext, camera);
}
