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

void CPlayer::Move(ULONG direction, float dist, bool velocity)
{
	if (direction)
	{
		D3DXVECTOR3 shift{ 0,0,0 };

		if (direction & FORWARD) shift += look * dist;
		if (direction & BACKWARD) shift -= look * dist;

		if (direction & RIGHT) shift += right*dist;
		if (direction & LEFT) shift -= right * dist;

		if (direction & UP) shift += up * dist;
		if (direction & DOWN) shift -= up * dist;

		Move(shift, velocity);
	}
}

void CPlayer::Move(const D3DXVECTOR3 & shift, bool velocity)
{
	if (velocity)
	{
		this->velocity += shift;
	}
	else
	{
		position += shift;
		camera->Move(shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	D3DXMATRIX rm;
	CameraMode mode = camera->GetMode();

	if (mode == CameraMode::FIRST_PERSON || mode == CameraMode::THIRD_PERSON)
	{
		if (x != 0.f)
		{
			pitch += x;
			if (pitch > +89.f) { x -= (pitch - 89.f); pitch = +89.f; }
			if (pitch < -89.f) { x -= (pitch + 89.f); pitch = -89.f; }
		}
		if (y != 0.f)
		{
			yaw += y;
			if (yaw > 360.f) yaw -= 360.f;
			if (yaw < 0.f) yaw += 360.f;
		}
		if (z != 0.f)
		{
			roll += z;
			if (roll > +20.f) { z -= (roll - 20.f); roll = +20.f; }
			if (roll < -20.f) { z -= (roll + 20.f); roll = -20.f; }
		}

		camera->Rotate(x, y, z);

		if (y != 0.f)
		{
			D3DXMatrixRotationAxis(&rm, &up, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&look, &look, &rm);
			D3DXVec3TransformNormal(&right, &right, &rm);
		}
	}
	else if (mode == CameraMode::SPACESHIP)
	{
		camera->Rotate(x, y, z);
		if (x != 0.f)
		{
			D3DXMatrixRotationAxis(&rm, &right, (float)D3DXToRadian(x));
			D3DXVec3TransformNormal(&look, &look, &rm);
			D3DXVec3TransformNormal(&up, &up, &rm);
		}
		if (y != 0.f)
		{
			D3DXMatrixRotationAxis(&rm, &up, (float)D3DXToRadian(y));
			D3DXVec3TransformNormal(&look, &look, &rm);
			D3DXVec3TransformNormal(&right, &right, &rm);
		}
		if (z != 0.f)
		{
			D3DXMatrixRotationAxis(&rm, &look, (float)D3DXToRadian(z));
			D3DXVec3TransformNormal(&up, &up, &rm);
			D3DXVec3TransformNormal(&right, &right, &rm);
		}
	}

	D3DXVec3Normalize(&look, &look);
	D3DXVec3Cross(&right, &up, &look);
	D3DXVec3Normalize(&right, &right);
	D3DXVec3Cross(&up, &look, &right);
	D3DXVec3Normalize(&up, &up);
}

void CPlayer::Update(float timeElapsed)
{
	velocity += gravity * timeElapsed;

	float length = sqrtf(velocity.x*velocity.x + velocity.z*velocity.z);
	float maxXZ = maxVelocityXZ*timeElapsed;
	if (length > maxXZ)
	{
		velocity.x *= (maxXZ / length);
		velocity.z *= (maxXZ / length);
	}

	length = sqrtf(velocity.y*velocity.y);
	float maxY = maxVelocityY * timeElapsed;
	if (length > maxY) velocity.y *= (maxY / length);

	Move(velocity, false);

	if (playerUpdatedContext) OnPlayerUpdated(timeElapsed);

	CameraMode mode = camera->GetMode();

	if (mode == CameraMode::THIRD_PERSON)
		camera->Update(position, timeElapsed);
	camera->RecreateViewMatrix();

	//TODO: ¸¶Âû·Â ±¸Çö
}

void CPlayer::OnPlayerUpdated(float timeElapsed)
{
}

void CPlayer::OnCameraUpdated(float timeElapsed)
{
}

CCamera * CPlayer::OnChangeCamera(ID3D11Device * device, CameraMode newMode, CameraMode currentMode)
{
	CCamera* newCam = nullptr;

	switch (newMode)
	{
	case CameraMode::FIRST_PERSON:
		newCam = new CFirstPersonCamera(camera);
		break;
	case CameraMode::THIRD_PERSON:
		newCam = new CThirdPersonCamera(camera);
		break;
	case CameraMode::SPACESHIP:
		newCam = new CSpaceShipCamera(camera);
		break;
	}

	if (currentMode == CameraMode::SPACESHIP)
	{
		up = D3DXVECTOR3{ 0.f,1.f,0.f };
		right.y = 0.f;
		look.y = 0.f;
		D3DXVec3Normalize(&right, &right);
		D3DXVec3Normalize(&look, &look);
		pitch = 0.f;
		roll = 0.f;
		yaw = (float)D3DXToDegree(acosf(D3DXVec3Dot(&D3DXVECTOR3{ 0.f,0.f,1.f }, &look)));
		if (look.y < 0.f) yaw *= -1;
	}
	else if (newMode == CameraMode::SPACESHIP && camera)
	{
		right = camera->GetRightVector();
		up = camera->GetUpVector();
		look = camera->GetLookVector();
	}

	if (newCam)
	{
		if (!camera) newCam->CreateShaderVariable(device);
		newCam->SetMode(newMode);
		newCam->SetPlayer(this);
	}
	if (camera) delete camera;

	return newCam;
}

void CPlayer::ChangeCamera(ID3D11Device * device, CameraMode newMode, float timeElapsed)
{
	
}

void CPlayer::OnPrepareRender()
{
	mtxWorld._11 = right.x;
	mtxWorld._12 = right.y;
	mtxWorld._13 = right.z;
	mtxWorld._21 = up.x;
	mtxWorld._22 = up.y;
	mtxWorld._23 = up.z;
	mtxWorld._31 = look.x;
	mtxWorld._32 = look.y;
	mtxWorld._33 = look.z;
	mtxWorld._41 = position.x;
	mtxWorld._42 = position.y;
	mtxWorld._43 = position.z;
}

void CPlayer::Render(ID3D11DeviceContext * deviceContext)
{
	CameraMode mode = camera ? camera->GetMode() : CameraMode::NONE;
	if (mode == CameraMode::THIRD_PERSON)
	{
		CGameObject::Render(deviceContext);
	}
}

CCubePlayer::CCubePlayer(ID3D11Device * device)
{
}

void CCubePlayer::ChangeCamera(ID3D11Device * device, CameraMode newMode, float timeElapsed)
{
	CameraMode mode = camera ? camera->GetMode() : CameraMode::NONE;
	if (mode == newMode) return;
	switch (newMode)
	{
	case CameraMode::FIRST_PERSON:
		SetFriction(200.f);
		SetGravity(D3DXVECTOR3{ 0.f,0.f,0.f });
		SetMaxVelocityXZ(125.f);
		SetMaxVelocityY(400.f);
		camera = OnChangeCamera(device, newMode, mode);
		camera->SetTimeLag(0.f);
		camera->SetOffset({ 0.f,20.f,0.f });
		camera->CreateProjectionMatrix(1.0f, 5000.f, ASPECT_RATIO, 60.f);
		break;
	case CameraMode::SPACESHIP:
		SetFriction(125.f);
		SetGravity(D3DXVECTOR3{ 0.f,0.f,0.f });
		SetMaxVelocityXZ(400.f);
		SetMaxVelocityY(400.f);
		camera = OnChangeCamera(device, newMode, mode);
		camera->SetTimeLag(0.f);
		camera->SetOffset({ 0.f,0.f,0.f });
		camera->CreateProjectionMatrix(1.0f, 5000.f, ASPECT_RATIO, 60.f);
		break;
	case CameraMode::THIRD_PERSON:
		SetFriction(250.f);
		SetGravity(D3DXVECTOR3{ 0.f,0.f,0.f });
		SetMaxVelocityXZ(125.f);
		SetMaxVelocityY(400.f);
		camera = OnChangeCamera(device, newMode, mode);
		camera->SetTimeLag(0.25f);
		camera->SetOffset({ 0.f,20.f,-50.f });
		camera->CreateProjectionMatrix(1.0f, 5000.f, ASPECT_RATIO, 60.f);
		break;
	}
	Update(timeElapsed);
}

void CCubePlayer::OnPrepareRender()
{
}
