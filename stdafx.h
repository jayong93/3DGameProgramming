// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
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