#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer() : camera{ nullptr }
{
}


CPlayer::~CPlayer()
{
	if (camera) delete camera;
}

void CPlayer::CreateShaderVariables(ID3D11Device * device)
{
}

void CPlayer::UpdateShaderVariables(ID3D11DeviceContext * deviceContext)
{
	if (camera) camera->UpdateShaderVariable(deviceContext);
}
