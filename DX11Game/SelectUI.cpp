//===========
//01/16 三輪卓弘 メンバ変数の配列を 列挙体 ESelectUI の中身に差し替え

#include "SelectUI.h"
#include"Texture.h"
#include"polygon.h"
#include "debugproc.h"
#include"Light.h"
#include "Scene.h"

//デバッグ確認用
#include "input.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define STAR_FIRST		-550.0f				//星UI 一番←の位置(y)				01/16 三輪卓弘
#define STAR_POS_Y		290.0f				//星UI Y座標						01/16 三輪卓弘
#define STAR_INTERVAL	105.0f				//星UI 画像ごとの間隔(x)			01/16 三輪卓弘
#define STAR_SIZE		50.0f				//星UI 表示サイズ(x,y)				01/16 三輪卓弘
#define STAR_SIZE_SAT_X	STAR_SIZE * 1.52f	//星UI 表示サイズ(x,y)				01/16 三輪卓弘

#define STAR_FLAMESIZEX	701.0f				//星枠 サイズ(x)					01/16 三輪卓弘
#define STAR_FLAMESIZEY	101.0f				//星枠 サイズ(y)					01/16 三輪卓弘

#define ASTRONAUT_X		950					//宇宙飛行士の元画像サイズX			01/18 三輪卓弘
#define ASTRONAUT_Y		650					//宇宙飛行士の元画像サイズY			01/18 三輪卓弘
#define ASTRONAUT_MOVE	0.4f				//宇宙飛行士のふわふわ上限値		01/18 三輪卓弘
#define ASTRONAUT_SPEED	0.01f				//宇宙飛行士のふわふわ速度			01/18 三輪卓弘

#define SHUTTER_SIZE_X	617.0f				//シャッターの元画像サイズX		01/18 三輪卓弘
#define SHUTTER_SIZE_Y	435.0f				//シャッターの元画像サイズY		01/18 三輪卓弘

#define SHINING_ANIM_TIME	60.0f				//ステージ名の発光時間			01/25 三輪卓弘
#define SHINING_ANIM_SPEED	1.0f			//ステージ名の発光速度			01/25 三輪卓弘

#define NOPLAY_ANIM_TIME	2				//"まだ遊べないよ"が揺れる時間	01/23 三輪卓弘
#define NOPLAY_ANIM_SPEED	10.0f			//"まだ遊べないよ"が揺れる速度	01/23 三輪卓弘

#define STAGE_ANIM			10.0f			//ステージ名が揺れる速度		01/23 三輪卓弘
#define STAGENAME_POS		XMFLOAT2(0.0f, -250.0f)	//同サイズの画像が多かったのでデファイン化	01/25 三輪卓弘

//画像情報
static std::string fileName_SelectUI[]{
	"data/texture/defoMoonShrink.png",
	"data/texture/defoMarseShrink.png",
	"data/texture/defoJupiterShrink.png",
	"data/texture/defoSturnShrink.png",
	"data/texture/defoAsteroisShrink.png",

	"data/texture/astronaut.png",
	"data/texture/arrowSprites.png",
	"data/texture/arrow2Sprites.png",
	"data/texture/noplay.png",
	"data/texture/StarFlame2.png",
	"data/texture/shutter.png",

	"data/texture/stage1.png",
	"data/texture/stage2.png",
	"data/texture/stage3.png",
	"data/texture/stage4.png",
	"data/texture/stage5.png",

	"data/texture/stage1Shining.png",
	"data/texture/stage2Shining.png",
	"data/texture/stage3Shining.png",
	"data/texture/stage4Shining.png",
	"data/texture/stage5Shining.png"
};

//グローバル変数
static float g_fStageNameY;
XMFLOAT2 g_DefArrowRight;		//→矢印のデフォ位置						01/16 三輪卓弘
XMFLOAT2 g_DefArrowLeft;		//←矢印のデフォ位置						01/16 三輪卓弘
static float g_fMoveY;			//宇宙飛行士のふわふわ力を管理する変数		01/18 三輪卓弘
static bool g_bMoveUP;			//現在の宇宙飛行士のふわふわ方向 上or下			01/18 三輪卓弘

static bool g_bShiningAnimFlag;			// ステージの文字の発光フラグ			01/25 三輪卓弘
static bool g_bOldShiningAnimFlag;		// 直前のステージの文字の発光フラグ		01/25 三輪卓弘
static int  g_nShiningAnimTime;			// ステージの文字の発光時間				01/25 三輪卓弘

