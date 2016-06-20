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

void CPlayer::Move(float x, float y, float z)
{
	this->Move({ x,y,z });
}

void CPlayer::Move(D3DXVECTOR3 const & shift)
{
	auto pos = this->GetPosition();
	pos += shift;
	this->SetPosition(pos);
}

void CPlayer::Move(DWORD dir, float speed)
{
	if (dir)
	{
		D3DXVECTOR3 shift{ 0,0,0 };

		if (dir & FORWARD) shift.z += speed;
		if (dir & BACKWARD) shift.z -= speed;
			
		if (dir & RIGHT) shift.x += speed;
		if (dir & LEFT) shift.x -= speed;
			
		if (dir & UP) shift.y += speed;
		if (dir & DOWN) shift.y -= speed;

		Move(shift);
	}
}
