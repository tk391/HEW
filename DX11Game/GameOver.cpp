#include"GameOver.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
//#include "Sound.h"

// マクロ定義
#define BG_POS_X		0.0f
#define BG_POS_Y		0.0f
#define BG_WIDTH		SCREEN_WIDTH
#define BG_HEIGHT		SCREEN_HEIGHT

// グローバル変数
static LPCWSTR g_pszTexFName = L"data/texture/prottype/NULL";
static ID3D11ShaderResourceView* g_pTexture;

// 初期化
HRESULT GameOver::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// テクスチャ読込

	hr = CreateTextureFromFile(pDevice, g_pszTexFName, &g_pTexture);
	if (FAILED(hr)) {
		return hr;
	}

	// BGM再生
	//CSound::Play(BGM_GameOver);

	return hr;
}

// 終了処理
void GameOver::Uninit()
{
	// BGM再生停止
	//CSound::Stop(BGM_GameOver);

	// テクスチャ解放
	SAFE_RELEASE(g_pTexture);

}

// 更新
void GameOver::Update()
{
	// [Enter]またはスペースキー押下
	if (GetKeyRelease(VK_RETURN) || GetKeyRelease(VK_SPACE) || GetJoyTrigger(0, PS4CIRCLE)) {
		StartFadeOut(SCENE_RESULT);
		return;
	}
}

// 描画
void GameOver::Draw()
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	ID3D11DeviceContext* pDC = GetDeviceContext();

	SetPolygonSize(BG_WIDTH, BG_HEIGHT);
	SetPolygonPos(BG_POS_X, BG_POS_Y);
	SetPolygonTexture(g_pTexture);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);
}