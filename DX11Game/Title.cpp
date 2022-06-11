// タイトル画面 [title.cpp]
#include "Title.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "fade.h"
#include "debugproc.h"
#include "bg.h"
#include "Sound.h"

//===========================================
// マクロ定義
//===========================================
#define LOGO_PARPORSE
#define LOGO_SIZE_X		1260.0f	//ロゴの画像サイズX	01/15 三輪卓弘
#define LOGO_SIZE_Y		600.0f	//ロゴの画像サイズY	01/15 三輪卓弘

#define PRESS_SIZE_X    539.0f * 0.75f // ぷれすざえんたー　X
#define PRESS_SIZE_Y    94.0f  * 0.75f // ぷれすざえんたー　Y

#define PUSHA_TIMER 60.0f
#define BLINK_TIMER 60
#define BLINK_START_TIMER 5

// 画像情報
static std::string fileName_Title[]{
	"data/texture/astronaut.png",
	"data/texture/title.png",
	"data/texture/pushA.png",
	"data/texture/pushAShining.png"
};

enum eTITLE {
	TITLE_ASTRONAUT = 0,// 宇宙飛行士
	TITLE_LOGO,			// タイトルロゴ
	TITLE_PUSHA,		// A押してね
	TITLE_SHINING,		// 文字発光

	MAX_TITLE
};
//グローバル変数
static bool g_bAnimFlag = false;			//アニメーションが完了したかを判定するフラグ
static bool g_bOldAnimFlag = false;			//1フレーム前の同フラグ
static bool g_bFloatingFlag = false;		//フラグ:宇宙飛行士が最初の浮き上がりを完了したか	01/15 三輪卓弘

static float fMoveY = 2.8f;
static bool g_bStart;
static int g_nTitleShiningAnimTime;

static bool g_bPushAAlpha = true;		// PushAの不透明度をプラスするかマイナスするか
static float g_fPushATimer = 0.0f;		// PushAの点滅間隔
static bool g_bPushAFlag = false;			// PushAを押したかどうか

// 初期化
HRESULT Title::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ読み込み
	for (int i = 0; i < MAX_TITLE_TEXUTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName_Title[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr))
			return hr;
	}

	//メンバ変数の初期化
	//宇宙飛行士
	m_vPos[TITLE_ASTRONAUT] = XMFLOAT2(-370.0f, -40.0f);
	m_vSize[TITLE_ASTRONAUT] = XMFLOAT2(211.11f, 144.44f);
	m_vColor[TITLE_ASTRONAUT].w = 0.0f;

	//ロゴ
	m_vPos[TITLE_LOGO] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[TITLE_LOGO] = XMFLOAT2(LOGO_SIZE_X, 0.0f); //Xサイズを画像の横幅と等しい値に	01/15 三輪卓弘
	m_vColor[TITLE_LOGO].w = 1.0f;
	g_bAnimFlag = false;

	// ぷれすざえんたー
	m_vPos[TITLE_PUSHA] = XMFLOAT2(0.0f, -250.0f);
	m_vSize[TITLE_PUSHA] = XMFLOAT2(PRESS_SIZE_X, PRESS_SIZE_Y);
	m_vColor[TITLE_PUSHA].w = 0.0f;

	// Aおしてね　光るバージョン 01/25 三輪卓弘
	m_vPos[TITLE_SHINING] = XMFLOAT2(0.0f, -250.0f);
	m_vSize[TITLE_SHINING] = XMFLOAT2(PRESS_SIZE_X, PRESS_SIZE_Y);
	m_vColor[TITLE_SHINING].w = 0.0f;

	// BGM再生
	CSound::Play(BGM_TITLE);
	CSound::Play(SE_GOGOGO);

	return hr;
}

// 終了処理
void Title::Uninit()
{
	// BGM再生停止
	CSound::Stop(BGM_TITLE);
	CSound::Stop(SE_STAGEENTER);
	for (int i = 0; i < MAX_TITLE_TEXUTURE; ++i) {
		//背景テクスチャ解放
		SAFE_RELEASE(m_pTexture[i]);
	}
}

