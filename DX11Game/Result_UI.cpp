// リザルト画面 [ResultUI.cpp]
#include "Result_UI.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
#include "bg.h"
#include "game.h"
#include "Scene.h"
#include "Select.h"
#include "Sound.h"

// マクロ定義
#define	STAR_ALPHA		(0.01f)		// 星が灰色→黄色になる速度
#define STAR_INTERVAL	(0.005f)		// ↑のアニメーションが終わって、次の星のアニメーションが発生するまでの間隔
#define BUTTON_SIZE_X	(657.0f)
#define BUTTON_SIZE_Y	(192.0f)
#define HOSEI	(0.5f)
// 画像情報
static std::string fileName[]{
	"data/texture/stageName.png",
	"data/texture/stage_clear2.png",
	"data/texture/gameover.png",
	"data/texture/nextbotan2.png",
	"data/texture/retrybotan2.png",
	"data/texture/stageselectbotan2.png",
	"data/texture/starGrey.png",
	"data/texture/star.png",
	"data/texture/bombeMeter.png",
	"data/texture/prottype_white.png",
	"data/texture/bombeEmpty.png"
};

// 列挙型:画像情報		01/16 三輪卓弘
enum EResult {
	STAGE_NAME = 0,			// 画像:ステージ名(スプリッドシート)
	GAME_CLEAR,				// 画像:"ステージクリア"文字
	GAME_OVER,				// 画像:"ゲームオーバー"文字
	BUTTON_NEXT,			// ボタン:次のステージ
	BUTTON_RETRY,			// ボタン:リトライ
	BUTTON_SELECT,			// ボタン:ステージセレクト
	GRAYSTAR,				//灰色の星
	STAR,					//星
	CYLINDER,				//酸素ボンベ
	LIFEGAGE,				//酸素ゲージ
	EMPTY,					//酸素ボンベ　（空）
	MAX_RESULT				// リザルト画像総数
};

//星の情報
struct TGrayStar
{
	XMFLOAT3 pos;
	float Angle;
	float Apha;
};

// グローバル変数
const int max_star = 3;

static float StageNameY;
static long prevDW;//十字キーの1フレーム前の値
static int stnum;
static float Life;
static TGrayStar g_Star[max_star];
static float	StarCnt;	//星が色づき始めるまでの時間

// 初期化
HRESULT ResultUI::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	//テクスチャ読み込み
	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr))
			return hr;
	}

	// BGM再生
	//CSound::Play(BGM_Result);


	m_vPos[STAGE_NAME] = XMFLOAT2(0.0f, 216.0f);
	m_vSize[STAGE_NAME] = XMFLOAT2(350.0f, 175.0f);

	m_vPos[GAME_CLEAR] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[GAME_CLEAR] = XMFLOAT2(540.0f, 162.0f);
	m_vPos[GAME_OVER] = XMFLOAT2(0.0f, -128.0f);
	m_vSize[GAME_OVER] = XMFLOAT2(540.0f, 90.0f);
	m_vSize[GRAYSTAR] = XMFLOAT2(100.0f, 100.0f);
	m_vSize[STAR] = m_vSize[GRAYSTAR];


	m_vPos[BUTTON_NEXT] = XMFLOAT2(-200.0f, -240.0f);
	m_vPos[BUTTON_RETRY] = XMFLOAT2(-200.0f, -240.0f);
	m_vPos[BUTTON_SELECT] = XMFLOAT2(200.0f, -240.0f);

	m_vPos[LIFEGAGE] = XMFLOAT2(0.0, -110.0f);
	m_vSize[LIFEGAGE] = XMFLOAT2(500.0f, 50.0f);

	m_vPos[EMPTY] = XMFLOAT2(26.0, -110.0f);
	m_vSize[EMPTY] = XMFLOAT2(740.0f, 158.0f);
	m_vPos[CYLINDER] = XMFLOAT2(0.0f, -110.0f);
	m_vSize[CYLINDER] = XMFLOAT2(523.0f, 56.0f);

	m_vSize[BUTTON_NEXT] = XMFLOAT2(BUTTON_SIZE_X * HOSEI, BUTTON_SIZE_Y * HOSEI);
	m_vSize[BUTTON_RETRY] = XMFLOAT2(BUTTON_SIZE_X * HOSEI, BUTTON_SIZE_Y * HOSEI);
	m_vSize[BUTTON_SELECT] = XMFLOAT2(BUTTON_SIZE_X * HOSEI, BUTTON_SIZE_Y * HOSEI);

	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_vDefaultPos[i] = m_vPos[i];	// それぞれのPosの初期位置を保存		01/16 三輪卓弘
	}

	stnum = GetStageNum();

	//星表示の初期化	01/24	佐藤大地
	g_Star[0].pos = XMFLOAT3(-170.0f, 75.0f, 0.0f);
	g_Star[1].pos = XMFLOAT3(0.0f, 90.0f, 0.0f);
	g_Star[2].pos = XMFLOAT3(170.0f, 75.0f, 0.0f);

	g_Star[0].Angle = 15.0f;
	g_Star[1].Angle = 0.0f;
	g_Star[2].Angle = -15.0f;

	for (int i = 0; i < max_star; i++) {
		g_Star[i].Apha = 0.0f;
	}

	StarCnt = 0.0f;
	Life = GetPlayer()->GetO2();//酸素量取得
	//Life = 50.0f;//酸素量取得
	return hr;
}