static bool g_bNoPlayAnimFlag;			// "まだ遊べないよ"の文字が揺れるフラグ	01/23 三輪卓弘
static int  g_nNoPlayAnimTime;			// "まだ遊べないよ"の文字が揺れる時間	01/23 三輪卓弘

static bool g_bStageAnimFlag;			// ステージの文字が揺れるフラグ			01/23 三輪卓弘
static int  g_nStageAnimTime;			// ステージの文字が揺れる時間			01/23 三輪卓弘
static float g_fDeffStageNameY;			// ステージの文字の初期Y座標			01/23 三輪卓弘

//=============================================================================
//初期化
//=============================================================================
HRESULT SelectUI::Init() {

	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ読み込み
	for (int i = 0; i < MAX_SELECTUI_TEXUTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName_SelectUI[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr))
			return hr;
	}

	//メンバ変数の初期化
	m_vPos [MOON_1]		= XMFLOAT2(STAR_FIRST, STAR_POS_Y);
	m_vSize[MOON_1]		= XMFLOAT2(STAR_SIZE, STAR_SIZE);
	m_vPos[MARSE_2]		= XMFLOAT2(m_vPos[MOON_1].x    + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[MARSE_2]	= XMFLOAT2(STAR_SIZE, STAR_SIZE);
	m_vPos[JUPITER_3]	= XMFLOAT2(m_vPos[MARSE_2].x   + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[JUPITER_3]	= XMFLOAT2(STAR_SIZE, STAR_SIZE);
	m_vPos[VINUS_4]		= XMFLOAT2(m_vPos[JUPITER_3].x + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[VINUS_4]	= XMFLOAT2(STAR_SIZE_SAT_X, STAR_SIZE);
	m_vPos[ASTEROIDS_5] = XMFLOAT2(m_vPos[VINUS_4].x   + STAR_INTERVAL, STAR_POS_Y);
	m_vSize[ASTEROIDS_5]= XMFLOAT2(STAR_SIZE, STAR_SIZE);

	m_vPos[ASTRONAUT]	= XMFLOAT2(STAR_FIRST, STAR_POS_Y + 10.0f);
	m_vSize[ASTRONAUT]	= XMFLOAT2(ASTRONAUT_X / 16, ASTRONAUT_Y / 16);
	m_vPos[ARROW_RIGHT] = XMFLOAT2(300.0f, -250.0f);
	m_vSize[ARROW_RIGHT]= XMFLOAT2(100.0f, 100.0f);
	m_vPos[ARROW_LEFT]	= XMFLOAT2(-300.0f, -250.0f);
	m_vSize[ARROW_LEFT] = XMFLOAT2(100.0f, 100.0f);
	m_vPos[NOPLAY] = STAGENAME_POS;
	m_vSize[NOPLAY] = XMFLOAT2(0.0f, 0.0f);
	m_vPos[SHUTTER] = STAGENAME_POS;
	m_vSize[SHUTTER] = XMFLOAT2(SHUTTER_SIZE_X / 1.8f, SHUTTER_SIZE_Y / 1.8f);
	m_vPos[STARFLAME] = m_vPos[JUPITER_3];
	m_vSize[STARFLAME] = XMFLOAT2(STAR_FLAMESIZEX * 0.7f, STAR_FLAMESIZEY * 0.7f);
	
	for (int i = STAGENAME1; i <= NAMESHINING5; ++i) {
		m_vPos[i] = STAGENAME_POS;
		m_vSize[i] = XMFLOAT2(300.0f, 150.0f);
	}

	//色初期化
	for (int i = 0; i < MAX_SELECTUI_TEXUTURE; ++i) {
		m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		if (i >= STAGENAME1)
		{
			m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
		}
	}
	//矢印の初期位置を保存		01/16 三輪卓弘
	g_DefArrowRight = m_vPos[ARROW_RIGHT];
	g_DefArrowLeft  = m_vPos[ARROW_LEFT];
	g_fStageNameY = 0.0f;

	g_fDeffStageNameY = m_vPos[STAGENAME1].y;	//ステージの文字の初期X座標			01/23 三輪卓弘
	g_fMoveY = 0.0f;			//宇宙飛行士のふわふわ力を管理する変数		01/18 三輪卓弘
	g_bMoveUP = 1;				//現在の宇宙飛行士のふわふわ方向 上or下		01/18 三輪卓弘
	g_bShiningAnimFlag = 0;		// ステージの文字の発光フラグ				01/25 三輪卓弘
	g_bOldShiningAnimFlag = 0;	// 直前のステージの文字の発光フラグ			01/25 三輪卓弘
	g_nShiningAnimTime = 0;		// ステージの文字の発光時間					01/25 三輪卓弘

	g_bNoPlayAnimFlag = 0;		// "まだ遊べないよ"の文字が揺れるフラグ		01/23 三輪卓弘
	g_nNoPlayAnimTime = 0;		// "まだ遊べないよ"の文字が揺れる時間		01/23 三輪卓弘

	g_bStageAnimFlag = 0;		// ステージの文字が揺れるフラグ				01/23 三輪卓弘
	g_nStageAnimTime = 0;		// ステージの文字が揺れる時間				01/23 三輪卓弘
	return hr;
}

//=============================================================================
//終了
//=============================================================================
void SelectUI::Uninit() {
	for (int i = 0; i < MAX_SELECTUI_TEXUTURE; ++i) {
		//背景テクスチャ解放
		SAFE_RELEASE(m_pTexture[i]);
	}
}

//=============================================================================
//更新
//=============================================================================
void SelectUI::Update(int stageNum) {
	stageNum--;
	g_fStageNameY = 1.0f / 5.0f * stageNum;
	for (int i = MOON_1; i <= ASTEROIDS_5; i++) {
		//進行度に達しているものは通常表示に
		if (i < GetProgress()-1) {
			m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else if(i == GetProgress() - 1) {	// プレイ可能かつ未クリアの星アイコンはうっすら表示
			m_vColor[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
		}
		else { //進行度に達していない星アイコンは灰色に
			m_vColor[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.2f);
		}
	}

	//現在選択中のステージが進行度に達しているか
	if (stageNum < GetProgress()) {
		m_vColor[ASTRONAUT] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_vColor[NOPLAY].w = 0.0f;
		m_vColor[SHUTTER].w = 0.0f;
	}
	else {
		m_vSize[NOPLAY] = m_vSize[STAGENAME1];
		m_vColor[ASTRONAUT] = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.4f);
		m_vColor[NOPLAY].w = 1.0f;
		m_vColor[SHUTTER].w = 1.0f;
	}

	for (int i = STAGENAME1; i <= STAGENAME5; i++)
	{
		m_vColor[i].w = 0.0f;
	}
	m_vColor[STAGENAME1 + stageNum].w = 1.0f;

	//現在選択中の→ステージが進行度に達していなければ、矢印を薄くする	01/23 三輪卓弘
	if (GetProgress() < stageNum + 2 && stageNum != 4) {
		m_vColor[ARROW_RIGHT].w = 0.6f;
	}
	else 
	{
		m_vColor[ARROW_RIGHT].w = 1.0f;
	}
	//現在選択中の←ステージが進行度に達していなければ、矢印を薄くする	01/23 三輪卓弘
	if (GetProgress() < stageNum || (stageNum == 0 && GetProgress() < 5)) {
		m_vColor[ARROW_LEFT].w = 0.6f;
	}
	else
	{
		m_vColor[ARROW_LEFT].w = 1.0f;
	}

	//ミニ宇宙飛行士ふわふわ処理(縦移動力g_fMoveYを宇宙飛行士に反映)	01/18 三輪卓弘
	m_vPos[ASTRONAUT].y += g_fMoveY;
	if (g_fMoveY >= ASTRONAUT_MOVE) //宇宙飛行士の上移動が上限値に達したら
	{
		g_bMoveUP = false; //下移動状態に
	}
	else if (g_fMoveY <= -ASTRONAUT_MOVE) //宇宙飛行士の下移動が下限値に達したら
	{
		g_bMoveUP = true; //上移動状態に
	}
	if (g_bMoveUP == true)
	{
		g_fMoveY += ASTRONAUT_SPEED;
	}
	else
	{
		g_fMoveY -= ASTRONAUT_SPEED;
	}
	//"まだ遊べないよ"の文字が揺れるフラグがtrueだったら		 01/23	三輪卓弘
	if (g_bNoPlayAnimFlag == true)
	{
		//Time変数を使った"まだ遊べないよ"の文字を揺らす処理		 01/23	三輪卓弘
		g_nNoPlayAnimTime++;
		if (g_nNoPlayAnimTime <= NOPLAY_ANIM_TIME)
		{
			m_vPos[NOPLAY].x += NOPLAY_ANIM_SPEED;
		}
		if (NOPLAY_ANIM_TIME < g_nNoPlayAnimTime && g_nNoPlayAnimTime <= NOPLAY_ANIM_TIME * 3)
		{
			m_vPos[NOPLAY].x -= NOPLAY_ANIM_SPEED;
		}
		if (NOPLAY_ANIM_TIME * 3 < g_nNoPlayAnimTime && g_nNoPlayAnimTime <= NOPLAY_ANIM_TIME * 4)
		{
			m_vPos[NOPLAY].x += NOPLAY_ANIM_SPEED;
		}
		if (g_nNoPlayAnimTime >= NOPLAY_ANIM_TIME * 4)
		{
			g_bNoPlayAnimFlag = false;
			m_vPos[NOPLAY].x = 0.0f;
		}
	}
	//"まだ遊べないよ"の文字が揺れるフラグがtrueだったら		 01/23	三輪卓弘
	if (g_bStageAnimFlag == true)
	{
		//Time変数を使ったステージの文字を揺らす処理		 01/23	三輪卓弘
		if (g_nStageAnimTime <= STAGE_ANIM)
		{
			//デフォルト座標にアニメーションの値をかけて補正				 01/23	三輪卓弘
			m_vPos[STAGENAME1 + stageNum].y = g_fDeffStageNameY + (g_nStageAnimTime / STAGE_ANIM) * 5;
			g_nStageAnimTime++;
		}
		else
		{
			g_bStageAnimFlag = false;		//アニメーションフラグがfalseに		 01/23	三輪卓弘
		}
	}

	//発光フラグがtrueだったら		 01/25	三輪卓弘
	if (g_bShiningAnimFlag == true)
	{
		if (g_bShiningAnimFlag != g_bOldShiningAnimFlag)
		{
			//該当ステージの発光文字の不透明度を1に		 01/25	三輪卓弘
			m_vColor[NAMESHINING1 + stageNum].w = 1.0f;
		}
		//Time変数を使った発光処理		 01/25	三輪卓弘
		if (g_nShiningAnimTime <= SHINING_ANIM_TIME)
		{
			//ポリゴンの拡大処理
			m_vSize[NAMESHINING1 + stageNum].x += SHUTTER_SIZE_X * 0.008f;
			m_vSize[NAMESHINING1 + stageNum].y += SHUTTER_SIZE_Y * 0.008f;
			//該当ステージの発光文字の不透明度を、アニメーションの進み具合に合わせて不透明度を下げる	 01/23	三輪卓弘
			m_vColor[NAMESHINING1 + stageNum].w = 1.0f - (g_nShiningAnimTime / SHINING_ANIM_TIME);
			g_nShiningAnimTime++;
		}
		else
		{
			g_bShiningAnimFlag = false;		//アニメーションフラグがfalseに		 01/23	三輪卓弘
		}
		g_bOldShiningAnimFlag = g_bShiningAnimFlag;
	}
}

//=============================================================================
//描画
//=============================================================================
void SelectUI::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();

	//宇宙飛行士	01/18 三輪卓弘
	SetPolygonSize(m_vSize[STARFLAME].x, m_vSize[STARFLAME].y);
	SetPolygonPos(m_vPos[STARFLAME].x, m_vPos[STARFLAME].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[STARFLAME]);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	//星の描画
	for (int i = MOON_1; i <= ASTEROIDS_5; i++) {
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonColor(m_vColor[i].x, m_vColor[i].y, m_vColor[i].z);
		SetPolygonAlpha(m_vColor[i].w);	//星描画の不透明度は、ステージ進行度によって変更される
		DrawPolygon(pDC);
	}

	SetPolygonAlpha(1.0f);

	//宇宙飛行士	01/16 三輪卓弘
	SetPolygonSize(m_vSize[ASTRONAUT].x, m_vSize[ASTRONAUT].y);
	SetPolygonPos(m_vPos[ASTRONAUT].x, m_vPos[ASTRONAUT].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[ASTRONAUT]);
	SetPolygonAlpha(m_vColor[ASTRONAUT].w);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	//ステージ名
	for (int i = STAGENAME1; i < NAMESHINING5; ++i) 
	{
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonFrameSize(1.0f, 1.0f);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonColor(m_vColor[i].x, m_vColor[i].y, m_vColor[i].z);
		SetPolygonAlpha(m_vColor[i].w);
		DrawPolygon(pDC);
	}
/*	if (ANIM_THIRD <= g_fAnimProgress && g_fAnimProgress < ANIM_FOURTH)
	{//ポリゴンの拡大処理
		g_vSize.x += TEXTURE_SIZEX * 0.03f;
		g_vSize.y += TEXTURE_SIZEY * 0.03f;

		m_Color.w = 1 - (g_fAnimProgress - ANIM_THIRD) / (ANIM_FOURTH - ANIM_THIRD);		//ポリゴンの不透明度が1⇒0
	}
	*/
	//シャッター
	SetPolygonSize(m_vSize[SHUTTER].x, m_vSize[SHUTTER].y);
	SetPolygonPos(m_vPos[SHUTTER].x, m_vPos[SHUTTER].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[SHUTTER]);
	SetPolygonAlpha(m_vColor[SHUTTER].w);
	SetPolygonColor(m_vColor[SHUTTER].x, m_vColor[SHUTTER].y, m_vColor[SHUTTER].z);
	DrawPolygon(pDC);

	//矢印
	for (int i = ARROW_RIGHT; i <= ARROW_LEFT; i++) {
		SetPolygonSize(m_vSize[i].x, m_vSize[i].y);
		SetPolygonPos(m_vPos[i].x, m_vPos[i].y);
		SetPolygonFrameSize(0.25f, 0.25f);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonColor(m_vColor[i].x, m_vColor[i].y, m_vColor[i].z);
		SetPolygonAlpha(m_vColor[i].w);
		DrawPolygon(pDC);
	}

	//不透明度をMAXに
	SetPolygonAlpha(1.0f);

	//選べない(まだあそ)
	SetPolygonSize(m_vSize[NOPLAY].x, m_vSize[NOPLAY].y);
	SetPolygonPos(m_vPos[NOPLAY].x, m_vPos[NOPLAY].y);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonTexture(m_pTexture[NOPLAY]);
	SetPolygonColor(m_vColor[NOPLAY].x, m_vColor[NOPLAY].y, m_vColor[NOPLAY].z);
	SetPolygonAlpha(m_vColor[NOPLAY].w);
	DrawPolygon(pDC);

	//元に戻す
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonFrameSize(1.0f, 1.0f);
	SetPolygonUV(0.0f, 0.0f);
}

//=============================================================================
//画面上の宇宙飛行士の位置を選択中のステージと同期	01/16三輪卓弘
//=============================================================================
void SelectUI::AstroFloating(int num, int animation, int stageNum) {
	//宇宙飛行士の座標 ＝ 選択中ステージの座標X ＋ アニメーションでの移動量
	m_vPos[ASTRONAUT].x = m_vPos[stageNum].x + (float)animation * 6;
}
//=============================================================================
//Select.cppの内容と同期して、矢印キーをちょっと動かす関数	01/16三輪卓弘
//=============================================================================
void SelectUI::ArrowAnim(int LR, int animation) {
	//宇宙飛行士の座標 ＝ 選択中ステージの座標X ＋ アニメーションでの移動量
	if (!LR) // LRの値が右の場合
	{
		if (animation > 0 && 8 > animation) // アニメーションが1以上の時
		{	//矢印キーのx座標		＝矢印キーの初期x座標+ アニメーションの進行度 * 5
			m_vPos[ARROW_RIGHT].x = g_DefArrowRight.x + animation * 5.0f;
		}
		else
		{	//折返し時点から、進んだ矢印をもとに戻していく
			m_vPos[ARROW_RIGHT].x -= 5.0f;
		}
	}
	else // LRの値が左の場合
	{
		if (animation < 0 && -8 < animation) // アニメーションが1以上の時
		{	//矢印キーのx座標		＝矢印キーの初期x座標+ アニメーションの進行度 * 5
			m_vPos[ARROW_LEFT].x = g_DefArrowLeft.x + animation * 5.0f;
		}
		else
		{	//折返し時点から、進んだ矢印をもとに戻していく
			m_vPos[ARROW_LEFT].x += 5.0f;
		}
	}
}

//=============================================================================
//Select.cppから、"まだ遊べないよ"の揺れるフラグを設定	01/23三輪卓弘
//=============================================================================
void SelectUI::NoPlayAnimSet() {
	g_bNoPlayAnimFlag = true;
	g_nNoPlayAnimTime = 0;
}

//=============================================================================
//Select.cppから、"まだ遊べないよ"の揺れるフラグを取得	01/23三輪卓弘
//=============================================================================
bool SelectUI::NoPlayAnimGet() {
	return g_bNoPlayAnimFlag;
}

//=============================================================================
//Select.cppから、ステージ名の揺れるフラグを設定	01/23三輪卓弘
//=============================================================================
void SelectUI::StageAnimSet() {
	g_bStageAnimFlag = true;
	g_nStageAnimTime = 0;
}

//=============================================================================
//Select.cppから、ステージ名の発光フラグを設定	01/23三輪卓弘
//=============================================================================
void SelectUI::ShiningAnimSet() {
	g_bShiningAnimFlag = true;
	g_nShiningAnimTime = 0;
}