// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <D3DX10math.h>
#include <chrono>
#include <stdio.h>
#include <d3d9types.h>

const int FRAME_BUFFER_WIDTH{ 640 };
const int FRAME_BUFFER_HEIGHT{ 480 };

const int VS_SLOT_CAMERA{ 0x00 };
const int VS_SLOT_WORLD_MATRIX{ 0x01 };

const int PS_SLOT_COLOR{ 0x00 };

inline float RandomNormalizeFloat() { return (float)(rand() / (float)RAND_MAX); }
inline float RandomRangeFloat(float min, float max) { return (RandomNormalizeFloat()*(max - min) + min); }