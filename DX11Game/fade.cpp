//=============================================================================
// フェードイン/アウト処理 [fade.cpp]
//12/11 齋藤成希
//=============================================================================
#include "fade.h"
#include "polygon.h"
//#include "Sound.h"

// マクロ定義
#define FADE_RATE	0.02f	// フェードイン/アウトの刻み

// グローバル変数
static float g_fRed = 0.0f;		// フェードアウト色
static float g_fGreen = 0.0f;
static float g_fBlue = 0.0f;
static float g_fAlpha;	// 不透明度
static EFade g_eFade;	// 状態
static EScene g_eNext;	// 次のシーン

//=============================================================================
// 初期化
//=============================================================================
HRESULT InitFade()
{
	g_eFade = FADE_IN;
	g_fAlpha = 1.0f;
	g_eNext = SCENE_TITLE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade()
{
	g_eFade = FADE_NONE;
	g_fAlpha = 0.0f;
}

//=============================================================================
// 更新
//=============================================================================
void UpdateFade()
{
	switch (g_eFade) {
	case FADE_NONE:
		break;
	case FADE_OUT:
		g_fAlpha += FADE_RATE;	// 不透明度を増す
		if (g_fAlpha >= 1.0f) {
			// フェードイン処理に切り替え
			g_fAlpha = 1.0f;
			g_eFade = FADE_IN;
			// シーン切替
			SetScene(g_eNext);
		}
		// ボリュームもフェードアウト
		//CSound::SetVolume(1.0f - g_fAlpha);
		break;
	case FADE_IN:
		g_fAlpha -= FADE_RATE;	// 透明度を増す
		if (g_fAlpha <= 0.0f) {
			// フェードインを終了する
			g_fAlpha = 0.0f;

			g_eFade = FADE_NONE;
		}
		// ボリュームもフェードイン
		//CSound::SetVolume(1.0f - g_fAlpha);
		break;
	}
}

//=============================================================================
// 描画
//=============================================================================
void DrawFade()
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	SetBlendState(BS_ALPHABLEND);//αブレンド有効
	// 画面全体に半透明の矩形を描画
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonTexture(nullptr);
	SetPolygonColor(g_fRed, g_fGreen, g_fBlue);
	SetPolygonAlpha(g_fAlpha);
	DrawPolygon(GetDeviceContext());
	// 元に戻す
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonAlpha(1.0f);

	SetBlendState(BS_NONE);//αブレンド無効
	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);
}

//=============================================================================
// フェードアウト開始
//=============================================================================
void StartFadeOut(EScene eNext)
{
	if (g_eFade != FADE_OUT) {
		g_eFade = FADE_OUT;
		g_fAlpha = 0.0f;
		g_eNext = eNext;
	}
}

//=============================================================================
// 状態取得
//=============================================================================
EFade GetFade()
{
	return g_eFade;
}

//=============================================================================
// フェードアウト色設定
//=============================================================================
void SetFadeColor(float fR, float fG, float fB)
{
	g_fRed = fR;
	g_fGreen = fG;
	g_fBlue = fB;
}
