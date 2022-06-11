//=============================================================================
//酸素[Life.cpp]
//12/11 齋藤成希
//=============================================================================
#include "Life.h"
#include"Texture.h"
#include"polygon.h"
#include "debugproc.h"
#include <math.h>

//マクロ
#define MAX_TEXTURE		(2)		//テクスチャ数

#define PATH_LIFEGAGE_TEXTURE	 L"data/texture/prottype_white.png"	//酸素ゲージ
#define PATH_CYLINDER_TEXTURE	 L"data/texture/bombe.png"				//酸素ボンベ（細いものに変えてほしい）

#define CENTER_POS_X	(-500.0f)	//中心座標X
#define CENTER_POS_Y	(-150.0f)	//中心座標Y

#define UNDER_POS	(-270.0f)		//底の位置

#define MAX_SIZE		(200.0f)		//酸素ゲージの最大サイズ

//グローバル変数
static XMFLOAT2 g_vPos[MAX_TEXTURE];
static XMFLOAT2 g_vSize[MAX_TEXTURE];
static XMFLOAT4 g_vColor[MAX_TEXTURE];
static ID3D11ShaderResourceView* g_pTexture[MAX_TEXTURE];

//振動グローバル変数
//01/10  佐藤大地
static int vibX;
static int vibY;
static bool BeatFlg = true;
static float vibSize;
static bool	DanceFlg = false;
//ここまで
//=============================================================================
//初期化
//=============================================================================
HRESULT Life::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ読み込み
	hr = CreateTextureFromFile(pDevice, PATH_LIFEGAGE_TEXTURE, &g_pTexture[0]);
	hr = CreateTextureFromFile(pDevice, PATH_CYLINDER_TEXTURE, &g_pTexture[1]);
	if (FAILED(hr))
		return hr;

	//メンバ変数の初期化
	g_vPos[0] = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y - 25.0f);
	g_vSize[0] = XMFLOAT2(30.0f, 200.0f);
	g_vColor[0] = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	g_vPos[1] = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y);
	g_vSize[1] = XMFLOAT2(100.0f, 300.0f);
	g_vColor[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//振動変数の初期化
	//01/11  佐藤大地
	vibX = 0;
	vibY = 0;
	vibSize = 0.0f;
	return hr;
}

//=============================================================================
//終了
//=============================================================================
void Life::Uninit()
{
	for (int i = 0; i < MAX_TEXTURE; ++i) {
		//背景テクスチャ解放
		SAFE_RELEASE(g_pTexture[i]);
	}
}

//=============================================================================
//更新
//=============================================================================
void Life::Update()
{
#if _DEBUG
	//今のところは特になし
	PrintDebugProc("x,y={%.2f,%.2f}\n", g_vSize[1].x, g_vSize[1].y);
#endif
}

//=============================================================================
//描画
//=============================================================================
void Life::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	//酸素ゲージ
	SetPolygonSize(g_vSize[0].x + 25.0f, g_vSize[0].y);

	SetPolygonPos(g_vPos[0].x, g_vPos[0].y);
	//振動を追加
	//01/10  佐藤大地
	SetPolygonPos(g_vPos[0].x + vibX, g_vPos[0].y + vibY);
	SetPolygonTexture(g_pTexture[0]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(g_vColor[0].x, g_vColor[0].y, g_vColor[0].z);
	DrawPolygon(pDC);

	//酸素ボンベ
	SetPolygonSize(g_vSize[1].x, g_vSize[1].y);
	//鼓動を追加
	//01/11  佐藤大地
	SetPolygonSize(g_vSize[1].x + vibSize, g_vSize[1].y + vibSize);
	SetPolygonPos(g_vPos[1].x, g_vPos[1].y);
	//振動を追加
	//01/10  佐藤大地
	SetPolygonPos(g_vPos[1].x + vibX, g_vPos[1].y + vibY);
	SetPolygonTexture(g_pTexture[1]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(g_vColor[1].x, g_vColor[1].y, g_vColor[1].z);
	DrawPolygon(pDC);
	//元に戻す
	SetPolygonColor(1.0f, 1.0f, 1.0f);
}

//=============================================================================
//プレイヤーのライフ状態によって大きさを動かす
//=============================================================================
void Life::Set(float fLife)
{
	if (fLife < 0.0f) {
		g_vSize[0].x = 0.0f;
		return;
	}
	g_vSize[0].y = fLife * 1.9f;
	g_vPos[0].y = UNDER_POS + (g_vSize[0].y / 2.0f);
#if _DEBUG
	PrintDebugProc("fLife=%.2f\n", fLife);
#endif
	//緑→赤ver	01/18 三輪卓弘
	if (fLife >= 50.0f) {
		g_vColor[0] = XMFLOAT4(1.0f - ((fLife / 100) - (1.0f - fLife / 100)), 1.0f, 0.0f, 1.0f);
	}
	else {
		g_vColor[0] = XMFLOAT4(1.0f, fLife / 100 + fLife / 100, 0.0f, 1.0f);
	}
}

//パターン1;振動
//01/10 佐藤大地
void Life::Vib(float fLife)
{
	if (fLife >= 0)
	{
		if (fLife <= 50.0f && fLife > 25.0f)
		{
			vibX = (rand() % 4);
			vibY = (rand() % 1);
		}

		if (fLife <= 25.0f)
		{
			vibX = (rand() % 6);
			vibY = (rand() % 2);
		}
	}
	else
	{
		vibX = 0;
		vibY = 0;
	}
}

//パターン2:鼓動
//01/10  佐藤大地
void Life::Beat(float fLife)
{
	if (fLife > 0)
	{
		if (fLife <= 50.0f && fLife > 25.0f && BeatFlg)
		{
			if (vibSize <= 9.5f)
			{
				vibSize += 0.4f;
			}
			else
			{
				BeatFlg = false;
			}
		}
		else if (fLife <= 50.0f && fLife > 25.0f && !BeatFlg)
		{
			if (vibSize >= -0.5f)
			{
				vibSize -= 0.4f;
			}
			else
			{
				BeatFlg = true;
			}
		}

		if (fLife <= 25.0f && BeatFlg)
		{
			if (vibSize <= 12.5f)
			{
				vibSize += 0.6f;
			}
			else
			{
				BeatFlg = false;
			}
		}
		else if (fLife <= 25.0f && !BeatFlg)
		{
			if (vibSize >= -0.5f)
			{
				vibSize -= 0.6f;
			}
			else
			{
				BeatFlg = true;
			}
		}
	}
	else
	{
		vibSize = 0.0f;
	}
	// 酸素ボンベの膨らみに合わせて赤っぽくなる処理 不要なら消して可 01/18 三輪卓弘
	g_vColor[1] = XMFLOAT4(1.0f, 1.0f - vibSize / 40, 1.0f - vibSize / 40, 1.0f);
}

void Life::Dance(float fLife)
{
	if (fLife >= 0)
	{
		if (fLife <= 50.0f)
		{
			DanceFlg = true;
		}
	}
	else
	{
		DanceFlg = false;
	}
}