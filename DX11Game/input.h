//=============================================================================
//
// 入力処理 [input.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//キーボードのキー
const char VK_0 = '0';
const char VK_1 = '1';
const char VK_2 = '2';
const char VK_3 = '3';
const char VK_4 = '4';
const char VK_5 = '5';
const char VK_6 = '6';
const char VK_7 = '7';
const char VK_8 = '8';
const char VK_9 = '9';
const char VK_A = 'A';
const char VK_B = 'B';
const char VK_C = 'C';
const char VK_D = 'D';
const char VK_E = 'E';
const char VK_F = 'F';
const char VK_G = 'G';
const char VK_H = 'H';
const char VK_I = 'I';
const char VK_J = 'J';
const char VK_K = 'K';
const char VK_L = 'L';
const char VK_M = 'M';
const char VK_N = 'N';
const char VK_O = 'O';
const char VK_P = 'P';
const char VK_Q = 'Q';
const char VK_R = 'R';
const char VK_S = 'S';
const char VK_T = 'T';
const char VK_U = 'U';
const char VK_V = 'V';
const char VK_W = 'W';
const char VK_X = 'X';
const char VK_Y = 'Y';
const char VK_Z = 'Z';

//XBOXのボタン
enum XBOX {
	XBOXA = 0,
	XBOXB,
	XBOXX,
	XBOXY,
	XBOXL,
	XBOXR,
	XBOXSCREENSHOT,
	XBOXOPTION,
};

//PS4のボタン
enum PS4 {
	PS4SQUARE = 0,
	PS4CLOSS,
	PS4CIRCLE,
	PS4TRIANGLE,
	PS4L,
	PS4R,
	PS4ZL,
	PS4ZR,
	PS4SHARE,
	PS4OPTION,
	PS4L3,
	PS4R3
};

//Nintendo Switch プロコンのボタン
enum NSwitch {
	SWA=0,
	SWB,
	SWX,
	SWY,
	SWL,
	SWR,
	SWZL,
	SWZR,
	SWSTART,
	SWSELECT,
	SWPUSHL,
	SWPUSHR
};

//マウスのボタン
enum MouseButton {
	L=0,
	R,
	M,
	X1,
	X2
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitInput();
void UninitInput();
void UpdateInput();

bool GetKeyPress(int nKey);
bool GetKeyTrigger(int nKey);
bool GetKeyRelease(int nKey);
bool GetKeyRepeat(int nKey);

DWORD GetJoyCount();
JOYINFOEX *GetJoyState(DWORD dwJoy);
LONG GetJoyX(DWORD dwJoy);
LONG GetJoyY(DWORD dwJoy);
LONG GetJoyZ(DWORD dwJoy);
bool GetJoyButton(DWORD dwJoy, DWORD dwBtn);
bool GetJoyTrigger(DWORD dwJoy, DWORD dwBtn);
bool GetJoyRelease(DWORD dwJoy, DWORD dwBtn);

POINT *GetMousePosition();
bool GetMouseButton(DWORD dwBtn);
bool GetMouseTrigger(DWORD dwBtn);
bool GetMouseRelease(DWORD dwBtn);