// 終了処理
void ResultUI::Uninit() {
	//テクスチャ解放
	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		SAFE_RELEASE(m_pTexture[i]);
	}
}

// 更新
void ResultUI::Update(bool countUp)
{
	static int g_count = 0;

	//酸素ゲージの大きさ設定
	m_vSize[LIFEGAGE] = XMFLOAT2(Life*5.0f, 50.0f);
	m_vPos[LIFEGAGE].x = -250.0f + Life * 2.5f;

	static bool PushFlag = false;		//十字キーが押されたフレームのみ反応
	JOYINFOEX* joystate = GetJoyState(0);
	if (joystate->dwPOV != prevDW && prevDW >= 65535) {
		PushFlag = true;
	}
	else {
		PushFlag = false;
	}

	//ステージ〇を表記するために1減らす
	StageNameY = 1.0f / 5.0f * (stnum - 1);

	// checkをboolに よくなかったらもどします		01/18 三輪卓弘
	static bool check = 0;
	static bool nOldCheck = 0;	// 1フレーム前のcheck			01/16 三輪卓弘
	static int count = 0;

	count += 3;

	//カーソルがどちらにあっても、左右キーを押下でもう片方に移るように	01/18 三輪卓弘
	if ((PushFlag&&joystate->dwPOV == 9000)) { check = !check; }
	if ((PushFlag&&joystate->dwPOV == 27000)) { check = !check; }
	//if (check >= 2) { check = 1; }
	//if (check <  0) { check = 0; }

	// 選択中でないボタンを全て初期色に
	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	if (check == 0) {
		//m_vPos[3].y = -240.0f + SinDeg(count) * 3;	//選択中のボタンの色を変える処理と一括化したいため、ActionButton関数に		01/16 三輪卓弘
		//m_vPos[4].y = -240.0f + SinDeg(count) * 3;
		ActionButton(BUTTON_NEXT, count);
		ActionButton(BUTTON_RETRY, count);
		// [Enter]またはスペースキー押下
		if (GetJoyTrigger(0, XBOXA)) {
			CSound::Stop(SE_RESULTENTER);
			CSound::Play(SE_RESULTENTER);
			if (countUp) {
				SetStageNum(stnum + 1);
			}
			else {
				SetStageNum(stnum);
			}
			StartFadeOut(SCENE_GAME);
			return;
		}
	}
	else {
		ActionButton(BUTTON_SELECT, count);
		// [Enter]またはスペースキー押下
		if (GetJoyTrigger(0, XBOXA)) {
			CSound::Stop(SE_RESULTENTER);
			CSound::Play(SE_RESULTENTER);
			StartFadeOut(SCENE_SELECT);
			return;
		}
	}
	prevDW = joystate->dwPOV;
	//	PrintDebugProc("pos={%.2f,%.2f},size={%.2f,%.2f}\n", m_vPos[check].x, m_vPos[check].y, m_vSize[check].x, m_vSize[check].y);

	if (nOldCheck != check)	//checkが1フレーム前の情報と違う場合			01/16 三輪卓弘
	{
		for (int i = 0; i <= BUTTON_SELECT; i++) {
			count = 0.0f;					//揺れを初期化
			m_vPos[i] = m_vDefaultPos[i];	//全てのUIを初期値に

			CSound::Stop(SE_RESULTMOVE);
			CSound::Play(SE_RESULTMOVE);
		}
	}
	nOldCheck = check;		//現フレームのcheck値を取得				01/16 三輪卓弘
}

