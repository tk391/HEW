//=============================================================================
// 背景 [bg.cpp]
//12/11 齋藤成希
//=============================================================================
#include "Rayer.h"
#include "Texture.h"
#include "polygon.h"
#include "Select.h"
#include "Camera.h"

//=============================================================================
// マクロ定義
//=============================================================================
#define SEIZA_MOVE		40	//背景の星座の動きやすさ			01/15 三輪卓弘
#define STARS_MOVE		30	//背景の星のレイヤーの動きやすさ	01/15 三輪卓弘
#define PLANET_MOVE		20	//背景の火星とかの動きやすさ		01/15 三輪卓弘

static std::string fileName[]{
	"data/texture/selectMoon.png",
	"data/texture/selectMarse.png",
	"data/texture/selectJupiter.png",
	"data/texture/selectVinus.png",
	"data/texture/asteroids.png",

	//星座たち
	"data/texture/Aquarius.png",
	"data/texture/Aries.png",
	"data/texture/Cancer.png",
	"data/texture/Capricorn.png",
	"data/texture/Gemini.png",
	"data/texture/Leo.png",
	"data/texture/Libra.png",
	"data/texture/Pisces.png",
	"data/texture/Sagittarius.png",
	"data/texture/Scoepio.png",
	"data/texture/Taurus.png",
	"data/texture/Virgo.png",

	"data/texture/動かない背景2.png"	//(仮)星レイヤーの層を増やすために追加	01/15 三輪卓弘
};

//=============================================================================
// グローバル変数
//=============================================================================

static int OutSeiza[3];
XMFLOAT3 CameraPos = XMFLOAT3(0.0f, 0.0f, 0.0f);	//カメラの位置をもらうための変数	01/15 三輪卓弘

//=============================================================================
// 初期化
//=============================================================================
HRESULT Rayer::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// テクスチャ読込
	for (int i = 0; i < MAX_RAYER_TEXTURE; ++i) {
		hr = CreateTextureFromFile(pDevice, fileName[i].c_str(), &m_pTexture[i]);
		if (FAILED(hr)) {
			return hr;
		}
	}
	for (int i = 0; i < 3; i++) {
		OutSeiza[i] = (rand() % 12) + 5;
	}

	//変数初期化
	for (int i = 0; i < 5; i++) {
		//星
		m_vPos[i] = XMFLOAT2(0.0f, -100.0f);
		m_vSize[i] = XMFLOAT2(700.0f, 700.0f);
	}
	//星座（仮）
	m_vPos[OutSeiza[0]] = XMFLOAT2(300.0f, -200.0f);
	m_vSize[OutSeiza[0]] = XMFLOAT2(150.0f, 150.0f);
	m_vPos[OutSeiza[1]] = XMFLOAT2(1200.0f, 200.0f);
	m_vSize[OutSeiza[1]] = XMFLOAT2(200.0f, 200.0f);
	m_vPos[OutSeiza[2]] = XMFLOAT2(-300.0f, -200.0f);
	m_vSize[OutSeiza[2]] = XMFLOAT2(250.0f, 250.0f);

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void Rayer::Uninit()
{
	for (int i = 0; i < MAX_RAYER_TEXTURE; ++i) {
		// 背景テクスチャ解放
		SAFE_RELEASE(m_pTexture[i]);
	}
}

//=============================================================================
// 更新
//=============================================================================
void Rayer::Update()
{
	//特になし
}

//=============================================================================
// 描画
//=============================================================================
void Rayer::Draw()
{
	//カメラの座標を取得	01/15 三輪卓弘
	CameraPos = CCamera::Get()->GetPos();

	int stnum = GetStageNum();
	--stnum;

	ID3D11DeviceContext* pDC = GetDeviceContext();

	//星座
	//ランダムで3つだけステージ内に表示
	for (int i = 0; i < 3; ++i) {
		//カメラが動くに伴い、背景描画の位置も少し動くように		01/15 三輪卓弘
		//「カメラの座標 ÷ n(↓でいうと30）」の数値を影響させている。nの値が大きいほど動きが鈍くなる。
		SetPolygonPos(m_vPos[OutSeiza[i]].x - (CameraPos.x / SEIZA_MOVE), m_vPos[OutSeiza[i]].y - (CameraPos.y / SEIZA_MOVE));
		SetPolygonSize(m_vSize[OutSeiza[i]]);
		SetPolygonTexture(m_pTexture[OutSeiza[i]]);
		SetPolygonUV(0.0f, 0.0f);
		SetPolygonColor(1.0f, 1.0f, 1.0f);
		DrawPolygon(pDC);
	}

	//(仮)背景レイヤー1
	//カメラが動くに伴い、背景描画の位置も少し動くように	01/15 三輪卓弘
	SetPolygonPos(0.0f - (CameraPos.x / STARS_MOVE), 0.0f - (CameraPos.y / STARS_MOVE));
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonTexture(m_pTexture[17]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

	//スタート時の大きな星
	//カメラが動くに伴い、背景描画の位置も少し動くように	01/15 三輪卓弘
	SetPolygonPos(m_vPos[stnum].x - (CameraPos.x / PLANET_MOVE), m_vPos[stnum].y - (CameraPos.y / PLANET_MOVE));
	SetPolygonSize(m_vSize[stnum].x + CameraPos.z / 40, m_vSize[stnum].y + CameraPos.z / 40);
	SetPolygonTexture(m_pTexture[stnum]);
	SetPolygonUV(0.0f, 0.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	DrawPolygon(pDC);

}

//=============================================================================
// 位置のセット
//=============================================================================
void Rayer::Set(float moveX, float moveY, bool ClearFlag) {
	int stnum = GetStageNum();
	--stnum;

	m_vPos[stnum].x -= moveX;
	m_vPos[stnum].y -= moveY;
	if (ClearFlag) {
		for (int i = 0; i < 3; ++i) {
			m_vPos[OutSeiza[i]].x -= moveX;
			m_vPos[OutSeiza[i]].y -= moveY;
		}
		m_vPos[17].x -= moveX * 1.8f;
		m_vPos[17].y -= moveY * 1.8f;
	}
}