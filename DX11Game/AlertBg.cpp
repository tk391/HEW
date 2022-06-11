//警告時画面赤化
#include "AlertBg.h"
#include "Texture.h"
#include "polygon.h"
#include "Sound.h"
#include "debugproc.h"

//定数
const std::string ArtBGTexture[] = {
	"data/texture/RedBg.png",
	"data/texture/OutOfArea.png" 
};//テクスチャ名

//グローバル変数
static float g_fAlphaAlt;				//透明度
static bool AltFlg;				//アラートが鳴っているか
static bool StartAFlg;		//アラートのスタート

//初期化
HRESULT Alert::Init() {
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();

	// テクスチャ読込
	for (int i = 0; i < 2; i++) {
		hr = CreateTextureFromFile(pDevice, ArtBGTexture[i].c_str(), &m_pTexture[i]);
	}

	//メンバ変数（グローバル変数）初期化
	m_vPos[0] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[0] = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);
	m_vPos[1] = XMFLOAT2(0.0f, 0.0f);
	m_vSize[1] = XMFLOAT2(700.0f, 132.0f);
	g_fAlphaAlt = 0.0f;
	StartAFlg = false;

	return hr;
}

//終了
void Alert::Uninit() {
	// 背景テクスチャ解放
	for (int i = 0; i < 2; i++) {
		SAFE_RELEASE(m_pTexture[i]);
	}
}

//アップデート
void Alert::Update() {
	static int Count = 0;
	//アラート発動
	if (StartAFlg) {
		Count++;
		if (Count >= 60) {
			//一定値超えたらSE再生
			Count = 0;
			CSound::Play(SE_WARNING);
		}
	}
}

//描画
void Alert::Draw() {
	ID3D11DeviceContext* pDC = GetDeviceContext();
	for (int i = 0; i < 2; i++) {
		SetPolygonSize(m_vSize[i]);
		SetPolygonPos(m_vPos[i]);
		SetPolygonAlpha(g_fAlphaAlt);
		SetPolygonTexture(m_pTexture[i]);
		SetPolygonUV(0.0f, 0.0f);
		DrawPolygon(pDC);
	}
}

//これを呼び出して画面を赤くする
void AlertWarning(bool Flg) {
	//画面外の情報をPlayerからいただく
	StartAFlg = Flg;

	//画面外情報が「外(true)」の時に処理を行う
	if (StartAFlg) {
		if (g_fAlphaAlt <= 0.9f && AltFlg) {
			g_fAlphaAlt += 0.007f;
		}
		else {
			AltFlg = false;
		}

		if (g_fAlphaAlt >= 0.5f && !AltFlg) {
			g_fAlphaAlt -= 0.007f;
		}
		else {
			AltFlg = true;
		}
	}

	//画面外情報が「中(false)」の時に処理を行う
	if (!StartAFlg) {
		//画面内に戻った後、画面を赤から無色に戻す処理を行う
		if (g_fAlphaAlt > 0.0f) {
			g_fAlphaAlt -= 0.01f;
		}
		else if (g_fAlphaAlt <= 0.0f) {
			//フラグ情報を戻して次に画面外に出た時に
			//だんだん赤くなる処理から始める
			AltFlg = true;
		}
	}
}