// 描画
void ResultUI::Draw(bool clear)
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);
	ID3D11DeviceContext* pDC = GetDeviceContext();
	if (clear) {
		//01/24		佐藤大地
		SetLife(Life);
		Star(Life);
	}

	for (int i = 0; i < MAX_RESULTUI_TEXUTURE; i++) {
		if (i == GRAYSTAR || i == STAR) { continue; }
		if (clear) {
			if (stnum < 5) {
				if (i == 2 || i == 4) { continue; }
			}
			else {
				if (i == 2 || i == 3) { continue; }
			}
		}
		else {
			if (i == 1 || i == 3) { continue; }
			if (i >= GRAYSTAR) { continue; }
		}
		if (i == 0) {
			SetPolygonFrameSize(1.0f, 0.2f);
			SetPolygonUV(0.0f, StageNameY);
		}
		else {
			SetPolygonFrameSize(1.0f, 1.0f);
			SetPolygonUV(0.0f, 0.0f);
		}
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonColor(m_vColor[i]);
		DrawPolygon(pDC);
	}

	//星描画
	if (clear) {
		for (int i = 0; i < max_star; i++) {
			SetPolygonSize(m_vSize[GRAYSTAR].x, m_vSize[GRAYSTAR].y);
			SetPolygonPos(g_Star[i].pos.x, g_Star[i].pos.y);
			SetPolygonAngle(g_Star[i].Angle);
			SetPolygonAlpha(1.0f);
			SetPolygonTexture(m_pTexture[GRAYSTAR]);
			SetPolygonUV(0.0f, 0.0f);
			DrawPolygon(pDC);

			SetPolygonSize(m_vSize[STAR].x, m_vSize[STAR].y);
			SetPolygonPos(g_Star[i].pos.x, g_Star[i].pos.y);
			SetPolygonAngle(g_Star[i].Angle);
			SetPolygonAlpha(g_Star[i].Apha);
			SetPolygonTexture(m_pTexture[STAR]);
			SetPolygonUV(0.0f, 0.0f);
			DrawPolygon(pDC);
		}
	}
	SetPolygonAngle(0.0f);
	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);
}

void ResultUI::ActionButton(int nUInum, int count) {
	m_vPos[nUInum].y = m_vDefaultPos[nUInum].y + SinDeg(count) * 3.0f;
	m_vColor[nUInum] = XMFLOAT4(1.2f, 1.2f, 1.2f, 1.0f);
}

//星の描写	01/24	佐藤大地
void ResultUI::Star(float nLife)
{
	float LifeRem = nLife;
	//const float upAlpha = 0.0005f;
	const float upAlpha = 0.005f;

	//01/31齋藤成希
	//debugだと遅いけどreleaseだとちょうどいい
	if (StarCnt <= 1.0f /*100.0f*/) {
		StarCnt += 0.0015f/* 0.020f*/;
	}
	else if (LifeRem >= 0.0f && g_Star[0].Apha <= 1.0f) {
		if (g_Star[0].Apha == 0.0f)
		{
			CSound::Stop(SE_RESULTSTAR);
			CSound::Play(SE_RESULTSTAR);
		}
		g_Star[0].Apha += upAlpha;
		if (g_Star[0].Apha >= 1.0f)
		{
			StarCnt = 0.0f;
		}
	}
	else if (LifeRem >= 20.0f && g_Star[1].Apha <= 1.0f) {
		if (g_Star[1].Apha == 0.0f)
		{
			CSound::Stop(SE_RESULTSTAR);
			CSound::Play(SE_RESULTSTAR);
		}
		g_Star[1].Apha += upAlpha;
		if (g_Star[1].Apha >= 1.0f)
		{
			StarCnt = 0.0f;
		}
	}
	else if (LifeRem >= 50.0f && g_Star[2].Apha <= 1.0f) {
		if (g_Star[2].Apha == 0.0f)
		{
			CSound::Stop(SE_RESULTSTAR);
			CSound::Play(SE_RESULTSTAR);
		}
		g_Star[2].Apha += upAlpha;
	}
}
//ライフバー短縮処理	01/24	佐藤大地
void ResultUI::SetLife(float nLife)
{
	if (nLife < 0.0f) {
		m_vSize[LIFEGAGE].x = 0.0f;
		return;
	}
	float sabun;
	//緑→赤ver	01/18 三輪卓弘
	if (nLife >= 50.0f) {
		m_vColor[LIFEGAGE] = XMFLOAT4(1.0f - ((nLife / 100) - (1.0f - nLife / 100)), 1.0f, 0.0f, 1.0f);
	}
	else {
		m_vColor[LIFEGAGE] = XMFLOAT4(1.0f, nLife / 100 + nLife / 100, 0.0f, 1.0f);
	}
}