// 更新
void Title::Update()
{

	static float sindegCount = 0.0f;		//宇宙飛行士をふわふわさせるためのカウント

	// [Enter]またはスペースキー押下
	if (g_bAnimFlag && (GetJoyTrigger(0, XBOXA)) && g_bPushAFlag == false) {
		g_bStart = true;
		//発光文字の不透明度を1に		 01/25	三輪卓弘
		m_vColor[TITLE_SHINING].w = 1.0f;

		CSound::Stop(SE_STAGEENTER);
		CSound::Play(SE_STAGEENTER);
		//Time変数を使った発光処理		 01/25	三輪卓弘


		StartFadeOut(SCENE_SELECT);
		return;
	}

	//アニメーションをすっ飛ばす
	if (GetJoyTrigger(0, XBOXA)) {
		g_bAnimFlag = true;
	}

	//アニメーション処理
	if (!g_bAnimFlag&&m_vSize[1].y < LOGO_SIZE_Y) {
		m_vSize[1].y += 2.0f;
	}
	else {
		g_bAnimFlag = true;
	}

	//アニメーション完了後
	if (g_bAnimFlag) {
		CSound::Stop(SE_GOGOGO);
		//縦移動力を宇宙飛行士に反映	01/15 三輪卓弘
		m_vPos[0].y += fMoveY;
		if (g_bFloatingFlag == false)
		{
			fMoveY -= 0.015f;
			if (m_vPos[TITLE_ASTRONAUT].y >= 200)
			{
				g_bFloatingFlag = true;
			}
		}

		if (g_bFloatingFlag == true)
		{
			if (m_vPos[TITLE_ASTRONAUT].y >= 200)
			{
				fMoveY -= 0.01f;
			}
			else
			{
				fMoveY += 0.01f;
			}
		}
		//ロゴのさいずを固定	01/15 三輪卓弘
		m_vSize[1].y = 600.0f;
	}
	//最初のアニメーションが終わったらSEを再生
	if (g_bOldAnimFlag != g_bAnimFlag)
	{

		CSound::Stop(SE_TITLEPOP);
		CSound::Play(SE_TITLEPOP);
	}
	if (g_bStart && g_nTitleShiningAnimTime <= 30.0f)
	{
		//ポリゴンの拡大処理
		m_vSize[TITLE_SHINING].x += PRESS_SIZE_X * 0.015f;
		m_vSize[TITLE_SHINING].y += PRESS_SIZE_Y * 0.015f;
		//発光文字の不透明度を、アニメーションの進み具合に合わせて不透明度を下げる	 01/23	三輪卓弘
		m_vColor[TITLE_SHINING].w = 1.0f - (g_nTitleShiningAnimTime / 30.0f);
		g_nTitleShiningAnimTime++;
	}
	//アニメーション未完了の場合、宇宙飛行士のみを完全に透明に(=見えなく)する	01 / 15 三輪卓弘
	//アニメーション未完了の場合、宇宙飛行士とPRESSAボタンを完全透明するに変更　01/24　X
	if (!g_bAnimFlag)
	{
		m_vColor[TITLE_ASTRONAUT].w = 0.0f;
		m_vColor[TITLE_PUSHA].w = 0.0f;
	}
	else
	{
		m_vColor[TITLE_ASTRONAUT].w = 1.0f;
		//PushAの不透明度アニメーション処理		//01/26	三輪卓弘
		m_vColor[TITLE_PUSHA].w = (g_fPushATimer / PUSHA_TIMER);
		if (g_bPushAAlpha)
		{
			if (g_fPushATimer >= PUSHA_TIMER)
			{
				g_bPushAAlpha = false;
			}
			g_fPushATimer++;
		}
		else
		{
			if (g_fPushATimer <= 0)
			{
				g_bPushAAlpha = true;
			}
			g_fPushATimer--;
		}
	}
#if _DEBUG
	PrintDebugProc("x,y={%.2f,%.2f}\n", m_vPos[0].x, m_vPos[0].y);
	PrintDebugProc("fMoveY={%.2f}\n", fMoveY);
	PrintDebugProc("m_vColor[TITLE_PUSHA].w:%.2f\n", m_vColor[TITLE_PUSHA].w);
#endif
	g_bOldAnimFlag = g_bAnimFlag;
}

// 描画
void Title::Draw()
{
	// Zバッファ無効(Zチェック無&Z更新無)
	SetZBuffer(false);

	DrawBG("title");

	ID3D11DeviceContext* pDC = GetDeviceContext();
	//描画
	for (int i = 0; i < MAX_TITLE_TEXUTURE; i++) {
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonAlpha(m_vColor[i].w);
		DrawPolygon(pDC);
	}
	// Zバッファ有効(Zチェック有&Z更新有)
	SetZBuffer(true);
}