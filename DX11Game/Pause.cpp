//ポーズ機能の実装
//12/21  佐藤大地
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "polygon.h"
#include "Texture.h"
#include "sound.h"

// マクロ定義
#define	NUM_PAUSE_MENU		(3)			// ポーズメニュー数

#define	CONTINUE_WIDTH		(580.0f)	// コンティニュー幅
#define	CONTINUE_HEIGHT		(84.0f)		// コンティニュー高さ
#define	RETRY_WIDTH			(337.0f)	// リトライ幅
#define	RETRY_HEIGHT		(88.0f)		// リトライ高さ
#define	STAGESELECT_WIDTH	(586.0f)	// ステージセレクト幅
#define	STAGESELECT_HEIGHT	(75.0f)		// ステージセレクト高さ
#define	NAME_CORRECTION		* 0.6f		// 文字サイズ補正

#define	PAUSE_MENU_POS_X	(0.0f)		// ポーズメニュー位置(X座標)
#define	PAUSE_MENU_POS_Y	(PAUSE_MENU_INTERVAL)	// ポーズメニュー位置(Y座標)
#define	PAUSE_MENU_INTERVAL	(90.0f)	// ポーズメニュー間隔

#define	PLATE_WIDTH			(400.0f * 1.3f)	// プレートの幅
#define	PLATE_HEIGHT		(400.0f * 1.1f)	// プレートの幅
#define	PLATE_POS_X			(0.0f)		// プレートの位置(X座標)
#define	PLATE_POS_Y			(0.0f)		// プレートの位置(Y座標)

#define PLATE_TEXTURE		L"data/texture/pose_screen.png"
//ポーズ画面背景　　これは仮です

// グローバル変数
static long prevDW;//十字キーの1フレーム前の値
static ID3D11ShaderResourceView*	g_pTextures[3] = { nullptr };	// テクスチャへのポインタ
static ID3D11ShaderResourceView*	g_pPlateTexture;			//テクスチャ背景へのポインタ

static PAUSE_MENU g_nSelectMenu = PAUSE_MENU_CONTINUE;		// 選択中のメニューNo.
static float g_fCurve = 0.0f;
static float g_fCol = 0.0f;
static XMFLOAT2 g_vSize[NUM_PAUSE_MENU];

static LPCWSTR c_aFileNamePauseMenu[NUM_PAUSE_MENU] =
{
	L"data/texture/continue.png",	// コンティニュー
	L"data/texture/retry.png",	// リトライ
	L"data/texture/stageselect.png",	// クイット
	//仮の画像です
};


// 初期化処理
HRESULT InitPause(void)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	for (int nCntPauseMenu = 0; nCntPauseMenu < NUM_PAUSE_MENU; ++nCntPauseMenu)
	{
		// テクスチャの読み込み
		hr = CreateTextureFromFile(pDevice,									// デバイスへのポインタ
			c_aFileNamePauseMenu[nCntPauseMenu],	// ファイルの名前
			&g_pTextures[nCntPauseMenu]);			// 読み込むメモリー
	}

	hr = CreateTextureFromFile(pDevice, PLATE_TEXTURE, &g_pPlateTexture);

	g_nSelectMenu = PAUSE_MENU_CONTINUE;
	g_fCurve = 0.0f;
	g_vSize[0] = XMFLOAT2(CONTINUE_WIDTH NAME_CORRECTION, CONTINUE_HEIGHT NAME_CORRECTION);
	g_vSize[1] = XMFLOAT2(RETRY_WIDTH NAME_CORRECTION, RETRY_HEIGHT NAME_CORRECTION);
	g_vSize[2] = XMFLOAT2(STAGESELECT_WIDTH NAME_CORRECTION, STAGESELECT_HEIGHT NAME_CORRECTION);



	return hr;
}

// 終了処理
void UninitPause(void)
{
	// テクスチャの開放
	for (int nCntPauseMenu = 0; nCntPauseMenu < NUM_PAUSE_MENU; ++nCntPauseMenu)
	{
		SAFE_RELEASE(g_pTextures[nCntPauseMenu]);
	}
	SAFE_RELEASE(g_pPlateTexture);
}

// 更新処理
void UpdatePause(void)
{
	//十字キー操作
	static bool PushFlag = false;		//十字キーが押されたフレームのみ反応
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}

	if ((PushFlag&&joystate->dwPOV == 0))
	{

		CSound::Stop(SE_PAUSESELECT);
		CSound::Play(SE_PAUSESELECT);
		g_nSelectMenu = (PAUSE_MENU)((g_nSelectMenu + NUM_PAUSE_MENU - 1) % NUM_PAUSE_MENU);
		SetPauseMenu();
	}
	else if ((PushFlag&&joystate->dwPOV == 18000))
	{

		CSound::Stop(SE_PAUSESELECT);
		CSound::Play(SE_PAUSESELECT);
		g_nSelectMenu = (PAUSE_MENU)((g_nSelectMenu + 1) % NUM_PAUSE_MENU);
		SetPauseMenu();
	}
	prevDW = joystate->dwPOV;
}

// 描画処理
void DrawPause(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	SetPolygonTexture(g_pPlateTexture);
	SetPolygonSize(PLATE_WIDTH, PLATE_HEIGHT);
	SetPolygonPos(PLATE_POS_X, PLATE_POS_Y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonAlpha(1.0f);
	DrawPolygon(pDeviceContext);

	for (int nCntPauseMenu = 0; nCntPauseMenu < NUM_PAUSE_MENU; ++nCntPauseMenu)
	{
		SetPolygonSize(g_vSize[nCntPauseMenu]);
		SetPolygonPos(PAUSE_MENU_POS_X, PAUSE_MENU_POS_Y - nCntPauseMenu * PAUSE_MENU_INTERVAL);

		if (nCntPauseMenu == g_nSelectMenu)
		{
			SetPolygonColor(1.0f, 1.0f, 1.0f);
		}
		else
		{
			SetPolygonColor(0.3f, 0.3f, 0.3f);
		}
		// テクスチャの設定
		SetPolygonTexture(g_pTextures[nCntPauseMenu]);
		// ポリゴンの描画
		DrawPolygon(pDeviceContext);
	}

	SetPolygonColor(1.0f, 1.0f, 1.0f);
}

// ポーズメニューの設定
void SetPauseMenu(void)
{
	g_fCurve = 0.0f;
}

// ポーズメニューの取得
PAUSE_MENU GetPauseMenu(void)
{
	return g_nSelectMenu;
}

// ポーズメニューのリセット
void ResetPauseMenu(void)
{
	g_nSelectMenu = PAUSE_MENU_CONTINUE;
	SetPauseMenu();
}
//ここまで