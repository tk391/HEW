#pragma once
#include "ControlUI.h"
#include "Texture.h"
#include "polygon.h"
#include "debugproc.h"
#include "Scene.h"
#include "Player.h"
#include "Game.h"
#include "Sound.h"
//デバッグ確認用
#include "input.h"

//=====================================================
//マクロ定義
//=====================================================
#define PATH_CONTROLUI_TEXTURE  L"data/texture/operationXbox.png"
#define TEXTURE_SIZEX  (1724.0f * 0.28f) //画像サイズ
#define TEXTURE_SIZEY	(1288.0f * 0.28f)

#define CENTER_POS_X	(-400.0f)	//中心座標X
#define CENTER_POS_Y	(170.0f)	//中心座標Y

//=====================================================
//グローバル変数
//=====================================================
static ID3D11ShaderResourceView* g_pTexture;

//=============================================================================
//初期化
//=============================================================================
HRESULT ControlUI::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	//テクスチャ読み込み
	hr = CreateTextureFromFile(pDevice, PATH_CONTROLUI_TEXTURE, &g_pTexture);
	if (FAILED(hr))
	{
		return hr;
	}

	m_Pos = XMFLOAT2(CENTER_POS_X, CENTER_POS_Y);			//場所
	m_Size = XMFLOAT2(TEXTURE_SIZEX, TEXTURE_SIZEY);		//大きさ
	m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);				//色

	//音量のセット	0130三輪卓弘
	CSound::SetVolume(SE_CONTROLUIPOP, 80.0f);

	return hr;
}
//=============================================================================
//終了
//=============================================================================
void ControlUI::Uninit()
{
		SAFE_RELEASE(g_pTexture);
}
//=============================================================================
//更新
//=============================================================================
void ControlUI::Update()
{
	int i = 0;

	if (GetJoyTrigger(0, XBOXSCREENSHOT)) //ボタンを入力した瞬間
	{
		//SE再生
		CSound::Stop(SE_CONTROLUIPOP);
		CSound::Play(SE_CONTROLUIPOP);
		if (m_Color.w == 1.0f)	//操作方法UIが不透明ならば
		{
			m_Color.w = 0.0f;	//透明に(表示)
		}
		else if (m_Color.w == 0.0f)	//操作方法UIが透明ならば
		{
			m_Color.w = 1.0f;	//不透明に(非表示)
		}
	}

#if _DEBUG
	PrintDebugProc("m_Color.w={%.2f}\n", m_Color.w);
#endif
}
//=============================================================================
//描画
//=============================================================================
void ControlUI::Draw()
{
	ID3D11DeviceContext* pDC = GetDeviceContext();
	//スタートテロップ
	SetPolygonSize(m_Size.x, m_Size.y);
	SetPolygonPos(m_Pos.x, m_Pos.y);
	SetPolygonAlpha(m_Color.w);
	SetPolygonTexture(g_pTexture);
	SetPolygonUV(0.0f, 0.0f);
	DrawPolygon(pDC);
}
